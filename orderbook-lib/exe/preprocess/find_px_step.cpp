#include <parsers/PriceStepsExtractor.h>
#include <utils.h>
#include <cassert>

namespace fs = std::filesystem;

void PrintInstruments(const std::vector<std::string>& instruments) {
    if (instruments.empty()) {
        std::cout << "(0)\n";
        return;
    }
    std::cout << "(" << instruments.size() << "): [";
    for (size_t i = 0; i + 1 < instruments.size(); ++i) {
        std::cout << instruments[i] << ", ";
    }
    std::cout << instruments.back() << "]\n";
}

int main() {
    const bool FORCE_RECOMPUTE = true;
    const std::string src_directory = "data/examples/SE_by_instrument";
    assert(fs::exists(src_directory));

    // Find instruments with minimum px_step
    double min_px_step = std::numeric_limits<double>::max();
    std::vector<std::string> instruments_with_min_px_step;
    std::vector<std::string> instruments_with_zero_px_step;
    // Iterate over instrument directories in src_directory
    for (const auto& instrument_entry: fs::directory_iterator(src_directory)) {
        std::cout << "Process " << instrument_entry.path().string() << "\n";
        // Calculate PxStep
        PriceStepsExtractor extractor(instrument_entry.path().string());
        double px_step = extractor.ExtractPxStep(FORCE_RECOMPUTE);

        // Print results
        extractor.PrintResults();
        // Store zero px_step
        if (px_step == 0.0) {
            instruments_with_zero_px_step.push_back(instrument_entry.path().filename());
        } else {
            // Store minimum px_step
            if (px_step < min_px_step) {
                instruments_with_min_px_step.clear();
            }
            if (px_step <= min_px_step) {
                instruments_with_min_px_step.push_back(instrument_entry.path().filename());
                min_px_step = px_step;
            }
        }
    }
    std::cout << "Minimum px_step = " << min_px_step << "\n";

    std::cout << "Instruments with minimum px_step: ";
    PrintInstruments(instruments_with_min_px_step);

    std::cout << "Instruments with zero px_step: ";
    PrintInstruments(instruments_with_zero_px_step);

    return 0;
}