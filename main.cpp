#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("EECS 300");
    QCoreApplication::setApplicationName("Final Project Display");

    MainWindow w;
    w.show();
    return QApplication::exec();
}
