#ifndef HORUS_H
#define HORUS_H

#include <QDialog>
#include <QObject>
#include <QCloseEvent>
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

private:
    void createTrayIcon();

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

protected:
    void closeEvent(QCloseEvent * evt);


};

#endif // HORUS_H
