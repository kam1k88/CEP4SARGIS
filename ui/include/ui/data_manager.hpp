#pragma once

#include "core/types.hpp"
#include <QWidget>
#include <QTableWidget>

class DataManager : public QWidget {
    Q_OBJECT
public:
    explicit DataManager(QWidget* parent = nullptr);

    curvefit::Dataset dataset() const;
    void setDataset(const curvefit::Dataset& data);
    void clear();

signals:
    void dataChanged();

private:
    void updateFromTable();
    QTableWidget* table_;
    mutable curvefit::Dataset dataset_;
};
