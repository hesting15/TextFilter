#include "Document.h"

#include <QPlainTextEdit>
#include <QTextBlock>
#include <QDebug>


Document::Document(QTextDocument* document)
    : mDoc(document->clone())
    , mFilter("")
    , mCurrentHighlightedLine(-1)
    , mUndoHistoryPoint(document->availableUndoSteps())
{
    mDoc->setModified(document->isModified());
}

std::shared_ptr<QTextDocument> Document::cloneDocument()
{
    std::shared_ptr<QTextDocument> newDocument(mDoc->clone());
    return newDocument;
}

void Document::applyFilter(const QString& filter)
{
    mFilter = filter;
    mCurrentHighlightedLine = -1;
    mHighlightAreas.clear();

    if (mFilter.isEmpty())
    {
        return;
    }

    QTextBlock block = mDoc->begin();
    while (block.isValid())
    {
        MatchResult result = isLineMatchesFiter(block.text(), filter);
        if (result.result)
        {
            mHighlightAreas[block.blockNumber()] = result.highlightAreas;
        }

        block = block.next();
    }
}

Document::MatchResult Document::isLineMatchesFiter(
        const QString& line,
        const QString& filter)
{
    MatchResult result;
    QStringList filterItems = filter.split(" ");

    int fromIndex = 0;

    for (const QString& filterItem : filterItems)
    {
        int indexOf = line.indexOf(filterItem, fromIndex, Qt::CaseInsensitive);
        if (indexOf == -1)
        {
            return result;
        }
        result.highlightAreas.emplace_back(indexOf, indexOf+filterItem.length());
        fromIndex = indexOf + filterItem.length();
    }
    result.result = true;
    return result;
}

void Document::highlightMatchedText(
        QTextBlock& block,
        const std::vector<HighlightArea>& highlightAreas,
        bool highlightWholeLine)
{
    QTextCursor cursor(block);
    int beginningOfLine = cursor.position();

    if (highlightWholeLine)
    {
        QTextCharFormat fmt;
        fmt.setBackground(QColor(233,233,233));
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.setCharFormat(fmt);
    }

    QTextCharFormat fmt;
    fmt.setBackground(highlightWholeLine ? Qt::green : Qt::yellow);

    for (auto& area : highlightAreas)
    {
        cursor.setPosition(beginningOfLine + area.begin, QTextCursor::MoveAnchor);
        cursor.setPosition(beginningOfLine + area.end,   QTextCursor::KeepAnchor);
        cursor.setCharFormat(fmt);
    }
}

void Document::removeLine(const QTextBlock& block)
{
    QTextCursor cursor(block);
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
}

std::shared_ptr<QTextDocument> Document::getFilteredDocument()
{
    std::shared_ptr<QTextDocument> newDocument = cloneDocument();
    bool highlightWholeLine = false;

    QTextBlock block = newDocument->lastBlock();
    while (block.isValid())
    {
        auto it = mHighlightAreas.find(block.blockNumber());
        if (it == mHighlightAreas.end())
        {
            if (block.previous().isValid())
            {
                block = block.previous();
                removeLine(block.next());
            }
            else
            {
                removeLine(block);
            }
        }
        else
        {
            std::vector<HighlightArea>& highlightAreas = it->second;
            highlightMatchedText(block, highlightAreas, highlightWholeLine);
            block = block.previous();
        }
    }
    return newDocument;
}

std::shared_ptr<QTextDocument> Document::getFullDocumentWithHighlightedLine()
{
    std::shared_ptr<QTextDocument> newDocument = cloneDocument();

    QTextBlock block = newDocument->firstBlock();
    while (block.isValid())
    {
        auto it = mHighlightAreas.find(block.blockNumber());
        if (it != mHighlightAreas.end())
        {
            std::vector<HighlightArea>& highlightAreas = it->second;
            bool highlightWholeLine = (block.blockNumber()==mCurrentHighlightedLine);

            highlightMatchedText(block, highlightAreas, highlightWholeLine);
        }
        block = block.next();
    }

    return newDocument;
}

std::shared_ptr<QTextDocument> Document::getFullDocumentWithPrevLineHighlighted()
{
    if (!mHighlightAreas.empty())
    {
        auto it = mHighlightAreas.find(mCurrentHighlightedLine);
        if (it == mHighlightAreas.begin())
        {
            it = mHighlightAreas.end();
        }
        --it;

        mCurrentHighlightedLine = it->first;
    }

    return getFullDocumentWithHighlightedLine();
}

std::shared_ptr<QTextDocument> Document::getFullDocumentWithNextLineHighlighted()
{
    if (!mHighlightAreas.empty())
    {
        auto it = mHighlightAreas.find(mCurrentHighlightedLine);
        if (it != mHighlightAreas.end())
        {
            ++it;
        }

        if (it == mHighlightAreas.end())
        {
            it = mHighlightAreas.begin();
        }

        mCurrentHighlightedLine = it->first;
    }

    return getFullDocumentWithHighlightedLine();
}
