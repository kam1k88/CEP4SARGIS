#include "core/montecarlo/monte_carlo_engine.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <thread>
#include <future>

namespace SargisLab {

MonteCarloEngine::MonteCarloEngine() : seed_(0), rng_(seed_) {}

void MonteCarloEngine::setSeed(unsigned seed) {
    seed_ = seed;
    rng_.seed(seed);
}

std::vector<double> MonteCarloEngine::sampleParams(
    const std::vector<double>& baseParams,
    const std::vector<ParamDistribution>& dists) {
    std::vector<double> params = baseParams;
    for (size_t i = 0; i < std::min(params.size(), dists.size()); ++i) {
        const auto& d = dists[i];
        double val;
        switch (d.type) {
            case DistributionType::Normal: {
                std::normal_distribution<> nd(d.param1, d.param2);
                val = nd(rng_);
                break;
            }
            case DistributionType::Uniform: {
                std::uniform_real_distribution<> ud(d.param1, d.param2);
                val = ud(rng_);
                break;
            }
            case DistributionType::LogNormal: {
                std::lognormal_distribution<> ln(d.param1, d.param2);
                val = ln(rng_);
                break;
            }
        }
        params[i] = val;
    }
    return params;
}

MonteCarloResult MonteCarloEngine::run(const std::vector<double>& xPoints,
                                       IRegressionModel* model,
                                       const std::vector<double>& baseParams,
                                       const std::vector<ParamDistribution>& paramDists,
                                       int numSimulations) {
    auto evaluator = [&](const std::vector<double>& params) -> std::vector<double> {
        std::vector<double> out;
        out.reserve(xPoints.size());
        const double* p = params.data();
        for (double x : xPoints) {
            out.push_back(model->evaluate(x, p));
        }
        return out;
    };

    return runGeneric(evaluator, baseParams, paramDists, numSimulations);
}

MonteCarloResult MonteCarloEngine::runGeneric(
    std::function<std::vector<double>(const std::vector<double>&)> evaluator,
    const std::vector<double>& baseParams,
    const std::vector<ParamDistribution>& paramDists,
    int numSimulations) {
    MonteCarloResult result;
    if (numSimulations < 1) return result;

    std::vector<std::vector<double>> allOutputs;
    allOutputs.reserve(static_cast<size_t>(numSimulations));

    for (int s = 0; s < numSimulations; ++s) {
        auto params = sampleParams(baseParams, paramDists);
        allOutputs.push_back(evaluator(params));
    }

    const size_t nOut = allOutputs.empty() ? 0 : allOutputs[0].size();
    if (nOut == 0) return result;

    for (size_t j = 0; j < nOut; ++j) {
        std::vector<double> vals;
        vals.reserve(static_cast<size_t>(numSimulations));
        for (const auto& out : allOutputs) {
            if (j < out.size()) vals.push_back(out[j]);
        }

        const size_t n = vals.size();
        const double mean = std::accumulate(vals.begin(), vals.end(), 0.0) / n;
        double varSum = 0;
        for (double v : vals) {
            const double d = v - mean;
            varSum += d * d;
        }
        const double variance = (n > 1) ? varSum / (n - 1) : 0;
        const double stdDev = std::sqrt(variance);

        std::sort(vals.begin(), vals.end());
        const size_t i25 = static_cast<size_t>(n * 0.025);
        const size_t i50 = static_cast<size_t>(n * 0.5);
        const size_t i975 = static_cast<size_t>(n * 0.975);

        result.outcomes.insert(result.outcomes.end(), vals.begin(), vals.end());
        result.mean = mean;
        result.variance = variance;
        result.std_dev = stdDev;
        result.percentile_2_5 = vals[std::min(i25, n - 1)];
        result.percentile_50 = vals[std::min(i50, n - 1)];
        result.percentile_97_5 = vals[std::min(i975, n - 1)];

        break;
    }

    if (nOut > 0 && !allOutputs.empty()) {
        std::vector<double> firstCol;
        for (const auto& out : allOutputs) {
            if (!out.empty()) firstCol.push_back(out[0]);
        }
        const size_t n = firstCol.size();
        result.mean = std::accumulate(firstCol.begin(), firstCol.end(), 0.0) / n;
        double varSum = 0;
        for (double v : firstCol) {
            const double d = v - result.mean;
            varSum += d * d;
        }
        result.variance = (n > 1) ? varSum / (n - 1) : 0;
        result.std_dev = std::sqrt(result.variance);
        std::sort(firstCol.begin(), firstCol.end());
        result.percentile_2_5 = firstCol[static_cast<size_t>(n * 0.025)];
        result.percentile_50 = firstCol[static_cast<size_t>(n * 0.5)];
        result.percentile_97_5 = firstCol[static_cast<size_t>(n * 0.975)];
        result.outcomes = std::move(firstCol);
    }

    return result;
}

} // namespace SargisLab
