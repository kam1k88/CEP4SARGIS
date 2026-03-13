#pragma once

#include "core/types.hpp"
#include "core/regression/model_interface.hpp"
#include <vector>

namespace SargisLab {

class ForecastError {
public:
    /// Compute RMSE, MAE, MAPE and prediction CI
    ForecastErrorResult evaluate(const Dataset& data, IRegressionModel* model,
                                 const std::vector<double>& params,
                                 double confidenceLevel = 0.95);

    /// Estimate probability that error exceeds horizon (assuming normal residuals)
    double probErrorExceeds(double horizon, const std::vector<double>& residuals);
};

} // namespace SargisLab
