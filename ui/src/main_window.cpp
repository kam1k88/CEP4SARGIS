#include "ui/main_window.hpp"
#include "ui/plot_view.hpp"
#include "ui/data_manager.hpp"
#include "ui/residuals_window.hpp"
#include "ui/symmetry_window.hpp"
#include "ui/forecast_error_window.hpp"
#include "core/regression/regression_engine.hpp"
#include "core/analysis/residuals_analyzer.hpp"
#include "core/analysis/symmetry_analyzer.hpp"
#include "core/analysis/forecast_error.hpp"
#include "core/plugins/plugin_loader.hpp"
#include "core/io/csv_io.hpp"
#include "core/io/json_io.hpp"
#include "platform/os_utils.hpp"
#include <QDockWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <algorithm>
#include <cmath>

struct MainWindowPrivate {
    PlotView* plotView = nullptr;
    DataManager* dataManager = nullptr;
    QFormLayout* paramFormLayout = nullptr;
    std::vector<double> lastFitParams;
    curvefit::IRegressionModel* lastFitModel = nullptr;
    ResidualsWindow* residualsWindow = nullptr;
    SymmetryWindow* symmetryWindow = nullptr;
    ForecastErrorWindow* forecastErrorWindow = nullptr;

    QComboBox* modelCombo = nullptr;
    QPlainTextEdit* resultsEdit = nullptr;
    QList<QDoubleSpinBox*> paramSpins;

    curvefit::RegressionEngine engine;
    curvefit::PluginLoader pluginLoader;
    curvefit::ResidualsAnalyzer residualsAnalyzer;
    curvefit::SymmetryAnalyzer symmetryAnalyzer;
    curvefit::ForecastError forecastError;

    curvefit::IRegressionModel* currentModel() {
        const int idx = modelCombo ? modelCombo->currentIndex() : -1;
        auto models = engine.models();
        if (idx >= 0 && idx < static_cast<int>(models.size())) {
            return models[static_cast<size_t>(idx)];
        }
        return nullptr;
    }

    std::vector<double> getInitialParams() {
        std::vector<double> params;
        for (auto* spin : paramSpins) {
            if (spin) params.push_back(spin->value());
        }
        return params;
    }
};

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), d(new MainWindowPrivate) {
    setupUi();
    setupMenu();
    setupDockWidgets();
    d->engine.registerBuiltinModels();
    loadPlugins();

    for (auto* model : d->engine.models()) {
        d->modelCombo->addItem(QString::fromStdString(model->name()));
    }
    onModelSelected(0);

    connect(d->dataManager, &DataManager::dataChanged, this, [this]() {
        d->plotView->setData(d->dataManager->dataset());
    });
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    setWindowTitle("CurveFit - Regression Analysis");
    resize(1200, 800);

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    d->plotView = new PlotView(this);
    layout->addWidget(d->plotView);

    auto* rightPanel = new QGroupBox("Model and Fit");
    d->paramFormLayout = new QFormLayout(rightPanel);
    auto* formLayout = d->paramFormLayout;

    d->modelCombo = new QComboBox(this);
    connect(d->modelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModelSelected);
    formLayout->addRow("Model:", d->modelCombo);

    auto* fitBtn = new QPushButton("Fit", this);
    connect(fitBtn, &QPushButton::clicked, this, &MainWindow::onFit);
    formLayout->addRow(fitBtn);

    d->resultsEdit = new QPlainTextEdit(this);
    d->resultsEdit->setReadOnly(true);
    formLayout->addRow(d->resultsEdit);

    auto* analysisBtns = new QHBoxLayout();
    auto* residualsBtn = new QPushButton("Residuals", this);
    auto* symmetryBtn = new QPushButton("Symmetry", this);
    auto* forecastBtn = new QPushButton("Forecast Error", this);
    auto* mcBtn = new QPushButton("Monte Carlo", this);
    connect(residualsBtn, &QPushButton::clicked, this, &MainWindow::onShowResiduals);
    connect(symmetryBtn, &QPushButton::clicked, this, &MainWindow::onShowSymmetry);
    connect(forecastBtn, &QPushButton::clicked, this, &MainWindow::onShowForecastError);
    connect(mcBtn, &QPushButton::clicked, this, &MainWindow::onMonteCarlo);
    analysisBtns->addWidget(residualsBtn);
    analysisBtns->addWidget(symmetryBtn);
    analysisBtns->addWidget(forecastBtn);
    analysisBtns->addWidget(mcBtn);
    formLayout->addRow(analysisBtns);

    layout->addWidget(rightPanel);
    setCentralWidget(central);
}

