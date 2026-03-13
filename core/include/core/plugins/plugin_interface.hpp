#pragma once

#include "core/regression/model_interface.hpp"

#ifdef _WIN32
#  define SargisLab_PLUGIN_EXPORT __declspec(dllexport)
#else
#  define SargisLab_PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

namespace SargisLab {

/// Function pointer types for plugin ABI (plugins export create_model, destroy_model with C linkage)
using CreateModelFunc = IRegressionModel* (*)();
using DestroyModelFunc = void (*)(IRegressionModel*);

}

/// Plugin ABI: plugins must export these C-linkage symbols:
///   extern "C" SargisLab::IRegressionModel* create_model();
///   extern "C" void destroy_model(SargisLab::IRegressionModel* model);
