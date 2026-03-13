#pragma once

#include "core/types.hpp"
#include <QString>
#include <optional>

namespace curvefit {

namespace io {

/// Load dataset from JSON file: { "x": [...], "y": [...] }
std::optional<Dataset> loadJson(const QString& path);

/// Save dataset to JSON
bool saveJson(const Dataset& data, const QString& path);

} // namespace io

} // namespace curvefit
