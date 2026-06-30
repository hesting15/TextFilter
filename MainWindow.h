#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Document.h"

#include <QIcon>
#include <QMainWindow>
#include <QtWidgets/QAbstractButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
private slots:
    void on_lineEditSearch_textChanged(const QString &text);
    void on_toolButtonPrevious_clicked();
    void on_toolButtonNext_clicked();
    void on_toolButtonOpenFile_clicked();
    void on_toolButtonSaveFile_clicked();
    void on_toolButtonSaveFileAs_clicked();
    void clearFilter();
    void on_toolButtonCopyLine_clicked();
    void on_toolButtonCopyMultipleLines_clicked();
    void on_toolButtonAlwaysOnTop_clicked();
    void on_toolButtonSettings_clicked();
    void on_toolButtonHelp_clicked();
    void on_toolButtonWordWrap_clicked();
    void on_toolButtonNewFile_clicked();
    void on_pushButtonMenu_clicked(bool checked);

    void hideFrameInfo();
    void openRecent();
    void restoreScrollPosition();
    void applyRestoredScrollPosition();

    void on_plainTextEdit_textChanged();

private:
    void applySettings();
    void createMenuActions();
    void saveFile(const QString& filename);
    void loadFile(const QString& fileName);
    void setAlwaysOnTop();
    void setWordWrap();
    void loadLastFile();

    Ui::MainWindow *ui;
    void setRecentFiles();
    void updateSaveAndMenuButtonIcons();
    void updateFilename(const QString& filename);
    void setIconMultipleResolutions(
        QAbstractButton *button,
        const QString &iconName);

    void undoToHistoryPoint(int historyPoint);

    std::shared_ptr<Document> rootDocument;
    int mPreFilterTopBlock;

    // Icon cache: loaded once (see MainWindow ctor) instead of constructing
    // a new QIcon from the resource path on every call to
    // updateSaveAndMenuButtonIcons (which previously fired on every
    // keystroke via on_plainTextEdit_textChanged).
    QIcon mIconMenuClean;
    QIcon mIconMenuChanged;
    QIcon mIconSaveClean;
    QIcon mIconSaveChanged;

    // Tracks whether the icons are currently showing the "dirty" state.
    // updateSaveAndMenuButtonIcons() is a no-op when this already matches
    // the editor's actual dirty state, so setIcon() is only called on the
    // keystroke where the dirty flag actually flips, not on every keystroke.
    // Starts as an impossible/uninitialized-marker value so the very first
    // call always performs the initial icon assignment.
    int mLastDirtyState = -1; // -1 = not yet set, 0 = clean, 1 = dirty
};

#endif // MAINWINDOW_H