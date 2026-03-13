#include "core/regression/polynomial_model.hpp"
#include <ceres/ceres.h>
#include <sstream>

namespace curvefit {

PolynomialModel::PolynomialModel(int degree) : degree_(std::max(1, degree)) {}

std::string PolynomialModel::name() const {
    std::ostringstream oss;
    oss << "Polynomial (degree " << degree_ << ")";
    return oss.str();
}

size_t PolynomialModel::paramCount() const {
    return static_cast<size_t>(degree_ + 1);
}

std::vector<std::string> PolynomialModel::paramNames() const {
    std::vector<std::string> names;
    for (int i = 0; i <= degree_; ++i) {
        names.push_back("a" + std::to_string(i));
    }
    return names;
}

double PolynomialModel::evaluate(double x, const double* params) const {
    double result = 0;
    double xi = 1.0;
    for (int i = 0; i <= degree_; ++i) {
        result += params[i] * xi;
        xi *= x;
    }
    return result;
}

void PolynomialModel::derivatives(double x, const double* params, double* grad) const {
    double xi = 1.0;
    for (int i = 0; i <= degree_; ++i) {
        grad[i] = xi;
        xi *= x;
    }
}

namespace {

class PolynomialCostFunction : public ceres::CostFunction {
public:
    PolynomialCostFunction(const PolynomialModel* model, double x, double y)
        : model_(model), x_(x), y_(y) {
        set_num_residuals(1);
        mutable_parameter_block_sizes()->push_back(static_cast<int>(model->paramCount()));
    }

    bool Evaluate(double const* const* params, double* residuals, double** jacobians) const override {
        const double* p = params[0];
        const double f = model_->evaluate(x_, p);
        residuals[0] = f - y_;

        if (jacobians && jacobians[0]) {
            model_->derivatives(x_, p, jacobians[0]);
        }
        return true;
    }

private:
    const PolynomialModel* model_;
    double x_;
    double y_;
};

} // anonymous

ceres::CostFunction* PolynomialModel::createCostFunction(double x, double y) const {
    return new PolynomialCostFunction(this, x, y);
}

} // namespace curvefit
