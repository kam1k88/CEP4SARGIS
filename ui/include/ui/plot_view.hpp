#pragma once

#include "core/types.hpp"
#include <QWidget>
#include <QString>
#include <vector>

class PlotView : public QWidget {
    Q_OBJECT
public:
    explicit PlotView(QWidget* parent = nullptr);
    ~PlotView() override;

    void setData(const SargisLab::Dataset& data);
    void setFitCurve(const std::vector<double>& x, const std::vector<double>& y);
    void setResiduals(const std::vector<double>& x, const std::vector<double>& residuals);
    void clearFit();
    void clearResiduals();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
