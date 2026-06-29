#include "Settings.h"
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>

// ── INI key constants ────────────────────────────────────────────────────────
static const QString cFilename        = QStringLiteral("FILE_NAME");
static const QString cFontName        = QStringLiteral("FONT_NAME");
static const QString cFontSize        = QStringLiteral("FONT_SIZE");
static const QString cFontBold        = QStringLiteral("FONT_BOLD");
static const QString cGeometry        = QStringLiteral("GEOMETRY");
static const QString cAlwaysOnTop     = QStringLiteral("ALWAYS_ON_TOP");
static const QString cFilterThreshold = QStringLiteral("FILTER_THRESHOLD");
static const QString cWordWrap        = QStringLiteral("WORD_WRAP");
static const QString cRecentFiles     = QStringLiteral("RECENT_FILES");
static const QString cStyleStrategy   = QStringLiteral("STYLE_STRATEGY");

// ── Construction / loading ───────────────────────────────────────────────────
Settings::Settings()
    : QObject(nullptr)
{
    mIniPath =
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
        + QStringLiteral("/.TextFilter.ini");

    QSettings settings(mIniPath, QSettings::IniFormat);
    mFilename        = settings.value(cFilename, QString{}).toString();
    mGeometry        = settings.value(cGeometry, QByteArray{}).toByteArray();
    mAlwaysOnTop     = settings.value(cAlwaysOnTop, true).toBool();
    mFilterThreshold = settings.value(cFilterThreshold, 1).toInt();
    mWordWrap        = settings.value(cWordWrap, false).toBool();
    mStyleStrategy   = static_cast<QFont::StyleStrategy>(
        settings.value(cStyleStrategy, QFont::PreferDefault).toInt());

    const QString fontName = settings.value(cFontName, "Courier New").toString();
    const int     fontSize = settings.value(cFontSize, 11).toInt();
    const bool    fontBold = settings.value(cFontBold, false).toBool();

    mFont = QFont(fontName, fontSize);
    mFont.setBold(fontBold);
    mFont.setStyleStrategy(mStyleStrategy);

    mRecentFiles = settings.value(cRecentFiles).toStringList();

    // Debounce timer setup — single-shot so it fires exactly once after the
    // last scheduleSave() call within the kSaveDelayMs window.
    mSaveTimer.setSingleShot(true);
    connect(&mSaveTimer, &QTimer::timeout, this, &Settings::flushToDisk);
}

// ── Debounce helpers ─────────────────────────────────────────────────────────

// Called by every setter; resets the countdown each time so a burst of calls
// (e.g. Ctrl+wheel producing dozens of setFont() calls per second) results in
// a single disk write once the user stops.
void Settings::scheduleSave()
{
    mSaveTimer.start(kSaveDelayMs);
}

// Slot — wired to mSaveTimer::timeout.
void Settings::flushToDisk()
{
    QSettings settings(mIniPath, QSettings::IniFormat);
    settings.setValue(cFilename,        mFilename);
    settings.setValue(cFontName,        mFont.family());
    settings.setValue(cFontSize,        mFont.pointSize());
    settings.setValue(cFontBold,        mFont.bold());
    settings.setValue(cGeometry,        mGeometry);
    settings.setValue(cAlwaysOnTop,     mAlwaysOnTop);
    settings.setValue(cFilterThreshold, mFilterThreshold);
    settings.setValue(cWordWrap,        mWordWrap);
    settings.setValue(cStyleStrategy,   static_cast<int>(mStyleStrategy));
    settings.setValue(cRecentFiles,     mRecentFiles);
}

// Called from MainWindow::closeEvent to guarantee the final state is written
// before the process exits, even if the debounce timer has not fired yet.
void Settings::flushNow()
{
    mSaveTimer.stop();
    flushToDisk();
}

// ── Setters ──────────────────────────────────────────────────────────────────

void Settings::setFilename(const QString &filename)
{
    mFilename = filename;
    scheduleSave();
}

void Settings::setFont(const QFont &font)
{
    mFont = font;
    scheduleSave();
}

void Settings::setWindowGeometry(const QByteArray &geometry)
{
    mGeometry = geometry;
    scheduleSave();
}

void Settings::setAlwaysOnTop(bool alwaysOnTop)
{
    mAlwaysOnTop = alwaysOnTop;
    scheduleSave();
}

void Settings::setFilterThreshold(int filterThreshold)
{
    mFilterThreshold = filterThreshold;
    scheduleSave();
}

void Settings::setWordWrap(bool wordWrap)
{
    mWordWrap = wordWrap;
    scheduleSave();
}

void Settings::setStyleStrategy(QFont::StyleStrategy strategy)
{
    mStyleStrategy = strategy;
    mFont.setStyleStrategy(strategy);
    scheduleSave();
}

void Settings::addRecentFile(const QString &filename)
{
    // Remove any existing entry for this file and prune list to 9 items.
    for (int i = mRecentFiles.count() - 1; i >= 0; --i)
    {
        if (mRecentFiles[i] == filename || i > 8)
        {
            mRecentFiles.removeAt(i);
        }
    }
    mRecentFiles.push_front(filename);
    scheduleSave();
}