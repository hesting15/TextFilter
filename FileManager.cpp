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
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        qWarning() << "FileManager::load: failed to open" << filename << file.errorString();
        return QString();
    }

    QTextStream readFile(&file);
    QString content = readFile.readAll();
    file.close();
    return content;
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
