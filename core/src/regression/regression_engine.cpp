#include "core/regression/regression_engine.hpp"
#include "core/regression/gaussian_cdf_model.hpp"
#include "core/regression/polynomial_model.hpp"
#include "core/regression/exponential_model.hpp"
#include "core/regression/logistic_model.hpp"
#include <ceres/ceres.h>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>

namespace curvefit {

struct RegressionEngine::Impl {
    std::vector<IRegressionModel*> externalModels;
    std::unique_ptr<GaussianCDFModel> gaussianCdf;
    std::unique_ptr<PolynomialModel> polynomial2;
    std::unique_ptr<PolynomialModel> polynomial3;
    std::unique_ptr<ExponentialModel> exponential;
    std::unique_ptr<LogisticModel> logistic;
};

RegressionEngine::RegressionEngine() : impl_(std::make_unique<Impl>()) {}

RegressionEngine::~RegressionEngine() = default;

void RegressionEngine::registerBuiltinModels() {
    impl_->gaussianCdf = std::make_unique<GaussianCDFModel>();
    impl_->polynomial2 = std::make_unique<PolynomialModel>(2);
    impl_->polynomial3 = std::make_unique<PolynomialModel>(3);
    impl_->exponential = std::make_unique<ExponentialModel>();
    impl_->logistic = std::make_unique<LogisticModel>();
}

void RegressionEngine::addModel(IRegressionModel* model) {
    if (model) {
        impl_->externalModels.push_back(model);
    }
}

std::vector<IRegressionModel*> RegressionEngine::models() const {
    std::vector<IRegressionModel*> result;
    if (impl_->gaussianCdf) result.push_back(impl_->gaussianCdf.get());
    if (impl_->polynomial2) result.push_back(impl_->polynomial2.get());
    if (impl_->polynomial3) result.push_back(impl_->polynomial3.get());
    if (impl_->exponential) result.push_back(impl_->exponential.get());
    if (impl_->logistic) result.push_back(impl_->logistic.get());
    for (auto* m : impl_->externalModels) {
        result.push_back(m);
    }
    return result;
}

FitResult RegressionEngine::fit(const Dataset& data, IRegressionModel* model,
                                const std::vector<double>& initialParams) {
    FitResult result;
    if (!model || data.size() < 2) {
        result.message = "Invalid input";
        return result;
    }

    const size_t n = model->paramCount();
    if (initialParams.size() != n) {
        result.message = "Initial parameters count mismatch";
        return result;
    }

    std::vector<double> params = initialParams;

    ceres::Problem problem;

    for (size_t i = 0; i < data.size(); ++i) {
        ceres::CostFunction* cost = model->createCostFunction(data.x[i], data.y[i]);
        problem.AddResidualBlock(cost, nullptr, params.data());
    }

    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_type = ceres::TRUST_REGION;
    options.trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT;
    options.max_num_iterations = 200;
    options.function_tolerance = 1e-10;
    options.parameter_tolerance = 1e-10;

    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);

    result.iterations = summary.iterations.size();
    result.success = (summary.termination_type == ceres::CONVERGENCE);
    result.message = summary.BriefReport();
    result.parameters = params;

    if (result.success) {
        double ss_res = 0;
        double y_mean = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            y_mean += data.y[i];
        }
        y_mean /= data.size();

        double ss_tot = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            const double pred = model->evaluate(data.x[i], params.data());
            const double r = data.y[i] - pred;
            ss_res += r * r;
            ss_tot += (data.y[i] - y_mean) * (data.y[i] - y_mean);
        }

        result.rmse = std::sqrt(ss_res / data.size());
        result.r_squared = (ss_tot > 1e-14) ? (1.0 - ss_res / ss_tot) : 1.0;
    }

    return result;
}

} // namespace curvefit
