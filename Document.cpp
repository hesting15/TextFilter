#include "Document.h"

#include <QPlainTextEdit>
#include <QTextBlock>
#include <QDebug>
#include <algorithm>

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
        MatchResult result = filterLine(block.text(), filter);
        if (result.result)
        {
            mHighlightAreas[block.blockNumber()] = result.highlightAreas;
        }

        block = block.next();
    }
}

Document::MatchResult Document::filterLine(
        const QString& line,
        const QString& filter)
{
    MatchResult result;
    QStringList filterItems = filter.split(' ', Qt::SkipEmptyParts);

    int fromIndex = 0;

    for (const QString &filterItem : std::as_const(filterItems))
    {
        int indexOf = line.indexOf(filterItem, fromIndex, Qt::CaseInsensitive);
        if (indexOf == -1)
        {
            return result;
        }
        result.highlightAreas.emplace_back(indexOf, indexOf + filterItem.length());
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
    // Get the document pointer (QTextBlock::document() returns const QTextDocument*)
    const QTextDocument* cdoc = block.document();
    if (!cdoc) return;
    QTextDocument* doc = const_cast<QTextDocument*>(cdoc);
    if (!doc) return;

    // If we want to highlight the whole line (gray background), do that first using a block-based cursor.
    if (highlightWholeLine)
    {
        QTextCursor wholeCursor(block);
        QTextCharFormat wholeFmt;
        wholeFmt.setBackground(QColor(233,233,233));
        wholeCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        wholeCursor.setCharFormat(wholeFmt);
    }

    QTextCharFormat fmt;
    fmt.setBackground(highlightWholeLine ? Qt::green : Qt::yellow);

    const int docCharCount = doc->characterCount();
    const int blockTextLen = block.text().length();
    const int blockStart = block.position();

    for (const auto& area : highlightAreas)
    {
        if (area.end <= area.begin)
            continue;

        int startPos = blockStart + area.begin;
        int endPos   = blockStart + area.end;

        // Never include the block separator — clamp to block text length
        const int maxEndPos = blockStart + blockTextLen;
        if (endPos > maxEndPos) endPos = maxEndPos;

        if (startPos < 0) startPos = 0;
        if (endPos < startPos) continue;
        if (startPos >= docCharCount) continue;
        if (endPos > docCharCount) endPos = docCharCount;

        QTextCursor selCursor(doc);
        selCursor.setPosition(startPos, QTextCursor::MoveAnchor);
        selCursor.setPosition(endPos,   QTextCursor::KeepAnchor);
        selCursor.setCharFormat(fmt);
    }
}

void Document::removeLine(const QTextBlock& block)
{
    QTextCursor cursor(block);
    cursor.select(QTextCursor::BlockUnderCursor);
    // Remove the block content
    cursor.removeSelectedText();
    // Remove the block separator (newline) if any
    cursor.deleteChar();
}

std::shared_ptr<QTextDocument> Document::getFilteredDocument()
{
    // Create a fresh document and append only matched blocks with highlighting.
    std::shared_ptr<QTextDocument> resultDoc = std::make_shared<QTextDocument>();

    // Ensure the new document has no leftover character formatting.
    {
        QTextCursor clearCursor(resultDoc.get());
        clearCursor.select(QTextCursor::Document);
        QTextCharFormat clearFmt; // default (no background)
        clearCursor.setCharFormat(clearFmt);
    }

    QTextBlock srcBlock = mDoc->firstBlock();
    QTextCursor dstCursor(resultDoc.get());
    bool firstInserted = false;

    while (srcBlock.isValid())
    {
        auto it = mHighlightAreas.find(srcBlock.blockNumber());
        if (it != mHighlightAreas.end())
        {
            // For the first inserted block, do not call insertBlock() because
            // the document is empty and insertBlock() would create a leading empty block.
            if (firstInserted)
            {
                dstCursor.insertBlock();
            }
            else
            {
                firstInserted = true;
            }

            // Insert the matched line text as formatted segments (default + highlighted + default)
            const QString lineText = srcBlock.text();
            // Sort highlight areas by begin just in case
            std::vector<HighlightArea> areas = it->second;
            std::sort(areas.begin(), areas.end(), [](const HighlightArea& a, const HighlightArea& b){
                return a.begin < b.begin;
            });

            int pos = 0;
            for (const auto& a : areas)
            {
                // Insert text before highlight (if any) with default format
                if (a.begin > pos)
                {
                    QTextCharFormat defaultFmt;
                    dstCursor.setCharFormat(defaultFmt);
                    dstCursor.insertText(lineText.mid(pos, a.begin - pos));
                }

                // Insert highlighted text
                if (a.end > a.begin)
                {
                    QTextCharFormat hlFmt;
                    hlFmt.setBackground(Qt::yellow);
                    dstCursor.setCharFormat(hlFmt);
                    dstCursor.insertText(lineText.mid(a.begin, a.end - a.begin));
                }

                pos = a.end;
            }

            // Insert any remaining tail text after last highlight
            if (pos < lineText.length())
            {
                QTextCharFormat defaultFmt;
                dstCursor.setCharFormat(defaultFmt);
                dstCursor.insertText(lineText.mid(pos));
            }
        }
        srcBlock = srcBlock.next();
    }

    return resultDoc;
}

std::shared_ptr<QTextDocument> Document::getFullDocumentWithHighlightedLine()
{
    std::shared_ptr<QTextDocument> newDocument = cloneDocument();

    // Clear any existing character formatting on the cloned document
    {
        QTextCursor clearCursor(newDocument.get());
        clearCursor.select(QTextCursor::Document);
        QTextCharFormat clearFmt;
        clearCursor.setCharFormat(clearFmt);
    }

    QTextBlock block = newDocument->firstBlock();
    while (block.isValid())
    {
        // store next block now because we may modify current block content
        QTextBlock nextBlock = block.next();

        auto it = mHighlightAreas.find(block.blockNumber());
        if (it != mHighlightAreas.end())
        {
            std::vector<HighlightArea> areas = it->second;
            std::sort(areas.begin(), areas.end(), [](const HighlightArea& a, const HighlightArea& b){
                return a.begin < b.begin;
            });

            bool highlightWholeLine = (block.blockNumber() == mCurrentHighlightedLine);

            // Rebuild the block content with formats using a cursor positioned at the block.
            QTextCursor blockCursor(block);
            blockCursor.select(QTextCursor::BlockUnderCursor);
            blockCursor.removeSelectedText();
            // Create a fresh block to insert into
            blockCursor.insertBlock();

            // After insertion, get the newly created block to compute its position
            QTextBlock newBlock = blockCursor.block();
            const QString lineText = it->first >= 0 ? newBlock.text() : QString(); // newBlock.text() will be empty until we insert
            Q_UNUSED(lineText);

            if (highlightWholeLine)
            {
                // Insert the whole line with whole-line background, then override matched parts with green
                QTextCharFormat wholeFmt;
                wholeFmt.setBackground(QColor(233,233,233)); // gray for whole line
                blockCursor.setCharFormat(wholeFmt);
                // Insert original text from the source block (we need the original text)
                const QString srcText = mDoc->findBlockByNumber(block.blockNumber()).text();
                blockCursor.insertText(srcText);

                // Now override matched areas with green using absolute positions on the document
                QTextBlock insertedBlock = blockCursor.block();
                QTextDocument* doc = const_cast<QTextDocument*>(insertedBlock.document());
                const int insertedBlockStart = insertedBlock.position();

                for (const auto& a : areas)
                {
                    if (a.end <= a.begin) continue;
                    int startPos = insertedBlockStart + a.begin;
                    int endPos   = insertedBlockStart + a.end;

                    // Clamp to block text length
                    const int maxEndPos = insertedBlockStart + insertedBlock.text().length();
                    if (endPos > maxEndPos) endPos = maxEndPos;
                    if (startPos < 0) startPos = 0;
                    if (endPos <= startPos) continue;

                    QTextCursor sel(doc);
                    sel.setPosition(startPos, QTextCursor::MoveAnchor);
                    sel.setPosition(endPos,   QTextCursor::KeepAnchor);
                    QTextCharFormat greenFmt;
                    greenFmt.setBackground(Qt::green);
                    sel.setCharFormat(greenFmt);
                }
            }
            else
            {
                // No whole-line highlight: insert segments with default/hl formats
                const QString srcText = mDoc->findBlockByNumber(block.blockNumber()).text();
                int pos = 0;
                for (const auto& a : areas)
                {
                    if (a.begin > pos)
                    {
                        QTextCharFormat defaultFmt;
                        blockCursor.setCharFormat(defaultFmt);
                        blockCursor.insertText(srcText.mid(pos, a.begin - pos));
                    }

                    if (a.end > a.begin)
                    {
                        QTextCharFormat hlFmt;
                        hlFmt.setBackground(Qt::yellow);
                        blockCursor.setCharFormat(hlFmt);
                        blockCursor.insertText(srcText.mid(a.begin, a.end - a.begin));
                    }
                    pos = a.end;
                }
                if (pos < srcText.length())
                {
                    QTextCharFormat defaultFmt;
                    blockCursor.setCharFormat(defaultFmt);
                    blockCursor.insertText(srcText.mid(pos));
                }
            }
        }

        block = nextBlock;
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
