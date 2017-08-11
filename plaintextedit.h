#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H
#include <QPlainTextEdit>
#include <memo.h>

class PlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit PlainTextEdit(QWidget *parent = Q_NULLPTR);

    QBrush getDefaultBackgroungColor();

    bool isFiltering() { return mIsFiltering; }
    int applyFilter(const QString &filter);
    void clearFilterAndLoadTextFromFile();
    void loadTextFromFile();

    void loadFile(const QString &filename);
    void saveFile(const QString &filename);
    void gotoNextFilteredLine();
    void gotoPreviousFilteredLine();
protected:
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    Memo mMemo;
    QString mFilter;
    bool mIsFiltering;
    bool mInFilterMode;
    bool mButtonPressedWhileFiltering;
    std::vector<FilteredLine> mFilteredLines;
    int mCurrentFilteredLineNumber;

    void highlightCurrentLine();
    void higlightTextInLine(const int lineNumber, const bool highlight, std::vector<std::pair<int, int>> highlightPositions);
    void reapplyFilterIfNeeded();
    void clearHighlighting();
};

#endif // PLAINTEXTEDIT_H
