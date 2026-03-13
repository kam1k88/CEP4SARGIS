#pragma once

#include <QDialog>
#include <vector>

class ResidualsWindow : public QDialog {
    Q_OBJECT
public:
    explicit ResidualsWindow(QWidget* parent = nullptr);

    void setData(const std::vector<double>& x, const std::vector<double>& residuals);
    void setStats(double mean, double variance, double skewness, double kurtosis);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
