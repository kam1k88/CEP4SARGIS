#pragma once

#include "core/types.hpp"
#include "core/regression/model_interface.hpp"
#include <functional>
#include <random>
#include <vector>

namespace curvefit {

enum class DistributionType {
    Normal,
    Uniform,
    LogNormal
};

struct ParamDistribution {
    DistributionType type;
    double param1;  // mean / low / log-mean
    double param2;  // std / high / log-std
};

class MonteCarloEngine {
public:
    MonteCarloEngine();

    void setSeed(unsigned seed);
    unsigned seed() const { return seed_; }

    /// Run simulations: each run uses perturbed params, model evaluates at given x
    MonteCarloResult run(const std::vector<double>& xPoints,
                         IRegressionModel* model,
                         const std::vector<double>& baseParams,
                         const std::vector<ParamDistribution>& paramDists,
                         int numSimulations = 1000);

    /// Run with custom evaluator: (params) -> vector of outputs
    MonteCarloResult runGeneric(
        std::function<std::vector<double>(const std::vector<double>&)> evaluator,
        const std::vector<double>& baseParams,
        const std::vector<ParamDistribution>& paramDists,
        int numSimulations = 1000);

private:
    std::vector<double> sampleParams(const std::vector<double>& baseParams,
                                     const std::vector<ParamDistribution>& dists);

    unsigned seed_;
    std::mt19937 rng_;
};

} // namespace curvefit
