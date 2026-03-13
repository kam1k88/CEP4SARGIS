#pragma once

#include "core/types.hpp"
#include <QDialog>

class ForecastErrorWindow : public QDialog {
    Q_OBJECT
public:
    explicit ForecastErrorWindow(QWidget* parent = nullptr);

    void setResult(const curvefit::ForecastErrorResult& result);
    void setForecastPlot(const std::vector<double>& x, const std::vector<double>& y,
                        const std::vector<double>& ciLower,
                        const std::vector<double>& ciUpper);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
