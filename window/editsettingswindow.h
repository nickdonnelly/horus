#ifndef EDITSETTINGSWINDOW_H
#define EDITSETTINGSWINDOW_H

#include <net/horusuploader.h>
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

    void hotkeySelected(int r, int c, int pr, int pc);
    void hotkeyChanged(int m, int k);

private:
    Ui::EditSettingsWindow *ui;

    int selectedRow;
    HorusSettings * sets;
    HorusUploader * uploader;

    HotkeySelector *hks;

    void setupHotkeysPanel();
};

#endif // EDITSETTINGSWINDOW_H
