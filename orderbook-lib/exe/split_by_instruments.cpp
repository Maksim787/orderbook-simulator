#include <parsers/FileReader.h>
#include <parsers/LineParsers.h>

#include "utils.h"

#include <iostream>
#include <filesystem>
#include <cassert>
#include <vector>

namespace fs = std::filesystem;

class Splitter {
private:
    const std::string order_log_file;
    const std::string dst_directory;
    int line_count = 0;
    struct InstrumentInfo {
        int curr_id = 1;
        std::ofstream file;
    };
    std::unordered_map<std::string, InstrumentInfo> file_by_instrument;

public:
    Splitter(std::string order_log_file, std::string dst_directory)
            : order_log_file(std::move(order_log_file)), dst_directory(std::move(dst_directory)) {}

    std::unordered_map<std::string, InstrumentInfo>::iterator create_instrument(const char* sec_code) {
        // Construct the instrument folder
        fs::path instrument_path = fs::path(dst_directory) / sec_code;
        if (!fs::exists(instrument_path)) {
            errno = 0; // some bug with fs::exists
            fs::create_directory(instrument_path);
            std::cout << "Created folder for instrument: \"" << sec_code << "\"\n";
        }
        // Construct the order log file
        fs::path log_file = instrument_path / order_log_file;
        std::ofstream file = std::ofstream(log_file.string());
        if (!file.is_open()) {
            throw std::runtime_error("Error opening the file: \"" + log_file.string() + "\"");
        }
        file << ONE_INSTRUMENT_HEADER << '\n';
        file_by_instrument[sec_code] = InstrumentInfo {.file=std::move(file)};
        return file_by_instrument.find(sec_code);
    }

    void write_info_to_file(const EventWithInstrument& event, InstrumentInfo& info) {
        info.file << info.curr_id << ',';
        event.Serialize(info.file);
        ++info.curr_id;
    }

    void callback(const char* data, size_t len) {
        EventWithInstrument event = parse_line_with_sec_code(data, len);
        if (event.id != line_count + 1) {
            throw std::runtime_error("Expected ID: " + std::to_string(line_count + 1) + "; Got: " + std::to_string(event.id));
        }
        ++line_count;
        auto it = file_by_instrument.find(event.sec_code);
        if (it == file_by_instrument.end()) {
            // Create file for instrument
            it = create_instrument(event.sec_code);
        }
        auto&[sec_code, info] = *it;
        write_info_to_file(event, info);
    }
};

int main() {
    const std::string src_folder = "data/examples/SE";
    const std::string dst_folder = src_folder + "_by_instrument";

    std::cout << "Source: " << src_folder << "\n";
    std::cout << "Destination: " << dst_folder << "\n";

    // Read source files
    std::vector<std::string> files;
    for (const auto& entry: fs::directory_iterator(src_folder)) {
        assert(entry.is_regular_file());
        files.push_back(entry.path().filename().string());
    }
    std::sort(files.begin(), files.end());

    std::cout << "Source content: " << files.size() << " files:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << i + 1 << ". " << files[i] << "\n";
    }

    // Create destination folder
    if (!fs::exists(dst_folder)) {
        errno = 0; // some bug with fs::exists
        if (!fs::create_directory(dst_folder)) {
            throw std::runtime_error("Failed to create the destination folder: " + dst_folder);
        }
        std::cout << "Destination folder was created\n";
    } else {
        std::cout << "Destination folder exists\n";
    }

    // Process each file
    for (const std::string& file: files) {
        std::cout << "Process " << file << "\n";
        Splitter splitter(file, dst_folder);
        FileReader reader(FILENAME, [&splitter](const char* data, size_t len) { splitter.callback(data, len); }, MULTIPLE_INSTRUMENTS_HEADER);
        run_reader(reader);
    }

    return 0;
}