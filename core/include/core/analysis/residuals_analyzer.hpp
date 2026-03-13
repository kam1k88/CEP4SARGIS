#pragma once

#include "core/types.hpp"
#include "core/regression/model_interface.hpp"
#include <vector>

namespace SargisLab {

class ResidualsAnalyzer {
public:
    /// Compute residuals r_i = y_i - f(x_i)
    ResidualsResult analyze(const Dataset& data, IRegressionModel* model,
                            const std::vector<double>& params);

    /// Linear regression of residuals vs x (trend check): r = a*x + b
    void residualsVsXRegression(const std::vector<double>& x,
                                const std::vector<double>& residuals,
                                double& slope, double& intercept, double& r2);

    /// Build QQ-plot data: (theoretical quantile, sample quantile)
    static std::vector<std::pair<double, double>> computeQQPlot(
        const std::vector<double>& residuals);
};

} // namespace SargisLab
