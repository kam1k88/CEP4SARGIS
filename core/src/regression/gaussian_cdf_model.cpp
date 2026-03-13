#include "core/regression/gaussian_cdf_model.hpp"
#include <cmath>
#include <ceres/ceres.h>

namespace curvefit {

namespace {

constexpr double SQRT2 = 1.4142135623730950488;
constexpr double SQRT2PI = 2.5066282746310005024;

} // anonymous

std::string GaussianCDFModel::name() const {
    return "Gaussian CDF";
}

size_t GaussianCDFModel::paramCount() const {
    return 2;
}

std::vector<std::string> GaussianCDFModel::paramNames() const {
    return {"mu", "sigma"};
}

double GaussianCDFModel::evaluate(double x, const double* params) const {
    const double mu = params[0];
    const double sigma = params[1];
    if (sigma <= 0) return 0.5;
    const double z = (x - mu) / (sigma * SQRT2);
    return 0.5 * (1.0 + std::erf(z));
}

void GaussianCDFModel::derivatives(double x, const double* params, double* grad) const {
    const double mu = params[0];
    const double sigma = params[1];
    const double dx = x - mu;
    const double sigma2 = sigma * sigma;
    const double arg = -dx * dx / (2.0 * sigma2);
    const double expArg = std::exp(arg);
    const double factor = 1.0 / (sigma * SQRT2PI);

    // ∂f/∂mu = -exp(-(x-mu)^2/(2*sigma^2)) / (sigma * sqrt(2*pi))
    grad[0] = -expArg * factor;

    // ∂f/∂sigma = (x-mu) * exp(-(x-mu)^2/(2*sigma^2)) / (sigma^2 * sqrt(2*pi))
    grad[1] = dx * expArg * factor / sigma;
}

namespace {

class GaussianCDFCostFunctor : public ceres::SizedCostFunction<1, 2> {
public:
    GaussianCDFCostFunctor(double x, double y) : x_(x), y_(y) {}

    bool Evaluate(double const* const* params, double* residuals, double** jacobians) const override {
        const double mu = params[0][0];
        const double sigma = params[0][1];
        if (sigma <= 1e-10) {
            residuals[0] = 1e10;
            return true;
        }
        const double z = (x_ - mu) / (sigma * SQRT2);
        const double f = 0.5 * (1.0 + std::erf(z));
        residuals[0] = f - y_;

        if (jacobians && jacobians[0]) {
            const double dx = x_ - mu;
            const double sigma2 = sigma * sigma;
            const double expArg = std::exp(-dx * dx / (2.0 * sigma2));
            const double factor = 1.0 / (sigma * SQRT2PI);
            jacobians[0][0] = -expArg * factor;
            jacobians[0][1] = dx * expArg * factor / sigma;
        }
        return true;
    }

private:
    double x_;
    double y_;
};

} // anonymous

ceres::CostFunction* GaussianCDFModel::createCostFunction(double x, double y) const {
    return new GaussianCDFCostFunctor(x, y);
}

} // namespace curvefit
