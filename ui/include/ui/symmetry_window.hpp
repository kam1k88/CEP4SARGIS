#pragma once

#include <QDialog>
#include <vector>
#include <utility>

class SymmetryWindow : public QDialog {
    Q_OBJECT
public:
    explicit SymmetryWindow(QWidget* parent = nullptr);

    void setHistogram(const std::vector<std::pair<double, int>>& histogram,
                     const std::vector<std::pair<double, int>>& mirrored);
    void setSkewness(double skewness);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
