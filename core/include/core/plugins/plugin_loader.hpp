#pragma once

#include "core/regression/model_interface.hpp"
#include <QString>
#include <vector>
#include <memory>

namespace SargisLab {

/// Loads regression model plugins from DLL/dylib/so
class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();

    /// Search plugins directory and load all valid plugins
    void loadFromDirectory(const QString& directory);

    /// Load single plugin by path
    bool loadPlugin(const QString& path);

    /// Get all loaded models (loader owns plugin-backed models)
    std::vector<IRegressionModel*> loadedModels() const;

    /// Clear all loaded plugins
    void clear();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace SargisLab
