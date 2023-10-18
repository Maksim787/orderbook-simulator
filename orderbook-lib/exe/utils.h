#pragma once

#include <parsers/FileReader.h>

const std::string FILENAME = "data/examples/SE/OrderLog20200103.txt";
const std::string MULTIPLE_INSTRUMENTS_HEADER = "NO,SECCODE,BUYSELL,TIME,ORDERNO,ACTION,PRICE,VOLUME,TRADENO,TRADEPRICE";
const std::string ONE_INSTRUMENT_HEADER = "NO,BUYSELL,TIME,ORDERNO,ACTION,PRICE,VOLUME,TRADENO,TRADEPRICE";

template <typename Callback>
void run_reader(FileReader<Callback>& reader) {
    auto start = std::chrono::high_resolution_clock::now();
    reader.Iterate();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Execution time: " << duration.count() << " seconds\n";
    std::cout << "Success\n";
}