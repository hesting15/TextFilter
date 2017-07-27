#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>
#include "settingswindow.h"
#include "settings.h"

static const int NO_CURRENT_FILTERED_LINE_NUMBER = -1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mIsFiltering(false)
{
    ui->setupUi(this);
    restoreGeometry(Settings::getInstance().getWindowGeometry());
    loadSettings();
    setPlainTextFromFile();
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
    if (!mIsFiltering)
    {
        if (filter.length() < Settings::getInstance().getFilterThreshold())
        {
            return;
        }
        mIsFiltering = true;
        mMemo.setText(ui->plainTextEdit->toPlainText());
    }
    else if (filter.isEmpty())
    {        
        setPlainTextFromFile();
        return;
    }

    mCurrentFilteredLineNumber = NO_CURRENT_FILTERED_LINE_NUMBER;
    ui->plainTextEdit->setPlainText("");

    mMemo.filterText(filter, mFilteredLines);

    for (const FilteredLine& filteredLine : mFilteredLines)
    {
        ui->plainTextEdit->appendPlainText(filteredLine.text);
        higlightTextInLine(ui->plainTextEdit->document()->lineCount() - 1, false, filteredLine.highlightPositions);
    }

    ui->toolButtonPrevious->setEnabled(mFilteredLines.size()>0);
    ui->toolButtonNext->setEnabled(mFilteredLines.size()>0);
}

void MainWindow::setPlainTextFromFile()
{
    mIsFiltering = false;
    ui->lineEditSearch->clear();
    ui->toolButtonPrevious->setEnabled(false);
    ui->toolButtonNext->setEnabled(false);

    QTextCursor cursor(ui->plainTextEdit->document());
    QTextCharFormat fmt;
    fmt.setBackground(getDefaultBackgroungColor());
    cursor.setCharFormat(fmt);
    ui->plainTextEdit->setTextCursor(cursor);
    ui->plainTextEdit->setPlainText(mMemo.getText());
}

