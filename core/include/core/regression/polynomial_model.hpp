#pragma once

#include "model_interface.hpp"
#include <vector>
#include <string>

namespace curvefit {

/// Polynomial model: f(x) = sum_{i=0}^{n} a_i * x^i
/// Degree is fixed at construction (default 2 = quadratic)
class PolynomialModel : public IRegressionModel {
public:
    explicit PolynomialModel(int degree = 2);

    std::string name() const override;
    size_t paramCount() const override;
    std::vector<std::string> paramNames() const override;

    double evaluate(double x, const double* params) const override;
    void derivatives(double x, const double* params, double* grad) const override;
    ceres::CostFunction* createCostFunction(double x, double y) const override;

private:
    int degree_;
};

} // namespace curvefit
