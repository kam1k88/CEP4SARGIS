#pragma once

#include "model_interface.hpp"
#include <vector>
#include <string>

namespace SargisLab {

/// Exponential model: f(x) = a * exp(b * x) + c
/// Parameters: a, b, c
class ExponentialModel : public IRegressionModel {
public:
    std::string name() const override;
    size_t paramCount() const override;
    std::vector<std::string> paramNames() const override;

    double evaluate(double x, const double* params) const override;
    void derivatives(double x, const double* params, double* grad) const override;
    ceres::CostFunction* createCostFunction(double x, double y) const override;
};

} // namespace SargisLab
