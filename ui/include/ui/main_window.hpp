#pragma once

#include <QMainWindow>
#include <QScopedPointer>

class PlotView;
class DataManager;
class ResidualsWindow;
class SymmetryWindow;
class ForecastErrorWindow;
struct MainWindowPrivate;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onImportCsv();
    void onImportJson();
    void onExportCsv();
    void onFit();
    void onModelSelected(int index);
    void onShowResiduals();
    void onShowSymmetry();
    void onShowForecastError();
    void onMonteCarlo();

private:
    void setupUi();
    void setupMenu();
    void setupDockWidgets();
    void loadPlugins();

    QScopedPointer<MainWindowPrivate> d;
};
