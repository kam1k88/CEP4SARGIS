#include "ui/plot_view.hpp"
#include <QVBoxLayout>

#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
#include "qcustomplot.h"
#else
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#endif

struct PlotView::Impl {
#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
    QCustomPlot* plot = nullptr;
#else
    QWidget* chartWidget = nullptr;
    QtCharts::QChart* chart = nullptr;
    QtCharts::QLineSeries* dataSeries = nullptr;
    QtCharts::QScatterSeries* dataPoints = nullptr;
    QtCharts::QLineSeries* fitSeries = nullptr;
    QtCharts::QLineSeries* residualSeries = nullptr;
#endif
};

PlotView::PlotView(QWidget* parent) : QWidget(parent), impl_(std::make_unique<Impl>()) {
    auto* layout = new QVBoxLayout(this);

#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
    impl_->plot = new QCustomPlot(this);
    layout->addWidget(impl_->plot);
#else
    impl_->chart = new QtCharts::QChart();
    impl_->dataSeries = new QtCharts::QLineSeries();
    impl_->dataSeries->setName("Data");
    impl_->dataPoints = new QtCharts::QScatterSeries();
    impl_->dataPoints->setName("Points");
    impl_->dataPoints->setMarkerSize(6);
    impl_->fitSeries = new QtCharts::QLineSeries();
    impl_->fitSeries->setName("Fit");
    impl_->fitSeries->setColor(Qt::red);

    impl_->chart->addSeries(impl_->dataSeries);
    impl_->chart->addSeries(impl_->dataPoints);
    impl_->chart->addSeries(impl_->fitSeries);
    impl_->chart->createDefaultAxes();
    impl_->chart->setTitle("Data and Fit");

    auto* chartView = new QtCharts::QChartView(impl_->chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QtCharts::QChartView::RectangleRubberBand);
    layout->addWidget(chartView);
    impl_->chartWidget = chartView;
#endif
}

PlotView::~PlotView() = default;

void PlotView::setData(const curvefit::Dataset& data) {
#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
    if (!impl_->plot) return;
    impl_->plot->clearGraphs();
    impl_->plot->addGraph();
    QVector<double> xv, yv;
    for (size_t i = 0; i < data.size(); ++i) {
        xv.append(data.x[i]);
        yv.append(data.y[i]);
    }
    impl_->plot->graph(0)->setData(xv, yv);
    impl_->plot->rescaleAxes();
    impl_->plot->replot();
#else
    impl_->dataSeries->clear();
    impl_->dataPoints->clear();
    for (size_t i = 0; i < data.size(); ++i) {
        impl_->dataSeries->append(data.x[i], data.y[i]);
        impl_->dataPoints->append(data.x[i], data.y[i]);
    }
    impl_->chart->createDefaultAxes();
#endif
}

void PlotView::setFitCurve(const std::vector<double>& x, const std::vector<double>& y) {
#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
    if (!impl_->plot || impl_->plot->graphCount() < 2) {
        if (impl_->plot) impl_->plot->addGraph();
    }
    QVector<double> xv, yv;
    for (size_t i = 0; i < x.size(); ++i) {
        xv.append(x[i]);
        yv.append(y[i]);
    }
    impl_->plot->graph(1)->setData(xv, yv);
    impl_->plot->graph(1)->setPen(QPen(Qt::red, 2));
    impl_->plot->rescaleAxes();
    impl_->plot->replot();
#else
    impl_->fitSeries->clear();
    for (size_t i = 0; i < x.size(); ++i) {
        impl_->fitSeries->append(x[i], y[i]);
    }
    impl_->chart->createDefaultAxes();
#endif
}

void PlotView::setResiduals(const std::vector<double>& x, const std::vector<double>& residuals) {
    (void)x;
    (void)residuals;
#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
    // Add residuals as second plot
#else
    if (impl_->residualSeries) {
        impl_->residualSeries->clear();
        for (size_t i = 0; i < x.size(); ++i) {
            impl_->residualSeries->append(x[i], residuals[i]);
        }
    }
#endif
}

void PlotView::clearFit() {
#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
    if (impl_->plot && impl_->plot->graphCount() > 1) {
        impl_->plot->graph(1)->data()->clear();
        impl_->plot->replot();
    }
#else
    impl_->fitSeries->clear();
#endif
}

void PlotView::clearResiduals() {
#if defined(CURVEFIT_QCUSTOMPLOT) && CURVEFIT_QCUSTOMPLOT
#else
    if (impl_->residualSeries) impl_->residualSeries->clear();
#endif
}
