#ifndef HORUS_H
#define HORUS_H

#include <screenwindow.h>
#include <QObject>
#include <QSettings>
#include <QCloseEvent>
#include <horusuploader.h>
#include <QMainWindow>
#include <QSystemTrayIcon>

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
    void openSettingsWindow();
    void uploadComplete(QString url);
    void uploadFailed(QString url);
    void stillImageTaken(QPixmap still);
    void recordingStart();
    void recordingFinished();
    void versionStringReturned(QString version);

private:
    void createTrayIcon();
    bool firstTime;
    HorusUploader * uploader;
    ScreenWindow * sw;
    QSettings * sets;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QIcon main_icon;
    QIcon recording_icon;

protected:
    void closeEvent(QCloseEvent * evt);


};

#endif // HORUS_H
