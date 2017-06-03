#include "horus.h"
#include <settingsdialog.h>
#include <screenwindow.h>
#include <horusuploader.h>
#include <QApplication>
#include <QString>
#include <QDesktopServices>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QMenu>
#include <QSettings>
#include <QAction>
#include <QClipboard>

Horus::Horus()
{
    createTrayIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();

    uploader = new HorusUploader();
    sets = new QSettings("horus-settings.ini", QSettings::IniFormat);
    sw = new ScreenWindow(uploader);
    sw->hide();
    connect(uploader, SIGNAL(uploadCompleted(QString)), this, SLOT(uploadComplete(QString)));
}

void Horus::uploadComplete(QString url){
    if(sets->value("openInBrowser", true).toBool()){
        QDesktopServices::openUrl(QString("http://") + url);
    }
    if(sets->value("copyMode", "url").toString() == "url"){
        QClipboard* board = QApplication::clipboard();
        board->setText(url);
    }
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
    sw->close();
    sw->deleteLater();
   delete sw;
   sw = new ScreenWindow(uploader);
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
