#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include "settingswindow.h"
#include "settings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widgetMenu->setMinimumSize(0, 0);
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
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Settings::getInstance().setWindowGeometry(saveGeometry());
    QMainWindow::closeEvent(event);
}

void MainWindow::on_lineEditSearch_textChanged(const QString &filter)
{
    const int filteredLinesCount = ui->plainTextEdit->applyFilter(filter);
    ui->toolButtonPrevious->setEnabled(filteredLinesCount>0);
    ui->toolButtonNext->setEnabled(filteredLinesCount>0);
}

void MainWindow::loadTextFromFile()
{
    ui->plainTextEdit->clearFilterAndLoadTextFromFile();
    ui->lineEditSearch->clear();
    ui->toolButtonPrevious->setEnabled(false);
    ui->toolButtonNext->setEnabled(false);
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

    ui->lineEditSearch->clear();
    ui->plainTextEdit->loadFile(filename);
    Settings::getInstance().setFilename(filename);
}

void MainWindow::saveFile()
{
    const QString filename = Settings::getInstance().getFilename();
    if (filename.isEmpty())
    {
        on_toolButtonSaveFileAs_clicked();
        return;
    }

    ui->plainTextEdit->saveFile(filename);
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
    // ui->widgetMenu->setVisible(!ui->widgetMenu->isVisible());
    ui->widgetMenu->setMinimumSize(0, checked ? 40 : 0);
}
