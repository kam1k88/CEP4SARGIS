#pragma once

#include "core/types.hpp"
#include <QString>
#include <optional>

namespace curvefit {

namespace io {

#if defined(CURVEFIT_EXCEL_AVAILABLE) && CURVEFIT_EXCEL_AVAILABLE

/// Load dataset from Excel .xlsx (first sheet, columns A and B)
std::optional<Dataset> loadExcel(const QString& path);

/// Save dataset to Excel .xlsx
bool saveExcel(const Dataset& data, const QString& path);

#else

inline std::optional<Dataset> loadExcel(const QString&) { return std::nullopt; }
inline bool saveExcel(const Dataset&, const QString&) { return false; }

#endif

} // namespace io

} // namespace curvefit
