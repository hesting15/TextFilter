#ifndef MEMO_H
#define MEMO_H

#include <QString>

struct FilteredLine
{
    int lineNumber;
    QString text;
    std::vector<std::pair<int, int>> highlightPositions;
};

class Memo
{
public:
    Memo();

    void setText(const QString& text) { mText = text; }
    QString getText() { return mText; }

    void loadFile(const QString &fileName);
    void saveFile(const QString &fileName);
    void filterText(const QString &filter, std::vector<FilteredLine>& filteredLines);

    bool isTextChanged(const QString& currentText);
private:
    QString mText;
    QString mOriginalText;
};

#endif // MEMO_H
