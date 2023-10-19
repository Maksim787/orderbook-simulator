#pragma once

#include <parsers/FileReader.h>
#include <parsers/Constants.h>


template <typename Callback>
void RunReaderWithTimeCheck(FileReader<Callback>& reader) {
    auto start = std::chrono::high_resolution_clock::now();
    reader.Iterate();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Execution time: " << duration.count() << " seconds\n";
    std::cout << "Success\n";
}