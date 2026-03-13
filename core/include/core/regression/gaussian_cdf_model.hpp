#pragma once

#include "model_interface.hpp"
#include <vector>
#include <string>

namespace curvefit {

/// Gaussian CDF model: f(x) = 0.5 * (1 + erf((x - mu) / (sigma * sqrt(2))))
/// Parameters: mu, sigma (sigma > 0)
class GaussianCDFModel : public IRegressionModel {
public:
    std::string name() const override;
    size_t paramCount() const override;
    std::vector<std::string> paramNames() const override;

    double evaluate(double x, const double* params) const override;
    void derivatives(double x, const double* params, double* grad) const override;
    ceres::CostFunction* createCostFunction(double x, double y) const override;
};

} // namespace curvefit
