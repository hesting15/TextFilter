#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <settings.h>
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

protected:
    bool event(QEvent *event);
    void closeEvent(QCloseEvent *event);
private slots:
    void on_lineEditSearch_textChanged(const QString &arg1);
    void filterTextAction();
    void copyLineAction();
    void changeFontAction();
    void alwaysOnTopAction();
    void loadFileAction();
    void saveFile();
    void saveFileAction();
    void saveFileAsAction();
    void helpAction();

    void on_lineEditSearch_returnPressed();

    void on_toolButtonPrevious_clicked();

    void on_toolButtonNext_clicked();

private:
    Ui::MainWindow *ui;
    Settings mSettings;
    Memo mMemo;
    bool mIsFiltering;
    std::vector<FilteredLine> mFilteredLines;
    int mCurrentFilteredLineNumber;

    void createMenuActions();
    void loadFile(const QString &fileName);
    void higlightTextInLine(const int lineNumber, const bool underline, std::vector<std::pair<int, int> > highlightPositions);
    QBrush getDefaultBackgroungColor();
    void highlightCurrentLine();
    void setAlwaysOnTop();
    void setPlainTextFromFile();
};

#endif // MAINWINDOW_H
