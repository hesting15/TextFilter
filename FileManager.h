#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <QString>

namespace FileManager
{

QString load(const QString &filename);
void save(const QString &filename, const QString &text);

};

#endif // FILE_MANAGER_H
