#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QRect>
#include <QFont>

class Settings
{
public:
    Settings();

    QString getFilename()          { return mFilename;    }
    QFont getFont()                { return mFont;        }
    QByteArray getWindowGeometry() { return mGeometry;    }
    bool  getAlwaysOnTop()         { return mAlwaysOnTop; }

    void setFilename(const QString &filename);
    void setFont(const QFont &font);
    void setWindowGeometry(const QByteArray &geometry);
    void setAlwaysOnTop(bool alwaysOnTop);

private:
    void saveSettings();

    QString    mIniPath;
    QString    mFilename;
    QFont      mFont;
    QByteArray mGeometry;
    bool       mAlwaysOnTop;
};

#endif // SETTINGS_H
