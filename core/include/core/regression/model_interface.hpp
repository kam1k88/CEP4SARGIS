#pragma once

#include <string>
#include <vector>
#include <cstddef>

// Forward declaration - avoid including Ceres in header for plugin ABI
namespace ceres {
    class CostFunction;
}

namespace SargisLab {

/// Unified interface for regression models (built-in and plugins)
struct IRegressionModel {
    virtual ~IRegressionModel() = default;

    virtual std::string name() const = 0;
    virtual size_t paramCount() const = 0;
    virtual std::vector<std::string> paramNames() const = 0;

    /// Evaluate model at x with given parameters
    virtual double evaluate(double x, const double* params) const = 0;

    /// Compute partial derivatives wrt parameters at (x, params), write to grad[0..paramCount-1]
    virtual void derivatives(double x, const double* params, double* grad) const = 0;

    /// Create Ceres cost function for point (x, y) - used by regression engine
    virtual ceres::CostFunction* createCostFunction(double x, double y) const = 0;
};

} // namespace SargisLab
