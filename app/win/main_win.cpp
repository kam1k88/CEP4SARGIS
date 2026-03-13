#include "ui/main_window.hpp"
#include <QApplication>
#include <QLocale>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("CurveFit");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("CurveFit");

    MainWindow w;
    w.show();
    return app.exec();
}
