#include "plaintextedit.h"
#include <QDebug>
#include <QApplication>
#include <QTextCursor>
#include <QTextBlock>
#include <QToolTip>
#include <QScrollBar>
#include <settings.h>

PlainTextEdit::PlainTextEdit(QWidget *parent) : QPlainTextEdit(parent)
{
}

void PlainTextEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
    {
        int line = event->pos().y() / fontMetrics().height() + verticalScrollBar()->value();
        QTextCursor cursor(document()->findBlockByLineNumber(line));
        cursor.select(QTextCursor::LineUnderCursor);
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
        if(event->delta() > 0)
        {
            zoomIn(2);
        }
        else
        {
            zoomOut(2);
        }
        Settings::getInstance().setFont(font());
        return;
    }

    QPlainTextEdit::wheelEvent(event);
}
