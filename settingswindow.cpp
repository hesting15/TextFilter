#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "settings.h"
#include <QDebug>
#include "mainwindow.h"
#include <QFontDialog>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    mFont = Settings::getInstance().getFont();
    setFontTitle();
    ui->checkBoxAlwaysOnTop->setChecked(Settings::getInstance().isAlwaysOnTop());
    ui->spinBoxStartFilter->setValue(Settings::getInstance().getFilterThreshold());
    ui->checkBoxWordWrap->setChecked(Settings::getInstance().isWordWrap());
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setFontTitle()
{
    QString isBold = mFont.bold() ? "Bold" : "Regular";
    QString title = QString("%1, %2, %3").arg(mFont.family(),
                                              QString::number(mFont.pointSize()),
                                              isBold);
    ui->labelFontName->setText(title);
    ui->labelFontName->setFont(mFont);
}

void SettingsWindow::on_pushButtonOk_clicked()
{
    Settings::getInstance().setFont(mFont);
    Settings::getInstance().setFilterThreshold(ui->spinBoxStartFilter->value());
    Settings::getInstance().setAlwaysOnTop(ui->checkBoxAlwaysOnTop->isChecked());
    Settings::getInstance().setWordWrap(ui->checkBoxWordWrap->isChecked());
    emit applySettings();
    close();
}

void SettingsWindow::on_pushButtonCancel_clicked()
{
    close();
}

void SettingsWindow::on_pushButtonChangeFont_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, mFont, this);
    if (ok)
    {
        mFont = font;
        setFontTitle();
    }
}
