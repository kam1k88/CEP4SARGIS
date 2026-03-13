#pragma once

#include "core/types.hpp"
#include "model_interface.hpp"
#include <memory>
#include <vector>

namespace SargisLab {

class RegressionEngine {
public:
    RegressionEngine();
    ~RegressionEngine();

    /// Register built-in models
    void registerBuiltinModels();

    /// Add model (engine does not take ownership; used for plugins)
    void addModel(IRegressionModel* model);

    /// Get all available models
    std::vector<IRegressionModel*> models() const;

    /// Fit dataset with given model and initial parameters
    FitResult fit(const Dataset& data, IRegressionModel* model,
                  const std::vector<double>& initialParams);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace SargisLab
