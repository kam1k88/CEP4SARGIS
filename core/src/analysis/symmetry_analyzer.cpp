#include "core/analysis/symmetry_analyzer.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace curvefit {

SymmetryAnalyzer::SymmetryResult SymmetryAnalyzer::analyze(
    const std::vector<double>& residuals, int numBins) {
    SymmetryResult result;
    if (residuals.empty()) return result;

    const size_t n = residuals.size();
    const double mean = std::accumulate(residuals.begin(), residuals.end(), 0.0) / n;

    double varSum = 0;
    for (double r : residuals) {
        const double d = r - mean;
        varSum += d * d;
    }
    const double std = std::sqrt((n > 1) ? varSum / (n - 1) : 0);

    double m3 = 0;
    if (std > 1e-14) {
        for (double r : residuals) {
            const double z = (r - mean) / std;
            m3 += z * z * z;
        }
        m3 /= n;
    }
    result.skewness = m3;

    const double minVal = *std::min_element(residuals.begin(), residuals.end());
    const double maxVal = *std::max_element(residuals.begin(), residuals.end());
    const double range = std::max(maxVal - minVal, 1e-14);
    const double binWidth = range / numBins;

    std::vector<int> bins(static_cast<size_t>(numBins), 0);
    for (double r : residuals) {
        int idx = static_cast<int>((r - minVal) / range * numBins);
        if (idx >= numBins) idx = numBins - 1;
        if (idx < 0) idx = 0;
        ++bins[static_cast<size_t>(idx)];
    }

    for (int i = 0; i < numBins; ++i) {
        const double center = minVal + (i + 0.5) * binWidth;
        result.histogram.emplace_back(center, bins[static_cast<size_t>(i)]);
    }

    const double mid = (minVal + maxVal) / 2;
    for (int i = 0; i < numBins; ++i) {
        const double center = minVal + (i + 0.5) * binWidth;
        const double mirrored = 2 * mid - center;
        result.mirroredHistogram.emplace_back(mirrored, bins[static_cast<size_t>(i)]);
    }

    return result;
}

} // namespace curvefit
