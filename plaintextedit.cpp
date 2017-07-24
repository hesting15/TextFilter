#include "plaintextedit.h"
#include <QDebug>
#include <QApplication>
#include <QTextCursor>
#include <QTextBlock>
#include <QToolTip>
#include <QScrollBar>

void PlainTextEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
    {
        int line = event->pos().y() / fontMetrics().height() + verticalScrollBar()->value();
        QTextCursor cursor(document()->findBlockByLineNumber(line));
        cursor.select(QTextCursor::LineUnderCursor);
        setTextCursor(cursor);
        copy();
    }
    else
    {
        QPlainTextEdit::mousePressEvent(event);
    }
}
