#include "core/plugins/plugin_loader.hpp"
#include "core/plugins/plugin_interface.hpp"
#include <QDir>
#include <algorithm>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif

namespace curvefit {

struct PluginHandle {
#ifdef _WIN32
    HMODULE handle = nullptr;
#else
    void* handle = nullptr;
#endif
    IRegressionModel* model = nullptr;
    CreateModelFunc createFunc = nullptr;
    DestroyModelFunc destroyFunc = nullptr;

    ~PluginHandle() {
        if (model && destroyFunc) {
            destroyFunc(model);
        }
#ifdef _WIN32
        if (handle) FreeLibrary(handle);
#else
        if (handle) dlclose(handle);
#endif
    }
};

struct PluginLoader::Impl {
    std::vector<std::unique_ptr<PluginHandle>> plugins;
};

PluginLoader::PluginLoader() : impl_(std::make_unique<Impl>()) {}

PluginLoader::~PluginLoader() = default;

namespace {

void* loadLibrary(const QString& path) {
#ifdef _WIN32
    return reinterpret_cast<void*>(LoadLibraryW(path.toStdWString().c_str()));
#else
    return dlopen(path.toUtf8().constData(), RTLD_LAZY | RTLD_LOCAL);
#endif
}

void* getSymbol(void* handle, const char* name) {
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle), name));
#else
    return dlsym(handle, name);
#endif
}

QString pluginExtension() {
#ifdef _WIN32
    return "dll";
#elif __APPLE__
    return "dylib";
#else
    return "so";
#endif
}

} // anonymous

void PluginLoader::loadFromDirectory(const QString& directory) {
    QDir dir(directory);
    if (!dir.exists()) return;

    const QString ext = pluginExtension();
    const QStringList filters = {"*." + ext};

    for (const QFileInfo& info : dir.entryInfoList(filters, QDir::Files)) {
        loadPlugin(info.absoluteFilePath());
    }
}

bool PluginLoader::loadPlugin(const QString& path) {
    void* handle = loadLibrary(path);
    if (!handle) return false;

    auto createFunc = reinterpret_cast<CreateModelFunc>(getSymbol(handle, "create_model"));
    auto destroyFunc = reinterpret_cast<DestroyModelFunc>(getSymbol(handle, "destroy_model"));

    if (!createFunc || !destroyFunc) {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(handle));
#else
        dlclose(handle);
#endif
        return false;
    }

    IRegressionModel* model = createFunc();
    if (!model) {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(handle));
#else
        dlclose(handle);
#endif
        return false;
    }

    auto plugin = std::make_unique<PluginHandle>();
#ifdef _WIN32
    plugin->handle = static_cast<HMODULE>(handle);
#else
    plugin->handle = handle;
#endif
    plugin->model = model;
    plugin->createFunc = createFunc;
    plugin->destroyFunc = destroyFunc;

    impl_->plugins.push_back(std::move(plugin));
    return true;
}

std::vector<IRegressionModel*> PluginLoader::loadedModels() const {
    std::vector<IRegressionModel*> result;
    for (const auto& p : impl_->plugins) {
        if (p->model) {
            result.push_back(p->model);
        }
    }
    return result;
}

void PluginLoader::clear() {
    impl_->plugins.clear();
}

} // namespace curvefit
