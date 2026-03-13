// Example plugin: exports Gaussian CDF model via plugin ABI.
// Links to curvefit_core to reuse GaussianCDFModel implementation.

#include "core/regression/gaussian_cdf_model.hpp"
#include "core/plugins/plugin_interface.hpp"

extern "C" {

CURVEFIT_PLUGIN_EXPORT curvefit::IRegressionModel* create_model() {
    return new curvefit::GaussianCDFModel();
}

CURVEFIT_PLUGIN_EXPORT void destroy_model(curvefit::IRegressionModel* model) {
    delete model;
}

}
