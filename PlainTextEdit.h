#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <FileManager.h>

class PlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:

    explicit PlainTextEdit(QWidget *parent = Q_NULLPTR);

    void setPlainText(const QString &text);
    void setTextFromOtherDocument(std::shared_ptr<QTextDocument> otherDocument);

    // LineNumberArea
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    int getCurrentLineNumber(QTextCursor cursor);

    void gotoLineNumber(const int lineNumber);

    bool isDirty() const { return mIsDirty; }
    void setDirty(bool isDirty) { mIsDirty = isDirty; }

    void updateTabWidth();

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:

    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
    bool mIsDirty;
};


class LineNumberArea : public QWidget
{
public:

    LineNumberArea(PlainTextEdit *editor) : QWidget(editor)
    {
        codeEditor = editor;
    }

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    PlainTextEdit *codeEditor;
};

#endif // PLAINTEXTEDIT_H
