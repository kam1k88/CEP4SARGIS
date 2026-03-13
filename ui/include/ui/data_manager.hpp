#pragma once

#include "core/types.hpp"
#include <QWidget>
#include <QTableWidget>

class DataManager : public QWidget {
    Q_OBJECT
public:
    explicit DataManager(QWidget* parent = nullptr);

    SargisLab::Dataset dataset() const;
    void setDataset(const SargisLab::Dataset& data);
    void clear();

signals:
    void dataChanged();

private:
    void updateFromTable();
    QTableWidget* table_;
    mutable SargisLab::Dataset dataset_;
};
