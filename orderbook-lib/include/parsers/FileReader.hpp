#include <utility>
#include <vector>
#include <functional>
#include <filesystem>


template <typename Callback>
FileReader<Callback>::FileReader(std::string filename, Callback callback, std::string header, int limit_lines)
        : m_filename(std::move(filename)), m_callback(callback), m_header(std::move(header)), m_limit_lines(limit_lines) {}

template <typename Callback>
FileReader<Callback>::~FileReader() {
    // Close file
    if (m_file.is_open()) {
        m_file.close();
    }
}

template <typename Callback>
void FileReader<Callback>::Iterate() {
    // Open file
    m_file.open(m_filename);
    if (!m_file.is_open()) {
        throw std::runtime_error("Failed to open the file: \"" + m_filename + "\". Current working directory: " + std::filesystem::current_path().string());
    }

    // Read header
    if (!std::getline(m_file, m_line_content)) {
        throw std::runtime_error("Empty file");
    }
    if (m_line_content.back() == '\r') {
        m_line_content.pop_back();
    }
    if (!m_header.empty() && m_header != m_line_content) {
        throw std::runtime_error("Expected: \"" +
                                 m_header + "\" (" + std::to_string(m_header.size()) + ")\". Got: \"" +
                                 m_line_content + "\" (" + std::to_string(m_line_content.size()) + ")");
    }

    // Read lines
    // TODO: read by batches
    while (std::getline(m_file, m_line_content)) {
        if (m_line_content.back() == '\r') {
            m_line_content.pop_back();
        }
        if (m_line_content.empty()) {
            continue;
        }
        m_callback(m_line_content.data(), m_line_content.size());
        ++m_line_count;
        if (m_limit_lines == m_line_count) {
            break;
        }
    }
}
