#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memo.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadSettings();

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

private:
    void createMenuActions();
    QBrush getDefaultBackgroungColor();
    void loadFile(const QString &fileName);
    void higlightTextInLine(const int lineNumber, const bool underline, std::vector<std::pair<int, int>> highlightPositions);
    void highlightCurrentLine();
    void setAlwaysOnTop();
    void setPlainTextFromFile();

    Ui::MainWindow *ui;
    Memo mMemo;
    bool mIsFiltering;
    std::vector<FilteredLine> mFilteredLines;
    int mCurrentFilteredLineNumber;
};

#endif // MAINWINDOW_H
