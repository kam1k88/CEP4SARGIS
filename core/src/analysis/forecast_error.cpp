#include "core/analysis/forecast_error.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace SargisLab {

ForecastErrorResult ForecastError::evaluate(const Dataset& data, IRegressionModel* model,
                                            const std::vector<double>& params,
                                            double confidenceLevel) {
    ForecastErrorResult result;
    if (!model || data.empty()) return result;

    const double* p = params.data();
    std::vector<double> residuals;
    residuals.reserve(data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        const double pred = model->evaluate(data.x[i], p);
        residuals.push_back(data.y[i] - pred);
    }

    const size_t n = residuals.size();
    double rmseSum = 0, maeSum = 0, mapeSum = 0;
    for (size_t i = 0; i < n; ++i) {
        const double r = residuals[i];
        rmseSum += r * r;
        maeSum += std::abs(r);
        if (std::abs(data.y[i]) > 1e-14) {
            mapeSum += std::abs(r / data.y[i]);
        }
    }

    result.rmse = std::sqrt(rmseSum / n);
    result.mae = maeSum / n;
    result.mape = (mapeSum > 0) ? (mapeSum / n) * 100.0 : 0;

    const double meanR = std::accumulate(residuals.begin(), residuals.end(), 0.0) / n;
    double varSum = 0;
    for (double r : residuals) {
        const double d = r - meanR;
        varSum += d * d;
    }
    const double stdR = std::sqrt((n > 1) ? varSum / (n - 1) : 0);

    const double z = (confidenceLevel <= 0.5) ? 1.96 : 1.96;
    result.ci_lower = -z * stdR;
    result.ci_upper = z * stdR;

    result.prob_outside_horizon = probErrorExceeds(std::abs(result.ci_upper), residuals);

    return result;
}

double ForecastError::probErrorExceeds(double horizon, const std::vector<double>& residuals) {
    if (residuals.empty()) return 0;

    const size_t n = residuals.size();
    const double mean = std::accumulate(residuals.begin(), residuals.end(), 0.0) / n;
    double varSum = 0;
    for (double r : residuals) {
        const double d = r - mean;
        varSum += d * d;
    }
    const double std = std::sqrt((n > 1) ? varSum / (n - 1) : 0);
    if (std < 1e-14) return (std::abs(mean) > horizon) ? 1.0 : 0.0;

    const double z = horizon / std;
    return 2.0 * (1.0 - 0.5 * (1.0 + std::erf(z / 1.414213562373095)));
}

} // namespace SargisLab
