#ifndef EDITSETTINGSWINDOW_H
#define EDITSETTINGSWINDOW_H

#include <horussettings.h>
#include <horusuploader.h>
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
    explicit EditSettingsWindow(HorusSettings *settings, HorusUploader *upl, QWidget *parent = 0);
    ~EditSettingsWindow();

private:
    void setUIElementValues();
    void saveHotkeys();

    QString getSystemImagesFolder();

private slots:
    void switchPageUpload();
    void switchPageLocal();
    void switchPageServer();
    void switchPageHotkeys();
    void switchPageAbout();
    void selectLocalFolder();
    void saveAllAndClose();
    void checkForUpdates();
    void showChangelogs();

private:
    Ui::EditSettingsWindow *ui;
    HorusSettings * sets;
    HorusUploader * uploader;

    HotkeySelector *hksScreenshot, *hksVideo;

    void setupHotkeysPanel();
};

#endif // EDITSETTINGSWINDOW_H
