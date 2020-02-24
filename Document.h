#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QTextDocument>
#include <memory>

class Document
{
public:

    Document(QTextDocument* document);

    void applyFilter(const QString& filter);

    // Get original document
    std::shared_ptr<QTextDocument> getDocument() { return mDoc; }

    // Get filtered document
    // Matched text will be highlighted
    std::shared_ptr<QTextDocument> getFilteredDocument();

    // Get full document
    // Matched text will be highlighted
    // Current line will be highlighted as well
    std::shared_ptr<QTextDocument> getFullDocumentWithPrevLineHighlighted();
    std::shared_ptr<QTextDocument> getFullDocumentWithNextLineHighlighted();

    QString getFilter() const { return mFilter; }

    int getCurrentHighligtedLineNum() const { return mCurrentHighlightedLine; }
    int getFilteredLineCount() const { return mHighlightAreas.size(); }

    // Workaround. Cloning QTextDocument does not clone undo history
    // So instead of replacing QTextDocument, I will replace text
    // and keep track at which undo history point text was inserted
    int getUndoHistoryPoint() { return mUndoHistoryPoint; }

private:

    // Area to highliht in the line
    // Begin and end represents values since line start
    struct HighlightArea
    {
        HighlightArea(int begin, int end)
            : begin(begin)
            , end(end)
        {
        }

        int begin;
        int end;
    };

    // Search for lines with fuzzy filter match
    // Vector highlightPositions contains individual positions
    // of each character
    // If line matches, we will squash individual positions
    // into sequences
    struct MatchResult
    {
        MatchResult()
            : result(false)
        {
        }

        bool result;
        std::vector<HighlightArea> highlightAreas;
    };

    MatchResult isLineMatchesFiter(const QString& line, const QString& filter);


    void highlightMatchedText(
        QTextBlock& block,
        const std::vector<HighlightArea>& highlightAreas,
        bool highlightWholeLine);

    void removeLine(const QTextBlock& block);

    std::shared_ptr<QTextDocument> cloneDocument();
    std::shared_ptr<QTextDocument> getFullDocumentWithHighlightedLine();

private:
    std::shared_ptr<QTextDocument> mDoc;
    QString mFilter;

    // To iterate over highighted lines we need to
    // remember current line
    int mCurrentHighlightedLine;

    // This map represents LineNumber and all highlihting there
    std::map<int, std::vector<HighlightArea>> mHighlightAreas;

    int mUndoHistoryPoint;
};

#endif // DOCUMENT_H
