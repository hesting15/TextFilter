#include "PlainTextEdit.h"
#include <QDebug>
#include <QApplication>
#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>
#include <Settings.h>
#include <QMessageBox>
#include <QPainter>
#include <QTextDocumentFragment>

PlainTextEdit::PlainTextEdit(QWidget *parent)
  : QPlainTextEdit(parent)
  , mIsDirty(false)
{
    lineNumberArea = new LineNumberArea(this);

    connect(
       this,
       SIGNAL(blockCountChanged(int)),
       this,
       SLOT(updateLineNumberAreaWidth(int)));

    connect(
       this,
       SIGNAL(updateRequest(QRect,int)),
       this,
       SLOT(updateLineNumberArea(QRect,int)));

    updateLineNumberAreaWidth(0);
}

void PlainTextEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton
       && QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
    {
        int line =
            event->pos().y() / fontMetrics().height()
            + verticalScrollBar()->value();

        QTextCursor cursor(document()->findBlockByLineNumber(line));
        cursor.movePosition(QTextCursor::EndOfBlock);
        cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
        setTextCursor(cursor);
        copy();
        return;
    }

    QPlainTextEdit::mousePressEvent(event);
}

void PlainTextEdit::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
    {
        int zoomLevel = 1;
        if (font().pointSize() > 45)
        {
            zoomLevel = 8;
        }
        else if (font().pointSize() > 25)
        {
            zoomLevel = 4;
        }
        else if (font().pointSize() > 12)
        {
            zoomLevel = 2;
        }

        if(event->angleDelta().y() > 0)
        {
            zoomIn(zoomLevel);
        }
        else
        {
            zoomOut(zoomLevel);
        }
        Settings::getInstance().setFont(font());
        updateTabWidth();
        return;
    }

    QPlainTextEdit::wheelEvent(event);
}

void PlainTextEdit::setPlainText(const QString &text)
{
    QPlainTextEdit::setPlainText(text);
    document()->setModified(false);
    mIsDirty = false;
}

void PlainTextEdit::gotoLineNumber(const int lineNumber)
{
    // Show current line in the middle of the text edit
    int numberOfVisibleLines = height() / fontMetrics().height();
    verticalScrollBar()->setValue(lineNumber - numberOfVisibleLines/2);
}

int PlainTextEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 8 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void PlainTextEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void PlainTextEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
    {
        lineNumberArea->scroll(0, dy);
    }
    else
    {
        lineNumberArea->update(
            0,
            rect.y(),
            lineNumberArea->width(),
            rect.height());
    }

    if (rect.contains(viewport()->rect()))
    {
        updateLineNumberAreaWidth(0);
    }
}

void PlainTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(
        QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void PlainTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(233,233,233));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();

    int top =
        (int)blockBoundingGeometry(block)
        .translated(contentOffset()).top();

    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(140,140,140));
            painter.drawText(
                0,
                top,
                lineNumberArea->width()-5,
                fontMetrics().height(),
                Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void PlainTextEdit::setTextFromOtherDocument(
    std::shared_ptr<QTextDocument> otherDocument)
{
    QTextCursor otherCursor(otherDocument.get());
    otherCursor.select(QTextCursor::Document);

    QTextCursor cursor(document());
    cursor.select(QTextCursor::Document);

    if (otherCursor.selectedText().isEmpty())
    {
        cursor.removeSelectedText();
    }
    else
    {
        cursor.insertFragment(otherCursor.selection());
    }
}

void PlainTextEdit::updateTabWidth()
{
    setTabStopDistance(QFontMetricsF(font()).horizontalAdvance(' ') * 4);
}
