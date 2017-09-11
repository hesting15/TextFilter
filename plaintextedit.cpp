#include "plaintextedit.h"
#include <QDebug>
#include <QApplication>
#include <QTextCursor>
#include <QTextBlock>
#include <QToolTip>
#include <QScrollBar>
#include <settings.h>
#include <QMessageBox>

static const int NO_CURRENT_FILTERED_LINE_NUMBER = -1;

PlainTextEdit::PlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent),
    mIsFiltering(false),
    mInFilterMode(false),
    mButtonPressedWhileFiltering(false)
{
}

void PlainTextEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
    {
        int line = event->pos().y() / fontMetrics().height() + verticalScrollBar()->value();
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

void PlainTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (mIsFiltering && !mButtonPressedWhileFiltering)
    {
        mButtonPressedWhileFiltering = true;
        clearHighlighting();
    }
    QPlainTextEdit::keyPressEvent(event);
}

void PlainTextEdit::clearHighlighting()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);
    QTextCharFormat fmt;
    fmt.setBackground(getDefaultBackgroungColor());
    cursor.setCharFormat(fmt);
}

QBrush PlainTextEdit::getDefaultBackgroungColor()
{
    return palette().brush(QPalette::ColorRole::Base);
}

int PlainTextEdit::applyFilter(const QString &filter)
{
    mFilter = filter;
    if (!mIsFiltering)
    {
        if (filter.length() < Settings::getInstance().getFilterThreshold())
        {
            return 0;
        }
        mIsFiltering = true;
        mButtonPressedWhileFiltering = false;
        mMemo.setText(toPlainText());
    }
    else if (filter.isEmpty())
    {
        clearFilterAndLoadTextFromFile();
        return 0;
    }

    mCurrentFilteredLineNumber = NO_CURRENT_FILTERED_LINE_NUMBER;
    mMemo.filterText(filter, mFilteredLines);

    clear();
    for (const FilteredLine& filteredLine : mFilteredLines)
    {
        appendPlainText(filteredLine.text);
        higlightTextInLine(document()->blockCount() - 1, false, filteredLine.highlightPositions);
    }
    mInFilterMode = true;

    return mFilteredLines.size();
}

void PlainTextEdit::clearFilterAndLoadTextFromFile()
{
    mIsFiltering = false;
    loadTextFromFile();
}

void PlainTextEdit::loadTextFromFile()
{
    mInFilterMode = false;
    clearHighlighting();
    setPlainText(mMemo.getText());
}

void PlainTextEdit::loadFile(const QString &filename)
{
    mIsFiltering = false;
    mMemo.loadFile(filename);
    loadTextFromFile();
}

void PlainTextEdit::saveFile(const QString &filename)
{
    mMemo.setText(toPlainText());

    try
    {
        mMemo.saveFile(filename);
    }
    catch(const std::runtime_error& ex)
    {
        QMessageBox::information(this, tr("Unable to save file"), ex.what());
    }
}

void PlainTextEdit::gotoNextFilteredLine()
{
    reapplyFilterIfNeeded();

    if (mIsFiltering && mFilteredLines.size() > 0)
    {
        if (mCurrentFilteredLineNumber == NO_CURRENT_FILTERED_LINE_NUMBER)
        {
            mCurrentFilteredLineNumber = 0;
            loadTextFromFile();
            for (const FilteredLine& filteredLine : mFilteredLines)
            {
                higlightTextInLine(filteredLine.lineNumber, false, filteredLine.highlightPositions);
            }
        }
        else
        {
            FilteredLine filteredLine = mFilteredLines[mCurrentFilteredLineNumber];
            higlightTextInLine(filteredLine.lineNumber, false, filteredLine.highlightPositions);

            ++mCurrentFilteredLineNumber;
            if (mCurrentFilteredLineNumber >= (int)mFilteredLines.size())
            {
                mCurrentFilteredLineNumber = 0;
            }
        }
        highlightCurrentLine();
    }
}

void PlainTextEdit::gotoPreviousFilteredLine()
{
    reapplyFilterIfNeeded();

    if (mIsFiltering && mFilteredLines.size() > 0)
    {
        if (mCurrentFilteredLineNumber == NO_CURRENT_FILTERED_LINE_NUMBER)
        {
            mCurrentFilteredLineNumber = mFilteredLines.size() - 1;
            loadTextFromFile();
            for (const FilteredLine& filteredLine : mFilteredLines)
            {
                higlightTextInLine(filteredLine.lineNumber, false, filteredLine.highlightPositions);
            }
        }
        else
        {
            FilteredLine filteredLine = mFilteredLines[mCurrentFilteredLineNumber];
            higlightTextInLine(filteredLine.lineNumber, false, filteredLine.highlightPositions);

            --mCurrentFilteredLineNumber;
            if (mCurrentFilteredLineNumber < 0)
            {
                mCurrentFilteredLineNumber =  mFilteredLines.size() - 1;
            }
        }
        highlightCurrentLine();
    }
}

void PlainTextEdit::reapplyFilterIfNeeded()
{
    if (mButtonPressedWhileFiltering && !mInFilterMode)
    {
        mButtonPressedWhileFiltering = false;
        mMemo.setText(toPlainText());
        mMemo.filterText(mFilter, mFilteredLines);
        if (mCurrentFilteredLineNumber > (int)mFilteredLines.size())
        {
            mCurrentFilteredLineNumber = NO_CURRENT_FILTERED_LINE_NUMBER;
        }
    }
}

void PlainTextEdit::highlightCurrentLine()
{
    FilteredLine filteredLine = mFilteredLines[mCurrentFilteredLineNumber];
    QTextCursor cursor(document()->findBlockByNumber(filteredLine.lineNumber));

    QTextCharFormat fmt;
    fmt.setBackground(getDefaultBackgroungColor());
    cursor.setCharFormat(fmt);

    higlightTextInLine(filteredLine.lineNumber, true, filteredLine.highlightPositions);
    setTextCursor(cursor);

    // Show current line in the middle of the text edit
    int numberOfVisibleLines = height() / fontMetrics().height();
    int currentLine = getCurrentLineNumber(cursor);
    verticalScrollBar()->setValue(currentLine - numberOfVisibleLines/2);
}

int PlainTextEdit::getCurrentLineNumber(QTextCursor cursor)
{
    cursor.movePosition(QTextCursor::StartOfLine);

    int lines = 1;
    while(cursor.positionInBlock() > 0)
    {
        cursor.movePosition(QTextCursor::Up);
        lines++;
    }

    QTextBlock block = cursor.block().previous();
    while(block.isValid())
    {
        lines += block.lineCount();
        block = block.previous();
    }
    return lines;
}

void PlainTextEdit::higlightTextInLine(const int lineNumber, const bool highlight, std::vector<std::pair<int, int>> highlightPositions)
{
    QTextCursor cursor(document()->findBlockByNumber(lineNumber));
    int beginningOfLine = cursor.position();

    // highlight whole line
    QTextCharFormat fmt;
    fmt.setBackground(highlight ? QColor(233,233,233) : getDefaultBackgroungColor());
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.setCharFormat(fmt);

    // higlight filtered text
    fmt.setBackground(highlight ? Qt::green : Qt::yellow);

    for (const auto& highlightPosition : highlightPositions)
    {
        cursor.setPosition(beginningOfLine + highlightPosition.first,  QTextCursor::MoveAnchor);
        cursor.setPosition(beginningOfLine + highlightPosition.second, QTextCursor::KeepAnchor);
        cursor.setCharFormat(fmt);
    }
}
