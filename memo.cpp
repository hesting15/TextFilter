#include "memo.h"
#include <QFile>
#include <QTextStream>
#include <QException>
#include <QDebug>
#include <settings.h>

Memo::Memo()
{
    loadFile(Settings::getInstance().getFilename());
}

void Memo::loadFile(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        mText = "";
        return;
    }

    QFile file(fileName);
    file.open(QFile::ReadWrite | QFile::Text);
    QTextStream readFile(&file);
    mText = readFile.readAll();
    mOriginalText = mText;
}

void Memo::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error(file.errorString().toStdString());
    }

    file.write(mText.toUtf8());
    mOriginalText = mText;
}

void Memo::filterText(const QString &filter, std::vector<FilteredLine> &filteredLines)
{
    filteredLines.clear();

    QStringList filterItems = filter.split(" ");
    QStringList lines = mText.split(QRegExp("[\r\n]"), QString::KeepEmptyParts);
    int lineNumber = 0;

    for (QString& line : lines)
    {
        bool shouldHighlight = true;
        std::vector<std::pair<int, int>> highlightPositions;

        int previousIndex = 0;

        for (const QString& filterItem : filterItems)
        {
            int indexOf = line.indexOf(filterItem, previousIndex, Qt::CaseInsensitive);
            if (indexOf == -1)
            {
                shouldHighlight = false;
                break;
            }
            highlightPositions.push_back({indexOf, indexOf + filterItem.length()});
            previousIndex = indexOf + filterItem.length();
        }

        if (shouldHighlight)
        {
            filteredLines.push_back(FilteredLine{lineNumber, line, highlightPositions});
        }
        ++lineNumber;
    }
}

bool Memo::isTextChanged(const QString& currentText)
{
    return currentText != mOriginalText;
}