void MainWindow::createMenuActions()
{
    QAction *filter      = new QAction("Filter Text");
    QAction *copyLine    = new QAction("Copy Line               Ctrl+Left Mouse");
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
    help->setShortcut    (Qt::Key_F1);

    alwaysOnTop->setCheckable(true);
    alwaysOnTop->setChecked(Settings::getInstance().isAlwaysOnTop());
    separator1->setSeparator(true);
    separator2->setSeparator(true);
    separator3->setSeparator(true);

    connect(filter,      SIGNAL(triggered()), this, SLOT(filterTextAction()));
    connect(copyLine,    SIGNAL(triggered()), this, SLOT(copyLineAction()));
    connect(settings,    SIGNAL(triggered()), this, SLOT(settingsAction()));
    connect(alwaysOnTop, SIGNAL(triggered()), this, SLOT(alwaysOnTopAction()));
    connect(loadFile,    SIGNAL(triggered()), this, SLOT(loadFileAction()));
    connect(saveFile,    SIGNAL(triggered()), this, SLOT(saveFileAction()));
    connect(saveFileAs,  SIGNAL(triggered()), this, SLOT(saveFileAsAction()));
    connect(help,        SIGNAL(triggered()), this, SLOT(helpAction()));

    QMenu *menu=new QMenu(this);

    menu->addActions({filter,
                      copyLine,
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

    mMemo.loadFile(filename);
    setPlainTextFromFile();
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

    mMemo.setText(ui->plainTextEdit->toPlainText());

    try
    {
        mMemo.saveFile(filename);
    }
    catch(const std::runtime_error& ex)
    {
        QMessageBox::information(this, tr("Unable to save file"), ex.what());
    }
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
                             tr("Text Filter v1.1\n\n"
                                " * Use fuzzy match to filter text (e.g. 'ore psu' will find 'Lorem Ipsum').\n"
                                " * Press Enter in the Filter field to go to the next filter result.\n"
                                " * Use Ctrl + Left Mouse Click on the line, to copy whole line to clipboard.\n\n"
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

QBrush MainWindow::getDefaultBackgroungColor()
{
    return ui->plainTextEdit->palette().brush(QPalette::ColorRole::Base);
}

void MainWindow::higlightTextInLine(const int lineNumber, const bool highlight, std::vector<std::pair<int, int>> highlightPositions)
{
    QTextCursor cursor(ui->plainTextEdit->document()->findBlockByLineNumber(lineNumber));
    int beginningOfLine = cursor.position();

    // highlight whole line
    QTextCharFormat fmt;
    fmt.setBackground(highlight ? QColor(233,233,233) : getDefaultBackgroungColor());
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    cursor.setCharFormat(fmt);

    // higlight filtered text
    fmt.setBackground(highlight ? Qt::green : Qt::yellow);

    for (const auto& highlightPosition : highlightPositions)
    {
        cursor.setPosition(beginningOfLine + highlightPosition.first,  QTextCursor::MoveAnchor);
        cursor.setPosition(beginningOfLine + highlightPosition.second, QTextCursor::KeepAnchor);
        cursor.setCharFormat(fmt);
    }
}

void MainWindow::on_toolButtonPrevious_clicked()
{
    if (mIsFiltering && mFilteredLines.size() > 0)
    {
        if (mCurrentFilteredLineNumber == NO_CURRENT_FILTERED_LINE_NUMBER)
        {
            mCurrentFilteredLineNumber = mFilteredLines.size() - 1;
            ui->plainTextEdit->setPlainText(mMemo.getText());
            for (const FilteredLine& filteredLine : mFilteredLines)
            {
                higlightTextInLine(filteredLine.lineNumber, false, filteredLine.highlightPositions);
            }
        }
        else
        {
            FilteredLine filteredLine = mFilteredLines[mCurrentFilteredLineNumber];
            higlightTextInLine(filteredLine.lineNumber, false, filteredLine.highlightPositions);

            --mCurrentFilteredLineNumber;
            if (mCurrentFilteredLineNumber < 0)
            {
                mCurrentFilteredLineNumber =  mFilteredLines.size() - 1;
            }
        }
        highlightCurrentLine();
    }
}

void MainWindow::on_toolButtonNext_clicked()
{
    if (mIsFiltering && mFilteredLines.size() > 0)
    {
        if (mCurrentFilteredLineNumber == NO_CURRENT_FILTERED_LINE_NUMBER)
        {
            mCurrentFilteredLineNumber = 0;
            ui->plainTextEdit->setPlainText(mMemo.getText());
            for (const FilteredLine& filteredLine : mFilteredLines)
            {
                higlightTextInLine(filteredLine.lineNumber, false, filteredLine.highlightPositions);
            }
        }
        else
        {
            FilteredLine filteredLine = mFilteredLines[mCurrentFilteredLineNumber];
            higlightTextInLine(filteredLine.lineNumber, false, filteredLine.highlightPositions);

            ++mCurrentFilteredLineNumber;
            if (mCurrentFilteredLineNumber >= (int)mFilteredLines.size())
            {
                mCurrentFilteredLineNumber = 0;
            }
        }
        highlightCurrentLine();
    }
}

void MainWindow::highlightCurrentLine()
{
    FilteredLine filteredLine = mFilteredLines[mCurrentFilteredLineNumber];
    QTextCursor cursor(ui->plainTextEdit->document()->findBlockByLineNumber(filteredLine.lineNumber));

    // Show current line in the middle of the text edit
    int numberOfVisibleLines = ui->plainTextEdit->height() / ui->plainTextEdit->fontMetrics().height();
    ui->plainTextEdit->verticalScrollBar()->setValue(filteredLine.lineNumber - numberOfVisibleLines/2);

    QTextCharFormat fmt;
    fmt.setBackground(getDefaultBackgroungColor());
    cursor.setCharFormat(fmt);

    higlightTextInLine(filteredLine.lineNumber, true, filteredLine.highlightPositions);
    ui->plainTextEdit->setTextCursor(cursor);
}
