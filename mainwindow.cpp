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

void MainWindow::createMenuActions()
{
    QAction *filter      = new QAction("Filter Text");
    QAction *copyLine    = new QAction("Copy Line               Ctrl+Left Mouse");
    QAction *copyLines   = new QAction("Copy Multiple Lines");
    QAction *settings    = new QAction("Settings...");
    QAction *alwaysOnTop = new QAction("Always On Top");
    QAction *loadFile    = new QAction("Load File...");
    QAction *saveFile    = new QAction("Save File");
    QAction *saveFileAs  = new QAction("Save File As...");
    QAction *help        = new QAction("Help");

    QAction *separator1  = new QAction(this);
    QAction *separator2  = new QAction(this);
    QAction *separator3  = new QAction(this);

    filter->setShortcut  (Qt::CTRL | Qt::Key_F);
    loadFile->setShortcut(Qt::CTRL | Qt::Key_O);
    saveFile->setShortcut(Qt::CTRL | Qt::Key_S);
    copyLines->setShortcut(Qt::ALT | Qt::Key_C);
    help->setShortcut    (Qt::Key_F1);

    alwaysOnTop->setCheckable(true);
    alwaysOnTop->setChecked(Settings::getInstance().isAlwaysOnTop());
    separator1->setSeparator(true);
    separator2->setSeparator(true);
    separator3->setSeparator(true);

    connect(filter,      SIGNAL(triggered()), this, SLOT(filterTextAction()));
    connect(copyLine,    SIGNAL(triggered()), this, SLOT(copyLineAction()));
    connect(copyLines,   SIGNAL(triggered()), this, SLOT(copyMultipleLinesAction()));
    connect(settings,    SIGNAL(triggered()), this, SLOT(settingsAction()));
    connect(alwaysOnTop, SIGNAL(triggered()), this, SLOT(alwaysOnTopAction()));
    connect(loadFile,    SIGNAL(triggered()), this, SLOT(loadFileAction()));
    connect(saveFile,    SIGNAL(triggered()), this, SLOT(saveFileAction()));
    connect(saveFileAs,  SIGNAL(triggered()), this, SLOT(saveFileAsAction()));
    connect(help,        SIGNAL(triggered()), this, SLOT(helpAction()));

    QMenu *menu=new QMenu(this);

    menu->addActions({filter,
                      copyLine,
                      copyLines,
                      separator1,
                      loadFile,
                      saveFile,
                      saveFileAs,
                      separator3,
                      alwaysOnTop,
                      settings,
                      separator2,
                      help});

    ui->pushButtonMenu->setMenu(menu);
}

void MainWindow::filterTextAction()
{
    ui->lineEditSearch->clear();
    ui->lineEditSearch->setFocus();
}

void MainWindow::copyLineAction()
{
    QTextCursor cursor(ui->plainTextEdit->textCursor());
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.select(QTextCursor::LineUnderCursor);
    ui->plainTextEdit->setTextCursor(cursor);
    ui->plainTextEdit->copy();
}

void MainWindow::copyMultipleLinesAction()
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

void MainWindow::alwaysOnTopAction()
{
    Settings::getInstance().setAlwaysOnTop(!Settings::getInstance().isAlwaysOnTop());
    setAlwaysOnTop();
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
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate)
    {
        // When window is set to "Always On Top", in some cases MainWindows
        // stays above QMenu. To fix it, I recreate a menu
        createMenuActions();
    }
    return QWidget::event(event);
}

void MainWindow::loadFileAction()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"), "",
                                                    tr("Text File (*.txt);;All Files (*)"));
    loadFile(fileName);
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
        saveFileAsAction();
        return;
    }

    ui->plainTextEdit->saveFile(filename);
}

void MainWindow::saveFileAction()
{
    saveFile();
}

void MainWindow::saveFileAsAction()
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

void MainWindow::helpAction()
{
    QMessageBox::information(this, tr("Text Filter"),
                             tr("Text Filter v1.2\n\n"
                                " * Use fuzzy match to filter text (e.g. 'ore psu' will find 'Lorem Ipsum').\n"
                                " * Press Enter in the Filter field to go to the next filter result.\n"
                                " * Use Ctrl + Left Mouse Click on the line, to copy whole line to clipboard.\n"
                                " * Press Alt + C several times to extend selection and copy multiple lines to clipboard.\n"
                                "\n"
                                "Written by Stepan Sypliak using Qt Creator."),
                             QMessageBox::Ok);
}

void MainWindow::settingsAction()
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
