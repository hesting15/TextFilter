#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QRect>
#include <QFont>
#include <QList>
#include <QObject>
#include <QTimer>

// Settings is a QObject so it can own a QTimer for debounced disk writes.
class Settings : public QObject
{
    Q_OBJECT

public:
    static Settings &getInstance()
    {
        static Settings instance;
        return instance;
    }

    Settings(Settings const&)            = delete;
    Settings(Settings&&)                 = delete;
    Settings& operator=(Settings const&) = delete;
    Settings& operator=(Settings&&)      = delete;

    // ── Getters ──────────────────────────────────────────────────────────────
    QString              getFilename()       const { return mFilename;        }
    QFont                getFont()           const { return mFont;            }
    QByteArray           getWindowGeometry() const { return mGeometry;        }
    bool                 isAlwaysOnTop()     const { return mAlwaysOnTop;     }
    int                  getFilterThreshold()const { return mFilterThreshold; }
    bool                 isWordWrap()        const { return mWordWrap;        }
    QStringList          getRecentFiles()    const { return mRecentFiles;     }
    QFont::StyleStrategy getStyleStrategy()  const { return mStyleStrategy;   }

    // ── Setters (schedule a debounced disk write; never block the UI) ────────
    void setFilename(const QString &filename);
    void setFont(const QFont &font);
    void setWindowGeometry(const QByteArray &geometry);
    void setAlwaysOnTop(bool alwaysOnTop);
    void setFilterThreshold(int filterThreshold);
    void setWordWrap(bool wordWrap);
    void setStyleStrategy(QFont::StyleStrategy strategy);
    void addRecentFile(const QString &filename);

    // Force an immediate flush (called on app close).
    void flushNow();

private slots:
    // Connected to mSaveTimer::timeout — actually writes to disk.
    void flushToDisk();

private:
    explicit Settings();

    // Schedule a debounced write: resets the timer so rapid successive calls
    // (e.g. Ctrl+wheel zoom) collapse into a single disk write.
    void scheduleSave();

    QString              mIniPath;
    QString              mFilename;
    QFont                mFont;
    QByteArray           mGeometry;
    bool                 mAlwaysOnTop;
    int                  mFilterThreshold;
    bool                 mWordWrap;
    QFont::StyleStrategy mStyleStrategy;
    QStringList          mRecentFiles;

    // Debounce timer: fires once after the last setter call settles.
    QTimer               mSaveTimer;

    static constexpr int kSaveDelayMs = 400;
};

#endif // SETTINGS_H