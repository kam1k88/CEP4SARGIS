#include "core/io/excel_io.hpp"

#if defined(CURVEFIT_EXCEL_AVAILABLE) && CURVEFIT_EXCEL_AVAILABLE

#include <OpenXLSX.hpp>

namespace curvefit {
namespace io {

std::optional<Dataset> loadExcel(const QString& path) {
    try {
        OpenXLSX::XLDocument doc;
        doc.open(path.toStdString());
        auto wb = doc.workbook();
        auto ws = wb.worksheet(wb.worksheetNames().front());

        Dataset data;
        uint32_t row = 1;
        while (true) {
            auto a = ws.cell(row, 1).value();
            auto b = ws.cell(row, 2).value();
            if (!a.type() || !b.type()) break;

            double x = 0, y = 0;
            if (a.type() == OpenXLSX::XLValueType::Integer) x = static_cast<double>(a.get<int>());
            else if (a.type() == OpenXLSX::XLValueType::Float) x = a.get<double>();
            else break;

            if (b.type() == OpenXLSX::XLValueType::Integer) y = static_cast<double>(b.get<int>());
            else if (b.type() == OpenXLSX::XLValueType::Float) y = b.get<double>();
            else break;

            data.add(x, y);
            ++row;
        }
        doc.close();
        return data.empty() ? std::nullopt : std::optional<Dataset>(std::move(data));
    } catch (...) {
        return std::nullopt;
    }
}

bool saveExcel(const Dataset& data, const QString& path) {
    try {
        OpenXLSX::XLDocument doc;
        doc.create(path.toStdString());
        auto ws = doc.workbook().worksheet("Sheet1");

        for (size_t i = 0; i < data.size(); ++i) {
            ws.cell(static_cast<uint32_t>(i + 1), 1).value() = data.x[i];
            ws.cell(static_cast<uint32_t>(i + 1), 2).value() = data.y[i];
        }
        doc.save();
        doc.close();
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace io
} // namespace curvefit

#endif
