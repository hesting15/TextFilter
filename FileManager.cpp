#include "FileManager.h"
#include <QFile>
#include <QTextStream>
#include <QException>
#include <QDebug>

QString FileManager::load(const QString &filename)
{
    if (filename.isEmpty())
    {
        return "";
    }

    QFile file(filename);
    file.open(QFile::ReadWrite | QFile::Text);
    QTextStream readFile(&file);
    return readFile.readAll();
}

void FileManager::save(const QString &filename, const QString &text)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error(file.errorString().toStdString());
    }

    file.write(text.toUtf8());
}
