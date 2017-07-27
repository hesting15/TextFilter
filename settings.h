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

    void setFilename(const QString &filename);
    void setFont(const QFont &font);
    void setWindowGeometry(const QByteArray &geometry);
    void setAlwaysOnTop(bool alwaysOnTop);
    void setFilterThreshold(const int filterThreshold);

private:
    Settings();

    void saveSettings();

    QString    mIniPath;
    QString    mFilename;
    QFont      mFont;
    QByteArray mGeometry;
    bool       mAlwaysOnTop;
    int        mFilterThreshold;
};

#endif // SETTINGS_H
