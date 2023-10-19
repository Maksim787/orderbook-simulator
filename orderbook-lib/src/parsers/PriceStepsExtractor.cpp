#include <parsers/PriceStepsExtractor.h>

#include <parsers/FileReader.h>
#include <parsers/LineParsers.h>
#include <parsers/Constants.h>
#include <orderbook/Event.h>

#include <algorithm>
#include <set>
#include <filesystem>

namespace fs = std::filesystem;

PriceStepsExtractor::PriceStepsExtractor(const std::string& instrument_directory, const std::string& orderbook_log_filename) {
    assert(fs::exists(instrument_directory));
    if (!orderbook_log_filename.empty()) {
        // Use specified log file
        m_orderbook_log_path = (fs::path(instrument_directory) / orderbook_log_filename).string();
    } else {
        // Take first filename with logs
        for (const auto& log_entry: fs::directory_iterator(instrument_directory)) {
            if (log_entry.path().filename() != INFO_FILENAME) {
                m_orderbook_log_path = log_entry.path().string();
                break;
            }
        }
        assert(!m_orderbook_log_path.empty());
    }
    m_info_path = (fs::path(instrument_directory) / INFO_FILENAME).string();
}

double PriceStepsExtractor::ExtractPxStep(bool force_recompute) {
    // Check if result exists
    if (!fs::exists(m_info_path) || force_recompute) {
        // Run reader to store prices via CallBack
        FileReader reader(
                m_orderbook_log_path,
                [this](const char* data, size_t len) { this->CallBack(data, len); },
                ONE_INSTRUMENT_HEADER
        );
        reader.Iterate();
        // Calculate px_step and store results
        ComputeAndSaveResults();
    } else {
        // Load existing results
        LoadResults();
    }
    return m_px_step;
}

void PriceStepsExtractor::CallBack(const char* data, size_t len) {
    // Parse event
    auto event = parse_event_from_line<Event>(data, len);

    // Add prices to set
    if (event.px != 0) {
        prices_set.insert(event.px);
    }
    if (event.action == Action::TRADE) {
        prices_set.insert(event.trade_px);
    }

    // Check event id
    if (event.id != m_line_count + 1) {
        throw std::runtime_error("Expected ID: " + std::to_string(m_line_count + 1) + "; Got: " + std::to_string(event.id));
    }
    ++m_line_count;
}

void PriceStepsExtractor::ComputeAndSaveResults() {
    // Sort prices
    m_prices = std::vector<double>(prices_set.begin(), prices_set.end());
    std::sort(m_prices.begin(), m_prices.end());

    // Compute differences
    m_prices_differences = ComputeDifferences();

    // Find price step
    m_px_step = ComputePxStep();

    // Write results
    std::ofstream file(m_info_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open the file for writing: \"" + m_info_path + "\"");
    }
    file << "prices (" << m_prices.size() << "): ";
    for (double px: m_prices) {
        file << px << " ";
    }
    file << "\n";
    file << "px_step: " << m_px_step << "\n";
}

void PriceStepsExtractor::LoadResults() {
    // Check that state is empty
    assert(m_prices.empty());

    // Open file
    std::ifstream file(m_info_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open the file for reading: \"" + m_info_path + "\"");
    }
    // Read prices size
    std::string tmp;
    char tmp_char;
    file >> tmp;
    assert(tmp == "prices" && "Cannot parse prices size");
    file >> tmp_char;
    assert(tmp_char == '(' && "Cannot parse prices size");

    size_t px_size;
    file >> px_size >> tmp;
    assert(tmp == "):" && "Cannot parse prices size");
    // Read prices
    m_prices.resize(px_size);
    for (int i = 0; i < px_size; ++i) {
        file >> m_prices[i];
    }
    assert(std::is_sorted(m_prices.begin(), m_prices.end()));
    // Read px_step
    file >> tmp;
    assert(tmp == "px_step:" && "Cannot parse px_step");
    file >> m_px_step;

    // Compute differences
    m_prices_differences = ComputeDifferences();
}

void PriceStepsExtractor::PrintResults(bool verbose) const {
    assert(!m_prices.empty());

    if (verbose) {
        // Print prices
        std::cout << "prices:\n";
        for (double px: m_prices) {
            std::cout << px << " ";
        }
        std::cout << "\n";
        // Print differences
        std::cout << "differences:\n";
        for (double diff: m_prices_differences) {
            std::cout << diff << " ";
        }
        std::cout << "\n";
    }

    // Print price step
    std::cout << "px_step: " << m_px_step << "\n";
}

std::vector<double> PriceStepsExtractor::ComputeDifferences() const {
    std::set<double> differences_set;
    for (size_t i = 0; i + 1 < m_prices.size(); ++i) {
        differences_set.insert(m_prices[i + 1] - m_prices[i]);
    }
    // Already sorted
    return {differences_set.begin(), differences_set.end()};
}

double PriceStepsExtractor::ComputePxStep() const {
    assert(m_prices.front() > 0);
    assert(std::is_sorted(m_prices_differences.begin(), m_prices_differences.end()));

    // Average minimum prices differences that differ from 0 and do not differ from each other
    std::vector<double> possible_px_steps;
    double min_possible_px_step = MIN_PRICE_STEP_FACTOR_PCT * m_prices.front() / 100;
    double prev_diff = 0.0;
    for (double diff: m_prices_differences) {
        if (diff < min_possible_px_step) {
            // Difference is almost 0, it is a computational error
            std::cerr << "Warning: Skip diff = " << diff << ". min_possible_px_step = " << min_possible_px_step << "\n";
            continue;
        }
        if (prev_diff != 0.0 && diff - prev_diff > min_possible_px_step) {
            // Difference is almost 0
            break;
        }
        // Prices differences are almost the same, average them
        possible_px_steps.push_back(diff);
        prev_diff = diff;
    }
    if (possible_px_steps.empty()) {
        // Not enough observations
        std::cerr << "Warning: Unable to compute px_step\n";
        return 0.0;
    }
    // Average possible price steps
    double result_px_step = 0.0;
    for (double diff: possible_px_steps) {
        result_px_step += diff / static_cast<double>(possible_px_steps.size());
    }
    return result_px_step;
}
