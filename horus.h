#ifndef HORUS_H
#define HORUS_H

#include <screenwindow.h>
#include <QDialog>
#include <QObject>
#include <QSettings>
#include <QCloseEvent>
#include <horusuploader.h>
#include <QSystemTrayIcon>

class Horus : public QDialog
{
    Q_OBJECT

public:
    Horus();

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void openScreenshotWindow();
    void openSettingsWindow();
    void uploadComplete(QString url);

private:
    void createTrayIcon();
    bool firstTime;
    HorusUploader * uploader;
    ScreenWindow * sw;
    QSettings * sets;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

protected:
    void closeEvent(QCloseEvent * evt);


};

#endif // HORUS_H
