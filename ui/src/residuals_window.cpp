#include "ui/residuals_window.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>

#if defined(SargisLab_QCUSTOMPLOT) && SargisLab_QCUSTOMPLOT
#include "qcustomplot.h"
#else
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#endif

struct ResidualsWindow::Impl {
    QLabel* statsLabel = nullptr;
#if defined(SargisLab_QCUSTOMPLOT) && SargisLab_QCUSTOMPLOT
    QCustomPlot* plot = nullptr;
#else
    QtCharts::QChartView* chartView = nullptr;
#endif
};

ResidualsWindow::ResidualsWindow(QWidget* parent) : QDialog(parent), impl_(std::make_unique<Impl>()) {
    setWindowTitle("Residual Analysis");
    resize(600, 500);

    auto* layout = new QVBoxLayout(this);

    impl_->statsLabel = new QLabel(this);
    impl_->statsLabel->setText("Mean: -\nVariance: -\nSkewness: -\nKurtosis: -");
    layout->addWidget(impl_->statsLabel);

#if defined(SargisLab_QCUSTOMPLOT) && SargisLab_QCUSTOMPLOT
    impl_->plot = new QCustomPlot(this);
    layout->addWidget(impl_->plot);
#else
    impl_->chartView = new QtCharts::QChartView(this);
    impl_->chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(impl_->chartView);
#endif
}

void ResidualsWindow::setData(const std::vector<double>& x, const std::vector<double>& residuals) {
#if defined(SargisLab_QCUSTOMPLOT) && SargisLab_QCUSTOMPLOT
    if (!impl_->plot) return;
    impl_->plot->clearGraphs();
    impl_->plot->addGraph();
    QVector<double> xv, yv;
    for (size_t i = 0; i < x.size(); ++i) {
        xv.append(x[i]);
        yv.append(residuals[i]);
    }
    impl_->plot->graph(0)->setData(xv, yv);
    impl_->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    impl_->plot->xAxis->setLabel("X");
    impl_->plot->yAxis->setLabel("Residual");
    impl_->plot->rescaleAxes();
    impl_->plot->replot();
#else
    if (!impl_->chartView) return;
    auto* chart = impl_->chartView->chart();
    chart->removeAllSeries();
    auto* series = new QtCharts::QScatterSeries();
    series->setName("Residuals");
    for (size_t i = 0; i < x.size(); ++i) {
        series->append(x[i], residuals[i]);
    }
    chart->addSeries(series);
    chart->createDefaultAxes();
#endif
}

void ResidualsWindow::setStats(double mean, double variance, double skewness, double kurtosis) {
    impl_->statsLabel->setText(
        QString("Mean: %1\nVariance: %2\nSkewness: %3\nKurtosis: %4")
            .arg(mean).arg(variance).arg(skewness).arg(kurtosis));
}
