#include <parsers/FileReader.h>
#include <parsers/LineParsers.h>
#include <utils.h>

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

    void CallBack(const char* data, size_t len) {
        auto event = parse_event_from_line<EventWithInstrument>(data, len);
        if (event.id != line_count + 1) {
            throw std::runtime_error("Expected ID: " + std::to_string(line_count + 1) + "; Got: " + std::to_string(event.id));
        }
        ++line_count;
        auto it = file_by_instrument.find(event.sec_code);
        if (it == file_by_instrument.end()) {
            // Create file for instrument
            it = CreateInstrument(event.sec_code);
        }
        WriteEventToFile(event, it->second);
    }

private:
    std::unordered_map<std::string, InstrumentInfo>::iterator CreateInstrument(const char* sec_code) {
        // Construct the instrument directory
        fs::path instrument_path = fs::path(dst_directory) / sec_code;
        if (!fs::exists(instrument_path)) {
            fs::create_directory(instrument_path);
            std::cout << "Created directory for instrument: \"" << sec_code << "\"\n";
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

    void WriteEventToFile(const EventWithInstrument& event, InstrumentInfo& info) {
        info.file << info.curr_id << ',';
        event.Serialize(info.file);
        ++info.curr_id;
    }
};

int main() {
    const std::string src_directory = "data/examples/SE";
    const std::string dst_directory = src_directory + "_by_instrument";

    std::cout << "Source: " << src_directory << "\n";
    std::cout << "Destination: " << dst_directory << "\n";

    // Read source files
    std::vector<std::string> files;
    for (const auto& entry: fs::directory_iterator(src_directory)) {
        assert(entry.is_regular_file());
        files.push_back(entry.path().filename().string());
    }
    std::sort(files.begin(), files.end());

    std::cout << "Source content: " << files.size() << " files:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << i + 1 << ". " << files[i] << "\n";
    }

    // Create destination directory
    if (!fs::exists(dst_directory)) {
        if (!fs::create_directory(dst_directory)) {
            throw std::runtime_error("Failed to create the destination directory: " + dst_directory);
        }
        std::cout << "Destination directory was created\n";
    } else {
        std::cout << "Destination directory exists\n";
    }

    // Process each file
    for (const std::string& file: files) {
        std::cout << "Process " << file << "\n";
        Splitter splitter(file, dst_directory);
        FileReader reader(
                MULTIPLE_INSTRUMENTS_TEST_FILENAME,
                [&splitter](const char* data, size_t len) { splitter.CallBack(data, len); },
                MULTIPLE_INSTRUMENTS_HEADER
        );
        RunReaderWithTimeCheck(reader);
    }

    return 0;
}