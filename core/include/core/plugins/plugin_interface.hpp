#pragma once

#include "core/regression/model_interface.hpp"

#ifdef _WIN32
#  define CURVEFIT_PLUGIN_EXPORT __declspec(dllexport)
#else
#  define CURVEFIT_PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

namespace curvefit {

/// Function pointer types for plugin ABI (plugins export create_model, destroy_model with C linkage)
using CreateModelFunc = IRegressionModel* (*)();
using DestroyModelFunc = void (*)(IRegressionModel*);

}

/// Plugin ABI: plugins must export these C-linkage symbols:
///   extern "C" curvefit::IRegressionModel* create_model();
///   extern "C" void destroy_model(curvefit::IRegressionModel* model);
