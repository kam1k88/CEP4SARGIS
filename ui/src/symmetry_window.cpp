#include "ui/symmetry_window.hpp"
#include <QVBoxLayout>
#include <QLabel>

#if defined(SargisLab_QCUSTOMPLOT) && SargisLab_QCUSTOMPLOT
#include "qcustomplot.h"
#else
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#endif

struct SymmetryWindow::Impl {
    QLabel* skewnessLabel = nullptr;
#if defined(SargisLab_QCUSTOMPLOT) && SargisLab_QCUSTOMPLOT
    QCustomPlot* plot = nullptr;
#else
    QtCharts::QChartView* chartView = nullptr;
#endif
};

SymmetryWindow::SymmetryWindow(QWidget* parent) : QDialog(parent), impl_(std::make_unique<Impl>()) {
    setWindowTitle("Symmetry Analysis");
    resize(600, 450);

    auto* layout = new QVBoxLayout(this);

    impl_->skewnessLabel = new QLabel(this);
    layout->addWidget(impl_->skewnessLabel);

#if defined(SargisLab_QCUSTOMPLOT) && SargisLab_QCUSTOMPLOT
    impl_->plot = new QCustomPlot(this);
    layout->addWidget(impl_->plot);
#else
    impl_->chartView = new QtCharts::QChartView(this);
    impl_->chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(impl_->chartView);
#endif
}

void SymmetryWindow::setHistogram(const std::vector<std::pair<double, int>>& histogram,
                                  const std::vector<std::pair<double, int>>&) {
#if defined(SargisLab_QCUSTOMPLOT) && SargisLab_QCUSTOMPLOT
    if (!impl_->plot) return;
    impl_->plot->clearGraphs();
    impl_->plot->addGraph();
    QVector<double> xv, yv;
    for (const auto& p : histogram) {
        xv.append(p.first);
        yv.append(p.second);
    }
    impl_->plot->graph(0)->setData(xv, yv);
    impl_->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    impl_->plot->rescaleAxes();
    impl_->plot->replot();
#else
    if (!impl_->chartView) return;
    auto* chart = impl_->chartView->chart();
    chart->removeAllSeries();

    auto* barSet = new QtCharts::QBarSet("Count");
    QStringList categories;
    for (const auto& p : histogram) {
        *barSet << p.second;
        categories << QString::number(p.first, 'g', 3);
    }
    auto* series = new QtCharts::QBarSeries();
    series->append(barSet);
    chart->addSeries(series);

    auto* axisX = new QtCharts::QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    chart->createDefaultAxes();
#endif
}

void SymmetryWindow::setSkewness(double skewness) {
    impl_->skewnessLabel->setText(QString("Skewness: %1 (0 = symmetric)").arg(skewness));
}
