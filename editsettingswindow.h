#ifndef EDITSETTINGSWINDOW_H
#define EDITSETTINGSWINDOW_H

#include <horussettings.h>
#include <QMainWindow>
#include <QSettings>

namespace Ui {
class EditSettingsWindow;
}

class EditSettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditSettingsWindow(HorusSettings *settings, QWidget *parent = 0);
    ~EditSettingsWindow();

private:
    void setUIElementValues();
    QString getSystemImagesFolder();

private slots:
    void switchPageUpload();
    void switchPageLocal();
    void switchPageServer();
    void switchPageAbout();
    void selectLocalFolder();
    void saveAllAndClose();
private:
    Ui::EditSettingsWindow *ui;
    HorusSettings * sets;
};

#endif // EDITSETTINGSWINDOW_H
