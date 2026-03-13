#include "core/io/json_io.hpp"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace SargisLab {
namespace io {

std::optional<Dataset> loadJson(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return std::nullopt;
    }

    QByteArray content = file.readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(content, &err);
    if (doc.isNull()) return std::nullopt;

    QJsonObject obj = doc.object();
    QJsonArray xArr = obj["x"].toArray();
    QJsonArray yArr = obj["y"].toArray();

    if (xArr.size() != yArr.size() || xArr.isEmpty()) {
        return std::nullopt;
    }

    Dataset data;
    for (int i = 0; i < xArr.size(); ++i) {
        double x = xArr[i].toDouble();
        double y = yArr[i].toDouble();
        data.add(x, y);
    }
    return data;
}

bool saveJson(const Dataset& data, const QString& path) {
    QJsonArray xArr;
    QJsonArray yArr;
    for (size_t i = 0; i < data.size(); ++i) {
        xArr.append(data.x[i]);
        yArr.append(data.y[i]);
    }

    QJsonObject obj;
    obj["x"] = xArr;
    obj["y"] = yArr;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    return true;
}

} // namespace io
} // namespace SargisLab
