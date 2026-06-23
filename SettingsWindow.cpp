#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "Settings.h"
#include <QDebug>
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
    ui->comboBoxStyleStrategy->setCurrentIndex(
        styleStrategyToIndex(Settings::getInstance().getStyleStrategy()));
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
    Settings::getInstance().setStyleStrategy(
        indexToStyleStrategy(ui->comboBoxStyleStrategy->currentIndex()));
    emit applySettings();
    close();
}

QFont::StyleStrategy SettingsWindow::indexToStyleStrategy(int index)
{
    switch (index)
    {
    case 1:  return QFont::PreferAntialias;
    case 2:  return QFont::NoAntialias;
    case 3:  return QFont::NoSubpixelAntialias;
    case 4:  return QFont::PreferQuality;
    default: return QFont::PreferDefault;
    }
}

int SettingsWindow::styleStrategyToIndex(QFont::StyleStrategy strategy)
{
    switch (strategy)
    {
    case QFont::PreferAntialias:      return 1;
    case QFont::NoAntialias:          return 2;
    case QFont::NoSubpixelAntialias:  return 3;
    case QFont::PreferQuality:        return 4;
    default:                          return 0;
    }
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