void MainWindow::setupMenu() {
    auto* fileMenu = menuBar()->addMenu("&File");

    auto* importCsv = fileMenu->addAction("Import CSV...");
    importCsv->setShortcut(QKeySequence::Open);
    connect(importCsv, &QAction::triggered, this, &MainWindow::onImportCsv);

    auto* importJson = fileMenu->addAction("Import JSON...");
    connect(importJson, &QAction::triggered, this, &MainWindow::onImportJson);

    fileMenu->addSeparator();

    auto* exportCsv = fileMenu->addAction("Export CSV...");
    connect(exportCsv, &QAction::triggered, this, &MainWindow::onExportCsv);

    fileMenu->addSeparator();
    fileMenu->addAction("Exit", qApp, &QApplication::quit)->setShortcut(QKeySequence::Quit);
}

void MainWindow::setupDockWidgets() {
    auto* dataDock = new QDockWidget("Data", this);
    d->dataManager = new DataManager(this);
    dataDock->setWidget(d->dataManager);
    addDockWidget(Qt::RightDockWidgetArea, dataDock);
}

void MainWindow::loadPlugins() {
    QString pluginsDir = curvefit::platform::getPluginsDirectory();
    d->pluginLoader.loadFromDirectory(pluginsDir);
    for (auto* model : d->pluginLoader.loadedModels()) {
        d->engine.addModel(model);
        d->modelCombo->addItem(QString::fromStdString(model->name()));
    }
}

void MainWindow::onImportCsv() {
    QString path = QFileDialog::getOpenFileName(this, "Import CSV", QString(),
                                                "CSV (*.csv);;All (*)");
    if (path.isEmpty()) return;
    auto data = curvefit::io::loadCsv(path);
    if (data) {
        d->dataManager->setDataset(*data);
        d->plotView->setData(*data);
    } else {
        QMessageBox::warning(this, "Import", "Failed to load CSV.");
    }
}

void MainWindow::onImportJson() {
    QString path = QFileDialog::getOpenFileName(this, "Import JSON", QString(),
                                                "JSON (*.json);;All (*)");
    if (path.isEmpty()) return;
    auto data = curvefit::io::loadJson(path);
    if (data) {
        d->dataManager->setDataset(*data);
        d->plotView->setData(*data);
    } else {
        QMessageBox::warning(this, "Import", "Failed to load JSON.");
    }
}

void MainWindow::onExportCsv() {
    QString path = QFileDialog::getSaveFileName(this, "Export CSV", QString(),
                                                "CSV (*.csv);;All (*)");
    if (path.isEmpty()) return;
    if (!curvefit::io::saveCsv(d->dataManager->dataset(), path)) {
        QMessageBox::warning(this, "Export", "Failed to save CSV.");
    }
}

void MainWindow::onModelSelected(int) {
    while (!d->paramSpins.isEmpty()) {
        auto* spin = d->paramSpins.takeFirst();
        if (d->paramFormLayout) {
            d->paramFormLayout->removeRow(spin);
        }
        spin->deleteLater();
    }

    auto* model = d->currentModel();
    if (!model || !d->paramFormLayout) return;

    auto names = model->paramNames();
    for (const auto& name : names) {
        auto* spin = new QDoubleSpinBox(this);
        spin->setRange(-1e10, 1e10);
        spin->setDecimals(6);
        spin->setValue(1.0);
        d->paramSpins.append(spin);
        d->paramFormLayout->insertRow(1, QString::fromStdString(name) + ":", spin);
    }
}

