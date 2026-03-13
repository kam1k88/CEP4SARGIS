#include "ui/data_manager.hpp"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>

DataManager::DataManager(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    table_ = new QTableWidget(this);
    table_->setColumnCount(2);
    table_->setHorizontalHeaderLabels({"X", "Y"});
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->setAlternatingRowColors(true);
    layout->addWidget(table_);

    connect(table_, &QTableWidget::cellChanged, this, [this]() { updateFromTable(); emit dataChanged(); });
}

SargisLab::Dataset DataManager::dataset() const {
    updateFromTable();
    return dataset_;
}

void DataManager::setDataset(const SargisLab::Dataset& data) {
    dataset_ = data;
    table_->blockSignals(true);
    table_->setRowCount(static_cast<int>(data.size()));
    for (size_t i = 0; i < data.size(); ++i) {
        table_->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::number(data.x[i])));
        table_->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::number(data.y[i])));
    }
    table_->blockSignals(false);
}

void DataManager::clear() {
    dataset_.clear();
    table_->setRowCount(0);
    emit dataChanged();
}

void DataManager::updateFromTable() const {
    dataset_.clear();
    for (int row = 0; row < table_->rowCount(); ++row) {
        auto* xItem = table_->item(row, 0);
        auto* yItem = table_->item(row, 1);
        if (!xItem || !yItem) continue;
        bool okX, okY;
        double x = xItem->text().toDouble(&okX);
        double y = yItem->text().toDouble(&okY);
        if (okX && okY) {
            dataset_.add(x, y);
        }
    }
}
