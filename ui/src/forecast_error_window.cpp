#include "ui/forecast_error_window.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>

#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
#include "qcustomplot.h"
#else
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#endif

struct ForecastErrorWindow::Impl {
    QLabel* rmseLabel = nullptr;
    QLabel* maeLabel = nullptr;
    QLabel* mapeLabel = nullptr;
    QLabel* ciLabel = nullptr;
#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
    QCustomPlot* plot = nullptr;
#else
    QtCharts::QChartView* chartView = nullptr;
#endif
};

ForecastErrorWindow::ForecastErrorWindow(QWidget* parent) : QDialog(parent), impl_(std::make_unique<Impl>()) {
    setWindowTitle("Forecast Error");
    resize(600, 500);

    auto* layout = new QVBoxLayout(this);

    auto* form = new QFormLayout();
    impl_->rmseLabel = new QLabel("-");
    impl_->maeLabel = new QLabel("-");
    impl_->mapeLabel = new QLabel("-");
    impl_->ciLabel = new QLabel("-");
    form->addRow("RMSE:", impl_->rmseLabel);
    form->addRow("MAE:", impl_->maeLabel);
    form->addRow("MAPE (%):", impl_->mapeLabel);
    form->addRow("CI (95%):", impl_->ciLabel);
    layout->addLayout(form);

#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
    impl_->plot = new QCustomPlot(this);
    layout->addWidget(impl_->plot);
#else
    impl_->chartView = new QtCharts::QChartView(this);
    impl_->chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(impl_->chartView);
#endif
}

void ForecastErrorWindow::setResult(const curvefit::ForecastErrorResult& result) {
    impl_->rmseLabel->setText(QString::number(result.rmse));
    impl_->maeLabel->setText(QString::number(result.mae));
    impl_->mapeLabel->setText(QString::number(result.mape));
    impl_->ciLabel->setText(QString("[%1, %2]").arg(result.ci_lower).arg(result.ci_upper));
}

void ForecastErrorWindow::setForecastPlot(const std::vector<double>& x, const std::vector<double>& y,
                                         const std::vector<double>& ciLower,
                                         const std::vector<double>& ciUpper) {
    (void)ciLower;
    (void)ciUpper;
#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
    if (!impl_->plot) return;
    impl_->plot->clearGraphs();
    impl_->plot->addGraph();
    QVector<double> xv, yv;
    for (size_t i = 0; i < x.size(); ++i) {
        xv.append(x[i]);
        yv.append(y[i]);
    }
    impl_->plot->graph(0)->setData(xv, yv);
    impl_->plot->rescaleAxes();
    impl_->plot->replot();
#else
    if (!impl_->chartView) return;
    auto* chart = impl_->chartView->chart();
    chart->removeAllSeries();
    auto* series = new QtCharts::QLineSeries();
    series->setName("Forecast");
    for (size_t i = 0; i < x.size(); ++i) {
        series->append(x[i], y[i]);
    }
    chart->addSeries(series);
    chart->createDefaultAxes();
#endif
}
