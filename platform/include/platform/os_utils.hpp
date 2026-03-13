#pragma once

#include <QString>

namespace SargisLab {
namespace platform {

/// Get directory containing the executable
QString getExecutableDirectory();

/// Get plugins directory (typically next to executable or in app bundle)
QString getPluginsDirectory();

/// Get application data directory for user-specific storage
QString getAppDataDirectory();

} // namespace platform
} // namespace SargisLab
