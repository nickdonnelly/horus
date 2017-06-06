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
#include <QDir>
#include <QAction>
#include <QIcon>
#include <QClipboard>

Horus::Horus()
{
    createTrayIcon();
    setWindowIcon(QIcon(":/res/horus.png"));
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    firstTime = true;
    trayIcon->show();

    sets = new QSettings("horus-settings.ini", QSettings::IniFormat);
    uploader = new HorusUploader(sets->value("serverURL", "").toString(), sets->value("serverPort", "80").toString(), QString(""), sets->value("useSSL", false).toBool());
//    sw = new ScreenWindow(uploader);
//    sw->show();
//    sw->hide();
//    sw->close();
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
//    sw->close();
//    sw->deleteLater();
//    delete sw;
    if(!firstTime){
        sw->close();
        sw->deleteLater();
    }
    firstTime = false;
    sw = new ScreenWindow(uploader);
    sw->show();
}

void Horus::openSettingsWindow(){
    SettingsDialog *sd = new SettingsDialog(sets, this);
    sd->show();
}

void Horus::createTrayIcon(){
    QAction *actionTakeScreenshot, *actionBoxVideo, *actionSettings, *actionQuit;
    trayIconMenu = new QMenu(this);
    actionTakeScreenshot = trayIconMenu->addAction(tr("Take Screenshot"));
    actionTakeScreenshot->setIcon(QIcon(":/res/screenshot.png"));

    actionBoxVideo = trayIconMenu->addAction(tr("Take Recording"));
    actionBoxVideo->setIcon(QIcon(":/res/recording.png"));

    actionSettings = trayIconMenu->addAction(tr("Settings"));
    actionSettings->setIcon(QIcon(":/res/settings.png"));

    trayIconMenu->addSeparator();
    actionQuit = trayIconMenu->addAction(tr("Quit"));
    actionQuit->setIcon(QIcon(":/res/stop.png"));

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
