#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QRect>
#include <QFont>

class Settings
{
public:
    static Settings &getInstance()
    {
        static Settings instance;
        return instance;
    }

    Settings(Settings const&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(Settings const&) = delete;
    Settings& operator=(Settings &&) = delete;

    QString getFilename()          { return mFilename;        }
    QFont getFont()                { return mFont;            }
    QByteArray getWindowGeometry() { return mGeometry;        }
    bool isAlwaysOnTop()           { return mAlwaysOnTop;     }
    int getFilterThreshold()       { return mFilterThreshold; }
    bool isWordWrap()              { return mWordWrap;        }
    QStringList getRecentFiles()   { return mRecentFiles;     }

    void setFilename(const QString &filename);
    void setFont(const QFont &font);
    void setWindowGeometry(const QByteArray &geometry);
    void setAlwaysOnTop(const bool alwaysOnTop);
    void setFilterThreshold(const int filterThreshold);
    void setWordWrap(const bool wordWrap);
    void addRecentFile(const QString &filename);

private:
    Settings();

    void saveSettings();

    QString     mIniPath;
    QString     mFilename;
    QFont       mFont;
    QByteArray  mGeometry;
    bool        mAlwaysOnTop;
    int         mFilterThreshold;
    bool        mWordWrap;
    QStringList mRecentFiles;
};

#endif // SETTINGS_H
