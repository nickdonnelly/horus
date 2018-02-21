#ifndef HORUS_H
#define HORUS_H

#include <screenwindow.h>
#include <filedropper.h>
#include <horusuploader.h>
#include <horussettings.h>
#include <textdropper.h>
#include <QObject>
#include <QSettings>
#include <QCloseEvent>
#include <QMainWindow>
#include <QSystemTrayIcon>
#ifdef Q_OS_LINUX
#include <nativekeyeventfilter.h>
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

private:
    void createTrayIcon();
    void showChangelogs();
    void registerHotkeys();
    bool firstTime;
    QList<QScreen*> screens;
    QList<QMainWindow*> windows;
    ScreenWindow * sw;
    HorusSettings * sets;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QIcon main_icon;
    QIcon recording_icon;

    HorusUploader * uploader;
    FileDropper * fileDropper;
    TextDropper * textDropper;

#ifdef Q_OS_LINUX
    NativeKeyEventFilter *nefScreen;

#endif

protected:
    void closeEvent(QCloseEvent * evt);


};

#endif // HORUS_H
