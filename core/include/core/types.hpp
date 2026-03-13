#pragma once

#include <vector>
#include <string>
#include <cstddef>

namespace curvefit {

/// Single data point (x, y)
struct DataPoint {
    double x{0};
    double y{0};

    DataPoint() = default;
    DataPoint(double x_, double y_) : x(x_), y(y_) {}
};

/// Dataset for regression: pairs (x, y)
struct Dataset {
    std::vector<double> x;
    std::vector<double> y;

    Dataset() = default;

    explicit Dataset(std::size_t n) {
        x.reserve(n);
        y.reserve(n);
    }

    void add(double x_val, double y_val) {
        x.push_back(x_val);
        y.push_back(y_val);
    }

    std::size_t size() const {
        return x.size();
    }

    bool empty() const {
        return x.empty();
    }

    void clear() {
        x.clear();
        y.clear();
    }

    DataPoint at(std::size_t i) const {
        return DataPoint{x[i], y[i]};
    }
};

/// Result of regression fit
struct FitResult {
    std::vector<double> parameters;
    double rmse{0};
    double r_squared{0};
    int iterations{0};
    bool success{false};
    std::string message;
};

/// Residual analysis result
struct ResidualsResult {
    std::vector<double> residuals;
    double mean{0};
    double variance{0};
    double skewness{0};
    double kurtosis{0};
    std::vector<std::pair<double, double>> qq_plot;
};

/// Forecast error metrics
struct ForecastErrorResult {
    double rmse{0};
    double mae{0};
    double mape{0};
    double ci_lower{0};
    double ci_upper{0};
    double prob_outside_horizon{0};
};

/// Monte Carlo result
struct MonteCarloResult {
    std::vector<double> outcomes;
    double mean{0};
    double variance{0};
    double std_dev{0};
    double percentile_2_5{0};
    double percentile_50{0};
    double percentile_97_5{0};
};

} // namespace curvefit
