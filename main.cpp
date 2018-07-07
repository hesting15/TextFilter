#include "mainwindow.h"
#include <QApplication>
#include <Windows.h>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
