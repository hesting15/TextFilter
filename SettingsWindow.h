#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QFont>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

signals:
    void applySettings();

private slots:
    void on_pushButtonOk_clicked();
    void on_pushButtonCancel_clicked();

    void on_pushButtonChangeFont_clicked();

private:
    Ui::SettingsWindow *ui;
    void setFontTitle();
    QFont mFont;
};

#endif // SETTINGSWINDOW_H
