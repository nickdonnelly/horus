#ifndef EDITSETTINGSWINDOW_H
#define EDITSETTINGSWINDOW_H

#include <net/horusuploader.h>
#include <horussettings.h>

#include <memory>

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
    explicit EditSettingsWindow(std::shared_ptr<HorusSettings> settings, std::shared_ptr<HorusUploader> upl, QWidget *parent = 0);
    ~EditSettingsWindow();

signals:
    void closing();

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
    std::shared_ptr<HorusSettings> sets;
    std::shared_ptr<HorusUploader> uploader;

    HotkeySelector *hks;

    void setupHotkeysPanel();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // EDITSETTINGSWINDOW_H