void MainWindow::onFit() {
    auto* model = d->currentModel();
    if (!model) {
        QMessageBox::warning(this, "Fit", "No model selected.");
        return;
    }

    auto data = d->dataManager->dataset();
    if (data.size() < 2) {
        QMessageBox::warning(this, "Fit", "Not enough data points.");
        return;
    }

    auto initialParams = d->getInitialParams();
    if (initialParams.size() != model->paramCount()) {
        QMessageBox::warning(this, "Fit", "Initial parameters count mismatch.");
        return;
    }

    auto result = d->engine.fit(data, model, initialParams);

    QString text;
    text += QString("Success: %1\n").arg(result.success ? "Yes" : "No");
    text += QString("Iterations: %1\n").arg(result.iterations);
    text += QString("RMSE: %1\n").arg(result.rmse);
    text += QString("R²: %1\n").arg(result.r_squared);
    text += QString("\nMessage: %1\n").arg(QString::fromStdString(result.message));
    text += "\nParameters:\n";
    auto names = model->paramNames();
    for (size_t i = 0; i < result.parameters.size(); ++i) {
        QString name = (i < names.size()) ? QString::fromStdString(names[i]) : QString("p%1").arg(i);
        text += QString("  %1 = %2\n").arg(name).arg(result.parameters[i]);
    }
    d->resultsEdit->setPlainText(text);

    if (result.success) {
        d->lastFitParams = result.parameters;
        d->lastFitModel = model;
        for (size_t i = 0; i < result.parameters.size() && i < static_cast<size_t>(d->paramSpins.size()); ++i) {
            d->paramSpins[static_cast<int>(i)]->setValue(result.parameters[i]);
        }
        double xMin = *std::min_element(data.x.begin(), data.x.end());
        double xMax = *std::max_element(data.x.begin(), data.x.end());
        const int pts = 200;
        std::vector<double> fitX, fitY;
        for (int i = 0; i <= pts; ++i) {
            double x = xMin + (xMax - xMin) * i / pts;
            fitX.push_back(x);
            fitY.push_back(model->evaluate(x, result.parameters.data()));
        }
        d->plotView->setFitCurve(fitX, fitY);
    }
}

void MainWindow::onShowResiduals() {
    auto* model = d->lastFitModel ? d->lastFitModel : d->currentModel();
    auto data = d->dataManager->dataset();
    if (!model || data.empty()) return;

    std::vector<double> params = !d->lastFitParams.empty() ? d->lastFitParams : d->getInitialParams();
    if (params.size() != model->paramCount()) return;

    auto res = d->residualsAnalyzer.analyze(data, model, params);
    if (!d->residualsWindow) {
        d->residualsWindow = new ResidualsWindow(this);
    }
    d->residualsWindow->setData(data.x, res.residuals);
    d->residualsWindow->setStats(res.mean, res.variance, res.skewness, res.kurtosis);
    d->residualsWindow->show();
}

void MainWindow::onShowSymmetry() {
    auto* model = d->lastFitModel ? d->lastFitModel : d->currentModel();
    auto data = d->dataManager->dataset();
    if (!model || data.empty()) return;

    std::vector<double> params = !d->lastFitParams.empty() ? d->lastFitParams : d->getInitialParams();
    if (params.size() != model->paramCount()) return;

    auto res = d->residualsAnalyzer.analyze(data, model, params);
    auto sym = d->symmetryAnalyzer.analyze(res.residuals, 20);

    if (!d->symmetryWindow) {
        d->symmetryWindow = new SymmetryWindow(this);
    }
    d->symmetryWindow->setHistogram(sym.histogram, sym.mirroredHistogram);
    d->symmetryWindow->setSkewness(sym.skewness);
    d->symmetryWindow->show();
}

void MainWindow::onShowForecastError() {
    auto* model = d->lastFitModel ? d->lastFitModel : d->currentModel();
    auto data = d->dataManager->dataset();
    if (!model || data.empty()) return;

    std::vector<double> params = !d->lastFitParams.empty() ? d->lastFitParams : d->getInitialParams();
    if (params.size() != model->paramCount()) return;

    auto err = d->forecastError.evaluate(data, model, params);

    if (!d->forecastErrorWindow) {
        d->forecastErrorWindow = new ForecastErrorWindow(this);
    }
    d->forecastErrorWindow->setResult(err);
    d->forecastErrorWindow->show();
}

void MainWindow::onMonteCarlo() {
    QMessageBox::information(this, "Monte Carlo",
        "Monte Carlo simulation: use the API from core/montecarlo/monte_carlo_engine.hpp.\n"
        "This demo does not expose a full UI yet.");
}
