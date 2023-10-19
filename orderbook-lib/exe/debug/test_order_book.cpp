#include <parsers/FileReader.h>
#include <parsers/LineParsers.h>
#include <parsers/PriceStepsExtractor.h>
#include <orderbook/OrderBook.h>
#include <utils.h>

#include <iostream>

namespace fs = std::filesystem;

class OrderBookUpdater {
private:
    OrderBook<ORDERBOOK_MAX_SIZE> orderbook;
    const int n_print_lines;
    int line_count = 0;

public:
    OrderBookUpdater(double px_step, int n_print_lines) : orderbook(px_step), n_print_lines(n_print_lines) {}

    void Callback(const char* data, size_t len) {
        auto event = parse_event_from_line<Event>(data, len);
        orderbook.Update(event);
        if (line_count < n_print_lines) {
            std::cout << event;
            orderbook.Print();
        }
        if (event.id != line_count + 1) {
            throw std::runtime_error("Expected ID: " + std::to_string(line_count + 1) + "; Got: " + std::to_string(event.id));
        }
        ++line_count;
    }
};

int main() {
    const int n_print_lines = 1;
    const std::string instrument_test_filename = SINGLE_INSTRUMENT_TEST_FILENAME;
    const fs::path instrument_directory = fs::path(instrument_test_filename).parent_path();

    // Calculate price step
    PriceStepsExtractor extractor(instrument_directory.string());
    double px_step = extractor.ExtractPxStep();
    std::cout << instrument_directory.filename().string() << ": px_step = " << px_step << "\n";

    // Create orderbook
    OrderBookUpdater updater(px_step, n_print_lines);
    FileReader reader(
            instrument_test_filename,
            [&updater](const char* data, size_t len) { updater.Callback(data, len); },
            ONE_INSTRUMENT_HEADER,
            100
    );

    RunReaderWithTimeCheck(reader);

    return 0;
}
