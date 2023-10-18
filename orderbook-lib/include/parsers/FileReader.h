#pragma once

#include <iostream>
#include <fstream>
#include <string>

template <typename Callback>
class FileReader {
private:
    std::ifstream m_file;
    const std::string m_filename;
    const std::string m_header;
    const int m_limit_lines;
    int m_line_count = 0;

    Callback m_callback;

    std::string m_line_content;

public:
    FileReader(std::string filename, Callback callback, std::string header = "", int limit_lines=-1);

    void Iterate();

    ~FileReader();
};

#include <parsers/FileReader.hpp>