#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowIcon(QIcon(":/Icon64.png"));
    w.show();

    return a.exec();
}
