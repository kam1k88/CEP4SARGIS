#include "core/analysis/residuals_analyzer.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>

namespace curvefit {

ResidualsResult ResidualsAnalyzer::analyze(const Dataset& data, IRegressionModel* model,
                                           const std::vector<double>& params) {
    ResidualsResult result;
    if (!model || params.size() != model->paramCount()) {
        return result;
    }

    const double* p = params.data();
    result.residuals.reserve(data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        const double pred = model->evaluate(data.x[i], p);
        result.residuals.push_back(data.y[i] - pred);
    }

    const size_t n = result.residuals.size();
    if (n == 0) return result;

    const double mean = std::accumulate(result.residuals.begin(), result.residuals.end(), 0.0) / n;
    result.mean = mean;

    double varSum = 0;
    for (double r : result.residuals) {
        const double d = r - mean;
        varSum += d * d;
    }
    result.variance = (n > 1) ? varSum / (n - 1) : 0;

    const double std = std::sqrt(result.variance);
    if (std < 1e-14) {
        result.skewness = 0;
        result.kurtosis = 0;
        return result;
    }

    double m3 = 0, m4 = 0;
    for (double r : result.residuals) {
        const double z = (r - mean) / std;
        const double z2 = z * z;
        m3 += z2 * z;
        m4 += z2 * z2;
    }
    m3 /= n;
    m4 /= n;
    result.skewness = m3;
    result.kurtosis = m4 - 3.0;  // excess kurtosis

    result.qq_plot = computeQQPlot(result.residuals);
    return result;
}

void ResidualsAnalyzer::residualsVsXRegression(const std::vector<double>& x,
                                               const std::vector<double>& residuals,
                                               double& slope, double& intercept, double& r2) {
    const size_t n = std::min(x.size(), residuals.size());
    if (n < 2) {
        slope = 0;
        intercept = 0;
        r2 = 0;
        return;
    }

    double sx = 0, sy = 0, sxx = 0, sxy = 0, syy = 0;
    for (size_t i = 0; i < n; ++i) {
        sx += x[i];
        sy += residuals[i];
        sxx += x[i] * x[i];
        sxy += x[i] * residuals[i];
        syy += residuals[i] * residuals[i];
    }

    const double det = n * sxx - sx * sx;
    if (std::abs(det) < 1e-14) {
        slope = 0;
        intercept = sy / n;
        r2 = 0;
        return;
    }

    slope = (n * sxy - sx * sy) / det;
    intercept = (sy - slope * sx) / n;

    const double meanR = sy / n;
    double ssTot = 0, ssRes = 0;
    for (size_t i = 0; i < n; ++i) {
        const double pred = slope * x[i] + intercept;
        ssRes += (residuals[i] - pred) * (residuals[i] - pred);
        ssTot += (residuals[i] - meanR) * (residuals[i] - meanR);
    }
    r2 = (ssTot > 1e-14) ? (1.0 - ssRes / ssTot) : 1.0;
}

std::vector<std::pair<double, double>> ResidualsAnalyzer::computeQQPlot(
    const std::vector<double>& residuals) {
    std::vector<double> sorted = residuals;
    std::sort(sorted.begin(), sorted.end());
    const size_t n = sorted.size();
    std::vector<std::pair<double, double>> qq;

    auto normalQuantile = [](double p) -> double {
        if (p <= 0 || p >= 1) return 0;
        static const double a[] = {-3.969683028665376e1, 2.209460984245205e2,
            -2.759285104469687e2, 1.383577518672690e2, -3.066479806614716e1,
            2.506628277459239e0};
        static const double b[] = {-5.447609879822406e1, 1.615858368580409e2,
            -1.556989798598866e2, 6.680131188771972e1, -1.328068155288572e1};
        double q = p - 0.5;
        if (std::abs(q) <= 0.425) {
            double r = 0.180625 - q * q;
            return q * (((((a[0] * r + a[1]) * r + a[2]) * r + a[3]) * r + a[4]) * r + a[5]) /
                (((((b[0] * r + b[1]) * r + b[2]) * r + b[3]) * r + b[4]) * r + 1.0);
        }
        double r = (q < 0) ? p : 1 - p;
        r = std::sqrt(-std::log(r));
        static const double c[] = {1.0, 5.99491642647327e-2, 1.94727943745523e-2,
            2.72728826361669e-2, 2.75995740307101e-2, 2.23688700072777e-2};
        static const double d[] = {1.0, 1.47450215155001, 4.39959273483989e-1,
            9.03218426117622e-2, -1.35036910481605e-1, -4.23954326102277e-1};
        double num = c[5];
        for (int i = 4; i >= 0; --i) num = num * r + c[i];
        double den = d[5];
        for (int i = 4; i >= 0; --i) den = den * r + d[i];
        double x = num / den;
        return (q < 0) ? -x : x;
    };

    for (size_t i = 0; i < n; ++i) {
        const double p = (i + 0.5) / n;
        const double z = normalQuantile(p);
        qq.emplace_back(z, sorted[i]);
    }
    return qq;
}

} // namespace curvefit
