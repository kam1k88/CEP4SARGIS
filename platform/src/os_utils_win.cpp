#include "platform/os_utils.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <windows.h>
#include <vector>

namespace SargisLab {
namespace platform {

QString getExecutableDirectory() {
    std::vector<wchar_t> buf(MAX_PATH);
    DWORD len = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
    while (len == buf.size()) {
        buf.resize(buf.size() * 2);
        len = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
    }
    if (len == 0) {
        return QCoreApplication::applicationDirPath();
    }
    QString path = QString::fromWCharArray(buf.data(), static_cast<int>(len));
    return QDir(path).absolutePath();
}

QString getPluginsDirectory() {
    QString exeDir = getExecutableDirectory();
    QDir dir(exeDir);
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
} // namespace SargisLab
