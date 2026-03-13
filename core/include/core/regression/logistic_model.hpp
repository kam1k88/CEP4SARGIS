#pragma once

#include "model_interface.hpp"
#include <vector>
#include <string>

namespace SargisLab {

/// Logistic model: f(x) = L / (1 + exp(-k*(x-x0)))
/// Parameters: L (carrying capacity), k (rate), x0 (midpoint)
class LogisticModel : public IRegressionModel {
public:
    std::string name() const override;
    size_t paramCount() const override;
    std::vector<std::string> paramNames() const override;

    double evaluate(double x, const double* params) const override;
    void derivatives(double x, const double* params, double* grad) const override;
    ceres::CostFunction* createCostFunction(double x, double y) const override;
};

} // namespace SargisLab
