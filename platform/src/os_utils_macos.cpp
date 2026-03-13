#include "platform/os_utils.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <vector>
#include <cstdlib>

namespace curvefit {
namespace platform {

QString getExecutableDirectory() {
#ifdef __APPLE__
    std::vector<char> buf(1024);
    uint32_t size = static_cast<uint32_t>(buf.size());
    if (_NSGetExecutablePath(buf.data(), &size) == -1) {
        buf.resize(size);
        _NSGetExecutablePath(buf.data(), &size);
    }
    QString path = QDir::cleanPath(QString::fromUtf8(buf.data()));
    return QFileInfo(path).absolutePath();
#else
    return QCoreApplication::applicationDirPath();
#endif
}

QString getPluginsDirectory() {
    QString exeDir = getExecutableDirectory();
    QDir dir(exeDir);

#ifdef __APPLE__
    if (dir.cd("../PlugIns")) {
        return dir.absolutePath();
    }
    if (dir.cd("../Resources/plugins")) {
        return dir.absolutePath();
    }
#endif

    if (dir.cd("plugins")) {
        return dir.absolutePath();
    }
    dir.mkpath("plugins");
    return dir.absoluteFilePath("plugins");
}

QString getAppDataDirectory() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (path.isEmpty()) {
        path = getExecutableDirectory() + "/data";
    }
    QDir().mkpath(path);
    return path;
}

} // namespace platform
} // namespace curvefit
