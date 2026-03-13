#pragma once

#include "core/types.hpp"
#include <QString>
#include <optional>

namespace SargisLab {

namespace io {

/// Load dataset from CSV file (columns: x, y or first two numeric columns)
std::optional<Dataset> loadCsv(const QString& path, QChar delimiter = ',');

/// Save dataset to CSV
bool saveCsv(const Dataset& data, const QString& path, QChar delimiter = ',');

} // namespace io

} // namespace SargisLab
