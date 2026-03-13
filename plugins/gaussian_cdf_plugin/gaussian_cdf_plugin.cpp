// Example plugin: exports Gaussian CDF model via plugin ABI.
// Links to SargisLab_core to reuse GaussianCDFModel implementation.

#include "core/regression/gaussian_cdf_model.hpp"
#include "core/plugins/plugin_interface.hpp"

extern "C" {

SargisLab_PLUGIN_EXPORT SargisLab::IRegressionModel* create_model() {
    return new SargisLab::GaussianCDFModel();
}

SargisLab_PLUGIN_EXPORT void destroy_model(SargisLab::IRegressionModel* model) {
    delete model;
}

}
