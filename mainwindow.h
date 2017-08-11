#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    bool event(QEvent *event);
    void closeEvent(QCloseEvent *event);
private slots:
    void on_lineEditSearch_textChanged(const QString &arg1);
    void on_lineEditSearch_returnPressed();
    void on_toolButtonPrevious_clicked();
    void on_toolButtonNext_clicked();

    void filterTextAction();
    void copyLineAction();
    void alwaysOnTopAction();
    void loadFileAction();
    void saveFile();
    void saveFileAction();
    void saveFileAsAction();
    void helpAction();
    void settingsAction();
    void copyMultipleLinesAction();

private:
    void loadSettings();
    void createMenuActions();
    void loadFile(const QString &fileName);
    void setAlwaysOnTop();
    void loadTextFromFile();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
