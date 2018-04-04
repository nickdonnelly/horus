#ifndef HORUS_H
#define HORUS_H

#include <window/screenwindow.h>
#include <net/horusuploader.h>
#include <filedropper.h>
#include <horussettings.h>
#include <textdropper.h>

#include <memory>

#include <QObject>
#include <QSettings>
#include <QCloseEvent>
#include <QMainWindow>
#include <QSystemTrayIcon>

// These includes must come last.
#ifdef Q_OS_LINUX
#include <helpers/xkeyeventfilter.h>
#define PLATFORM "linux"
#else
#include <helpers/win32hotkeyregistry.h>
#define PLATFORM "win64"
#endif



class Horus : public QMainWindow
{
    Q_OBJECT

public:
    Horus();

    static const QString HORUS_VERSION;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void openVideoWindow10();
    void openVideoWindowDur();
    void openScreenshotWindow();
    void openEditLastWindow();
    void openManage();
    void openSettingsWindow();
    void uploadComplete(QString url);
    void uploadFailed(QString url);
    void stillImageTaken(QPixmap still);
    void recordingStart();
    void recordingFinished(int exitcode);
    void versionStringReturned(QString version);
    void screenWindowClosed();
    void setsUpdated();
    void executeShortcut(int ident);
    void refreshHotkeys();

private:
    void createTrayIcon();
    void showChangelogs();
    void registerHotkeys();
    void deregisterHotkeys();
    bool firstTime;
    QList<QScreen*> screens;
    QList<QMainWindow*> windows;
    ScreenWindow * sw;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QIcon main_icon;
    QIcon recording_icon;

    std::shared_ptr<HorusSettings> sets;
    std::shared_ptr<HorusUploader> uploader;

    std::unique_ptr<FileDropper> fileDropper;
    std::unique_ptr<TextDropper> textDropper;

#ifdef Q_OS_LINUX
    XKeyEventFilter *nefScreen;
#else
    Win32HotkeyRegistry *winHotkeyRegistry;
#endif

protected:
    void closeEvent(QCloseEvent * evt);


};

#endif // HORUS_H
