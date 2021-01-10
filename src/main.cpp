#include "mainwindow.h"
#include "wizard.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSurfaceFormat fmt;
    fmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(fmt);

    //set the default font
    app.setFont(QFont("calibri",10));

    Wizard wizard;
    MainWindow window;

    wizard.open();

    //connect Mainwindow and the Wizard
    app.connect(&wizard, &Wizard::accepted, [&window] {
        window.show();
    });

    app.connect(&window, &MainWindow::cancel,[&wizard] {
        wizard.open();
    });

    app.connect(&wizard, &Wizard::resultReady, &window, &MainWindow::dealWithWizard);

    return app.exec();
}
