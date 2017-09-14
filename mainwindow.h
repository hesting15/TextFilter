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
    void closeEvent(QCloseEvent *event);
private slots:
    void on_lineEditSearch_textChanged(const QString &arg1);
    void on_lineEditSearch_returnPressed();
    void on_toolButtonPrevious_clicked();
    void on_toolButtonNext_clicked();

    void on_toolButtonOpenFile_clicked();
    void on_toolButtonSaveFile_clicked();
    void on_toolButtonSaveFileAs_clicked();
    void on_toolButtonFilterText_clicked();
    void on_toolButtonCopyLine_clicked();
    void on_toolButtonCopyMultipleLines_clicked();
    void on_toolButtonAlwaysOnTop_clicked();
    void on_toolButtonSettings_clicked();
    void on_toolButtonHelp_clicked();
    void on_toolButtonWordWrap_clicked();

    void on_pushButtonMenu_clicked();

private:
    void loadSettings();
    void createMenuActions();
    void saveFile();
    void loadFile(const QString &fileName);
    void setAlwaysOnTop();
    void setWordWrap();
    void loadTextFromFile();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
