#include <parsers/FileReader.h>
#include <parsers/LineParsers.h>
#include <utils.h>

#include <iostream>

class EventParser {
private:
    const int n_print_lines;
    int line_count = 0;

public:
    EventParser(int n_print_lines) : n_print_lines(n_print_lines) {}

    void CallBack(const char* data, size_t len) {
        auto event = parse_event_from_line<EventWithInstrument>(data, len);
        if (line_count < n_print_lines) {
            std::cout << len << ": " << std::string(data, len) << "\n";
            std::cout << event;
        }
        if (event.id != line_count + 1) {
            throw std::runtime_error("Expected ID: " + std::to_string(line_count + 1) + "; Got: " + std::to_string(event.id));
        }
        ++line_count;
    }
};

int main() {
    const int n_print_lines = 1;
    EventParser parser(n_print_lines);
    FileReader reader(
            MULTIPLE_INSTRUMENTS_TEST_FILENAME,
            [&parser](const char* data, size_t len) { parser.CallBack(data, len); },
            MULTIPLE_INSTRUMENTS_HEADER
    );

    RunReaderWithTimeCheck(reader);

    return 0;
}