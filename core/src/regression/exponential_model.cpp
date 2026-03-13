#include "core/regression/exponential_model.hpp"
#include <ceres/ceres.h>
#include <cmath>

namespace curvefit {

std::string ExponentialModel::name() const {
    return "Exponential (a*exp(b*x)+c)";
}

size_t ExponentialModel::paramCount() const {
    return 3;
}

std::vector<std::string> ExponentialModel::paramNames() const {
    return {"a", "b", "c"};
}

double ExponentialModel::evaluate(double x, const double* params) const {
    const double a = params[0];
    const double b = params[1];
    const double c = params[2];
    return a * std::exp(b * x) + c;
}

void ExponentialModel::derivatives(double x, const double* params, double* grad) const {
    const double a = params[0];
    const double b = params[1];
    const double expbx = std::exp(b * x);
    grad[0] = expbx;
    grad[1] = a * x * expbx;
    grad[2] = 1.0;
}

namespace {

class ExponentialCostFunction : public ceres::SizedCostFunction<1, 3> {
public:
    ExponentialCostFunction(double x, double y) : x_(x), y_(y) {}

    bool Evaluate(double const* const* params, double* residuals, double** jacobians) const override {
        const double a = params[0][0];
        const double b = params[0][1];
        const double c = params[0][2];
        const double expbx = std::exp(b * x_);
        const double f = a * expbx + c;
        residuals[0] = f - y_;

        if (jacobians && jacobians[0]) {
            jacobians[0][0] = expbx;
            jacobians[0][1] = a * x_ * expbx;
            jacobians[0][2] = 1.0;
        }
        return true;
    }

private:
    double x_;
    double y_;
};

} // anonymous

ceres::CostFunction* ExponentialModel::createCostFunction(double x, double y) const {
    return new ExponentialCostFunction(x, y);
}

} // namespace curvefit
