#pragma once

#include "core/types.hpp"
#include <vector>

namespace curvefit {

class SymmetryAnalyzer {
public:
    /// Analyze symmetry of residual distribution
    /// Returns skewness (0 = symmetric) and histogram bins for visualization
    struct SymmetryResult {
        double skewness{0};
        std::vector<std::pair<double, int>> histogram;
        std::vector<std::pair<double, int>> mirroredHistogram;
    };

    SymmetryResult analyze(const std::vector<double>& residuals, int numBins = 20);
};

} // namespace curvefit
