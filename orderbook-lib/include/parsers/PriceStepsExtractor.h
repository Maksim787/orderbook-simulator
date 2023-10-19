#pragma once

#include <string>
#include <unordered_set>
#include <vector>

class PriceStepsExtractor {
private:
    // Save filename
    constexpr static const char* INFO_FILENAME = "info.txt";
    // Ratio of minimum price in %
    constexpr static double MIN_PRICE_STEP_FACTOR_PCT = 1e-6;

    // Path to order log
    std::string m_orderbook_log_path;
    // Path to results file
    std::string m_info_path;
    // Line counter
    int m_line_count = 0;

    // Prices from orders: are computed in CallBack
    std::unordered_set<double> prices_set;

    // Prices are computed in ComputeAndSaveResults() or loaded in LoadResults()
    std::vector<double> m_prices;
    // Prices differences are computed in ComputeDifferences()
    std::vector<double> m_prices_differences;
    // PxStep is computed in ComputePxStep()
    double m_px_step = 0.0;
public:
    explicit PriceStepsExtractor(const std::string& instrument_directory, const std::string& orderbook_log_filename = "");

    // Main function to extract px_step (force_recompute=false to get from cache)
    double ExtractPxStep(bool force_recompute = false);

    void PrintResults(bool verbose = false) const;

private:
    // Is called by Reader
    void CallBack(const char* data, size_t len);

    // Compute m_prices, m_prices_differences, m_px_step; Save them
    void ComputeAndSaveResults();

    // Load m_prices, m_px_step; Compute m_prices_differences
    void LoadResults();

    // Compute m_prices_differences
    std::vector<double> ComputeDifferences() const;

    // Compute px_step
    double ComputePxStep() const;
};
