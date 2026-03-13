#include "core/regression/logistic_model.hpp"
#include <ceres/ceres.h>
#include <cmath>

namespace curvefit {

std::string LogisticModel::name() const {
    return "Logistic (L/(1+exp(-k*(x-x0))))";
}

size_t LogisticModel::paramCount() const {
    return 3;
}

std::vector<std::string> LogisticModel::paramNames() const {
    return {"L", "k", "x0"};
}

double LogisticModel::evaluate(double x, const double* params) const {
    const double L = params[0];
    const double k = params[1];
    const double x0 = params[2];
    const double expVal = std::exp(-k * (x - x0));
    return L / (1.0 + expVal);
}

void LogisticModel::derivatives(double x, const double* params, double* grad) const {
    const double L = params[0];
    const double k = params[1];
    const double x0 = params[2];
    const double arg = -k * (x - x0);
    const double expVal = std::exp(arg);
    const double denom = 1.0 + expVal;
    const double denom2 = denom * denom;

    // ∂f/∂L = 1/(1+exp)
    grad[0] = 1.0 / denom;

    // ∂f/∂k = L * (x-x0) * exp / denom^2
    grad[1] = L * (x - x0) * expVal / denom2;

    // ∂f/∂x0 = -L * k * exp / denom^2
    grad[2] = -L * k * expVal / denom2;
}

namespace {

class LogisticCostFunction : public ceres::SizedCostFunction<1, 3> {
public:
    LogisticCostFunction(double x, double y) : x_(x), y_(y) {}

    bool Evaluate(double const* const* params, double* residuals, double** jacobians) const override {
        const double L = params[0][0];
        const double k = params[0][1];
        const double x0 = params[0][2];
        const double expVal = std::exp(-k * (x_ - x0));
        const double denom = 1.0 + expVal;
        const double f = L / denom;
        residuals[0] = f - y_;

        if (jacobians && jacobians[0]) {
            const double denom2 = denom * denom;
            jacobians[0][0] = 1.0 / denom;
            jacobians[0][1] = L * (x_ - x0) * expVal / denom2;
            jacobians[0][2] = -L * k * expVal / denom2;
        }
        return true;
    }

private:
    double x_;
    double y_;
};

} // anonymous

ceres::CostFunction* LogisticModel::createCostFunction(double x, double y) const {
    return new LogisticCostFunction(x, y);
}

} // namespace curvefit
