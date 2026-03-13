#include "core/io/csv_io.hpp"
#include <QFile>
#include <QTextStream>
#include <QStringList>

namespace curvefit {
namespace io {

std::optional<Dataset> loadCsv(const QString& path, QChar delimiter) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return std::nullopt;
    }

    Dataset data;
    QTextStream in(&file);

    auto toDouble = [](const QString& s) -> std::optional<double> {
        bool ok;
        double v = s.trimmed().toDouble(&ok);
        return ok ? std::optional<double>(v) : std::nullopt;
    };

    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue;

        const QStringList parts = line.split(delimiter);
        if (parts.size() < 2) continue;

        auto x = toDouble(parts[0]);
        auto y = toDouble(parts[1]);
        if (x && y) {
            data.add(*x, *y);
        }
    }

    return data.empty() ? std::nullopt : std::optional<Dataset>(std::move(data));
}

bool saveCsv(const Dataset& data, const QString& path, QChar delimiter) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    for (size_t i = 0; i < data.size(); ++i) {
        out << data.x[i] << delimiter << data.y[i] << "\n";
    }
    return true;
}

} // namespace io
} // namespace curvefit
