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

    bool isFiltering()           { return mIsFiltering;  }
    bool isInFilterMode()        { return mInFilterMode; }
    bool isOriginalTextChanged() { return mIsOriginalTextChanged; }
    int applyFilter(const QString &filter);
    void clearFilterAndLoadTextFromFile();
    void loadTextFromFile();

    void loadFile(const QString &filename);
    void saveFile(const QString &filename);
    void gotoNextFilteredLine();
    void gotoPreviousFilteredLine();
    void highlightEverySecondLine();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    int getCurrentLineNumber(QTextCursor cursor);
    void setCurrentLineNumber(const int lineNumber);

protected:
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;

    Memo mMemo;
    QString mFilter;
    bool mIsFiltering;
    bool mInFilterMode;
    bool mButtonPressedWhileFiltering;
    std::vector<FilteredLine> mFilteredLines;
    int mCurrentFilteredLineNumber;
    bool mIsOriginalTextChanged;

    void highlightFilteredLine();
    void higlightTextInLine(const int lineNumber, const bool highlight, std::vector<std::pair<int, int>> highlightPositions);
    void reapplyFilterIfNeeded();
    void clearHighlighting();
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(PlainTextEdit *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    PlainTextEdit *codeEditor;
};

#endif // PLAINTEXTEDIT_H
