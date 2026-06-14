#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Force Windows platform plugin to disable dark mode behavior
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowIcon(QIcon(":/Icon64.png"));
    w.show();

    return a.exec();
}
