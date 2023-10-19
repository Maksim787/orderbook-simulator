#include <parsers/FileReader.h>
#include <utils.h>

#include <iostream>
#include <chrono>

class DummyParser {
private:
    const int n_print_lines;
    int line_count = 0;
public:
    explicit DummyParser(int n_print_lines) : n_print_lines(n_print_lines) {}

    void CallBack(const char* data, size_t len) {
        if (line_count < n_print_lines) {
            std::cout << len << ": " << data << "\n";
        }
        ++line_count;
    }
};

int main() {
    const int n_print_lines = 10;
    DummyParser parser(n_print_lines);
    FileReader reader(
            MULTIPLE_INSTRUMENTS_TEST_FILENAME,
            [&parser](const char* data, size_t len) { parser.CallBack(data, len); },
            MULTIPLE_INSTRUMENTS_HEADER
    );

    RunReaderWithTimeCheck(reader);

    return 0;
}