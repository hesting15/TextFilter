#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include "settingswindow.h"
#include "settings.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_pushButtonMenu_clicked(false);
    ui->frameInfo->setVisible(false);
    restoreGeometry(Settings::getInstance().getWindowGeometry());
    loadSettings();
    loadTextFromFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSettings()
{
    ui->plainTextEdit->setFont(Settings::getInstance().getFont());
    setAlwaysOnTop();
    setWordWrap();
    setRecentFiles();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Settings::getInstance().setWindowGeometry(saveGeometry());
    if (ui->plainTextEdit->isOriginalTextChanged())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Text Filter", "Save before quit?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            saveFile();
        }
        else if (reply == QMessageBox::Cancel)
        {
            event->ignore();
        }
    }
}

void MainWindow::on_lineEditSearch_textChanged(const QString &filter)
{
    // set cursor position to last filtered position
    int currentLine = 0;
    if (filter.isEmpty() && !ui->plainTextEdit->isInFilterMode())
    {
        currentLine = ui->plainTextEdit->getCurrentLineNumber(ui->plainTextEdit->textCursor());
    }

    const int filteredLinesCount = ui->plainTextEdit->applyFilter(filter);
    ui->toolButtonPrevious->setEnabled(filteredLinesCount>0);
    ui->toolButtonNext->setEnabled(filteredLinesCount>0);

    if (currentLine)
    {
        ui->plainTextEdit->setCurrentLineNumber(currentLine);
    }
}

void MainWindow::loadTextFromFile()
{
    ui->plainTextEdit->clearFilterAndLoadTextFromFile();
    ui->lineEditSearch->clear();
    ui->toolButtonPrevious->setEnabled(false);
    ui->toolButtonNext->setEnabled(false);
    QString filename = Settings::getInstance().getFilename();
    setWindowTitle(filename + (filename.isEmpty() ? "" : " - ") + "Text Filter");
}

void MainWindow::setAlwaysOnTop()
{
    Qt::WindowFlags flags = this->windowFlags();
    if (Settings::getInstance().isAlwaysOnTop())
    {
        flags = flags | Qt::WindowStaysOnTopHint;
    }
    else
    {
        flags = flags & ~Qt::WindowStaysOnTopHint;
    }
    this->setWindowFlags(flags);
    this->show();
    ui->toolButtonAlwaysOnTop->setChecked(Settings::getInstance().isAlwaysOnTop());
}

void MainWindow::setWordWrap()
{
    QPlainTextEdit::LineWrapMode mode = Settings::getInstance().isWordWrap() ? QPlainTextEdit::WidgetWidth
                                                                             : QPlainTextEdit::NoWrap;
    ui->plainTextEdit->setLineWrapMode(mode);
    ui->toolButtonWordWrap->setChecked(Settings::getInstance().isWordWrap());
}

void MainWindow::loadFile(const QString &filename)
{
    if (filename.isEmpty())
    {
        return;
    }

    if (ui->plainTextEdit->isOriginalTextChanged())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Text Filter", "Save changed file?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            saveFile();
        }
        else if (reply == QMessageBox::Cancel)
        {
            return;
        }
    }

    ui->lineEditSearch->clear();
    ui->plainTextEdit->loadFile(filename);
    Settings::getInstance().setFilename(filename);
    Settings::getInstance().addRecentFile(filename);
    setRecentFiles();
    setWindowTitle(filename + " - Text Filter");
}

void MainWindow::saveFile()
{
    if (ui->plainTextEdit->isFiltering())
    {
        QMessageBox::warning(this, tr("Text Filter"),
                             tr("Cannot save file while text is filtered"),
                             QMessageBox::Close);
        return;
    }

    const QString filename = Settings::getInstance().getFilename();
    if (filename.isEmpty())
    {
        on_toolButtonSaveFileAs_clicked();
        return;
    }

    ui->plainTextEdit->saveFile(filename);
    ui->toolButtonSaveFile->setIcon(QIcon(":/resources/icon_save.png"));

    on_pushButtonMenu_clicked(false);
    ui->frameInfo->setVisible(true);
    QTimer::singleShot(2000, this, SLOT(hideFrameInfo()));

}

void MainWindow::hideFrameInfo()
{
    ui->frameInfo->setVisible(false);
}

void MainWindow::on_lineEditSearch_returnPressed()
{
    on_toolButtonNext_clicked();
}

void MainWindow::on_toolButtonPrevious_clicked()
{
    ui->plainTextEdit->gotoPreviousFilteredLine();
}

void MainWindow::on_toolButtonNext_clicked()
{
    ui->plainTextEdit->gotoNextFilteredLine();
}

void MainWindow::on_toolButtonOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"), "",
                                                    tr("Text File (*.txt);;All Files (*)"));
    loadFile(fileName);
}

