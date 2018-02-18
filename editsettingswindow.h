#ifndef EDITSETTINGSWINDOW_H
#define EDITSETTINGSWINDOW_H

#include <horussettings.h>
#include <QMainWindow>
#include <QSettings>
#include <components/hotkeyselector.h>

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
    void switchPageHotkeys();
    void switchPageAbout();
    void selectLocalFolder();
    void saveAllAndClose();
private:
    Ui::EditSettingsWindow *ui;
    HorusSettings * sets;

    HotkeySelector *hksScreenshot, *hksVideo;

    void setupHotkeysPanel();
};

#endif // EDITSETTINGSWINDOW_H
