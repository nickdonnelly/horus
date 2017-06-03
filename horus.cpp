#include "horus.h"
#include <settingsdialog.h>
#include <screenwindow.h>
#include <horusuploader.h>
#include <QApplication>
#include <QString>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QMenu>
#include <QAction>

Horus::Horus()
{
    createTrayIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();

    uploader = new HorusUploader();

    connect(uploader, SIGNAL(uploadCompleted(QString)), this, SLOT(uploadComplete(QString)));
    uploader->upload("hello");
    uploader->upload("goodbye");
}

void Horus::uploadComplete(QString url){
    // Notify user, copy to clipboard, open in browser, etc.
    // TODO
    QTextStream(stdout) << url << endl;
}

void Horus::messageClicked(){
}

void Horus::iconActivated(QSystemTrayIcon::ActivationReason reason){
    switch(reason){
    case QSystemTrayIcon::Context:
        break;
    case QSystemTrayIcon::Trigger:
        openScreenshotWindow();
        break;
    case QSystemTrayIcon::DoubleClick:
        openScreenshotWindow();
        break;
    }
}

void Horus::openScreenshotWindow(){
   ScreenWindow *sw = new ScreenWindow(uploader);
   sw->show();
}

void Horus::openSettingsWindow(){
    SettingsDialog *sd = new SettingsDialog();
    sd->show();
}

void Horus::createTrayIcon(){
    QAction *actionTakeScreenshot, *actionSettings, *actionQuit;
    trayIconMenu = new QMenu(this);
    actionTakeScreenshot = trayIconMenu->addAction(tr("Take Screenshot"));
    actionSettings = trayIconMenu->addAction(tr("Settings"));
    trayIconMenu->addSeparator();
    actionQuit = trayIconMenu->addAction(tr("Quit"));

    connect(actionTakeScreenshot, SIGNAL(triggered()), this, SLOT(openScreenshotWindow()));
    connect(actionSettings, SIGNAL(triggered()), this, SLOT(openSettingsWindow()));
    connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon("horus.png"));
    trayIcon->setContextMenu(trayIconMenu);
}

void Horus::closeEvent(QCloseEvent *evt){
    hide();
    evt->ignore();
}