void MainWindow::on_toolButtonSaveFile_clicked()
{
    saveFile();
}

void MainWindow::on_toolButtonSaveFileAs_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"), "",
                                                    tr("Text File (*.txt);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        Settings::getInstance().setFilename(fileName);
        saveFile();
    }
}

void MainWindow::on_toolButtonFilterText_clicked()
{
    ui->lineEditSearch->clear();
    ui->lineEditSearch->setFocus();
}

void MainWindow::on_toolButtonCopyLine_clicked()
{
    QTextCursor cursor(ui->plainTextEdit->textCursor());
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    ui->plainTextEdit->setTextCursor(cursor);
    ui->plainTextEdit->copy();
}

void MainWindow::on_toolButtonCopyMultipleLines_clicked()
{
    QTextCursor cursor = ui->plainTextEdit->textCursor();
    if (cursor.hasSelection())
    {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();

        // Check if selection begins from beginning of line
        cursor.setPosition(start);
        cursor.movePosition(QTextCursor::StartOfLine);
        if (start == cursor.position())
        {
            // Continue selecting text
            cursor.setPosition(end, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            ui->plainTextEdit->setTextCursor(cursor);
            ui->plainTextEdit->copy();
            return;
        }
        else
        {
            // If selections begins in the middle of the line
            // ignore selection and copy current line
            cursor.setPosition(end);
        }
    }

    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    ui->plainTextEdit->setTextCursor(cursor);
    ui->plainTextEdit->copy();
}

void MainWindow::on_toolButtonAlwaysOnTop_clicked()
{
    Settings::getInstance().setAlwaysOnTop(!Settings::getInstance().isAlwaysOnTop());
    setAlwaysOnTop();
}

void MainWindow::on_toolButtonSettings_clicked()
{
    SettingsWindow *settingsWindow = new SettingsWindow();
    settingsWindow->setModal(true);

    connect(settingsWindow, &SettingsWindow::applySettings,
            this, &MainWindow::loadSettings);

    Qt::WindowFlags flags = settingsWindow->windowFlags();
    flags = flags & ~Qt::WindowContextHelpButtonHint; // Hide help ? button
    if (Settings::getInstance().isAlwaysOnTop())
    {
        flags = flags | Qt::WindowStaysOnTopHint;
    }
    else
    {
        flags = flags & ~Qt::WindowStaysOnTopHint;
    }
    settingsWindow->setWindowFlags(flags);
    settingsWindow->exec();
}

void MainWindow::on_toolButtonHelp_clicked()
{
    QMessageBox::information(this, tr("Text Filter"),
                             tr("Text Filter v1.4\n\n"
                                " * Use fuzzy match to filter text (e.g. 'ore psu' will find 'Lorem Ipsum').\n"
                                " * Press Enter in the Filter field to go to the next filter result.\n"
                                " * Use Ctrl + Left Mouse Click on the line, to copy whole line to clipboard.\n"
                                " * Press Alt + C several times to extend selection and copy multiple lines to clipboard.\n"
                                "\n"
                                "Written by Stepan Sypliak using Qt Creator."),
                             QMessageBox::Ok);
}

void MainWindow::on_toolButtonWordWrap_clicked()
{
    Settings::getInstance().setWordWrap(!Settings::getInstance().isWordWrap());
    setWordWrap();
}

void MainWindow::on_pushButtonMenu_clicked(bool checked)
{
    // Shortcuts does not work when control is not visible
    //ui->widgetMenu->setVisible(!checked);
    ui->widgetMenu->setMinimumSize(0, checked ? 40 : 0);
    ui->widgetMenu->setMaximumSize(16777215, checked ? 40 : 0);
    ui->pushButtonMenu->setChecked(checked);
}

void MainWindow::on_toolButtonNewFile_clicked()
{
    ui->plainTextEdit->applyFilter("");
    ui->plainTextEdit->clear();
    ui->lineEditSearch->clear();
}

void MainWindow::on_plainTextEdit_textChanged()
{
    if(ui->plainTextEdit->isOriginalTextChanged())
    {
        ui->toolButtonSaveFile->setIcon(QIcon(":/resources/icon_save_changed.png"));
    }
}

void MainWindow::setRecentFiles()
{
    for (auto* action : ui->toolButtonOpenFile->actions())
    {
        ui->toolButtonOpenFile->removeAction(action);
        delete action;
    }

    QStringList recentFiles = Settings::getInstance().getRecentFiles();
    for (const QString& filename : recentFiles)
    {
        QAction* recentFile = new QAction(filename, this);
        QObject::connect(recentFile, SIGNAL(triggered()), this, SLOT(openRecent()));
        ui->toolButtonOpenFile->addAction(recentFile);
    }
}

void MainWindow::openRecent()
{
     QAction* action = qobject_cast<QAction *>(sender());
     loadFile(action->text());
}
