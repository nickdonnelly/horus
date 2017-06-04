#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QAbstractButton>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QSettings * settings, QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::SettingsDialog *ui;
    void saveSettings(QSettings * settings);
    void updateUIFromSettings(QSettings * settings);
    QSettings * sets;
};

#endif // SETTINGSDIALOG_H
