#include "Settings.h"
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QDesktopWidget>
#include <QStandardPaths>

const static QString cFilename        = "FILE_NAME";
const static QString cFontName        = "FONT_NAME";
const static QString cFontSize        = "FONT_SIZE";
const static QString cFontBold        = "FONT_BOLD";
const static QString cGeometry        = "GEOMETRY";
const static QString cAlwaysOnTop     = "ALWAYS_ON_TOP";
const static QString cFilterThreshold = "FILTER_THRESHOLD";
const static QString cWordWrap        = "WORD_WRAP";
const static QString cRecentFiles     = "RECENT_FILES";

Settings::Settings()
{
    mIniPath =
        QStandardPaths::writableLocation(
            QStandardPaths::HomeLocation) + "/.TextFilter.ini";

    QSettings settings(mIniPath, QSettings::IniFormat);
    mFilename = settings.value(cFilename, "").toString();
    mGeometry = settings.value(cGeometry, QByteArray{}).toByteArray();
    mAlwaysOnTop = settings.value(cAlwaysOnTop, true).toBool();
    mFilterThreshold = settings.value(cFilterThreshold, 1).toInt();
    mWordWrap = settings.value(cWordWrap, false).toBool();

    const QString fontName = settings.value(cFontName, "Courier New").toString();
    const int fontSize     = settings.value(cFontSize, 11).toInt();
    const bool fontBold    = settings.value(cFontBold, false).toBool();

    mFont = QFont(fontName, fontSize);
    mFont.setBold(fontBold);

    mRecentFiles = settings.value(cRecentFiles).toStringList();
}

void Settings::saveSettings()
{
    QSettings settings(mIniPath, QSettings::IniFormat);
    settings.setValue(cFilename, mFilename);
    settings.setValue(cFontName, mFont.family());
    settings.setValue(cFontSize, mFont.pointSize());
    settings.setValue(cFontBold, mFont.bold());
    settings.setValue(cGeometry, mGeometry);
    settings.setValue(cAlwaysOnTop, mAlwaysOnTop);
    settings.setValue(cFilterThreshold, mFilterThreshold);
    settings.setValue(cWordWrap, mWordWrap);
    settings.setValue(cRecentFiles, mRecentFiles);
}

void Settings::setFilename(const QString &filename)
{
    mFilename = filename;
    saveSettings();
}

void Settings::setFont(const QFont &font)
{
    mFont = font;
    saveSettings();
}

void Settings::setWindowGeometry(const QByteArray &geometry)
{
    mGeometry = geometry;
    saveSettings();
}

void Settings::setAlwaysOnTop(const bool alwaysOnTop)
{
    mAlwaysOnTop = alwaysOnTop;
    saveSettings();
}

void Settings::setFilterThreshold(const int filterThreshold)
{
    mFilterThreshold = filterThreshold;
    saveSettings();
}

void Settings::setWordWrap(const bool wordWrap)
{
    mWordWrap = wordWrap;
    saveSettings();
}

void Settings::addRecentFile(const QString& filename)
{
    for (int i = mRecentFiles.count() - 1; i >= 0; --i)
    {
        if (mRecentFiles[i] == filename || i > 8)
        {
            mRecentFiles.removeAt(i);
        }
    }
    mRecentFiles.push_front(filename);
    saveSettings();
}
