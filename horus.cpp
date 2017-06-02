#include "horus.h"
#include <screenwindow.h>
#include <QApplication>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QMenu>
#include <QAction>

Horus::Horus()
{
    createTrayIcon();
    setVisible(false);
    hide();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
}


void Horus::messageClicked(){
    ScreenWindow sWindow;
    sWindow.show();
}

void Horus::iconActivated(QSystemTrayIcon::ActivationReason reason){
    QTextStream(stdout) << "CLICKED" << endl;
    switch(reason){
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        ScreenWindow sWindow;
        sWindow.show();
        break;
    }
}

void Horus::createTrayIcon(){
    QAction actionTakeScreen(tr("Take &Screenshot"), this);
    QAction actionQuit(tr("&Quit"), this);
    connect(&actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(&actionTakeScreen);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(&actionQuit);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon("horus.png"));
    trayIcon->setContextMenu(trayIconMenu);
}

void Horus::closeEvent(QCloseEvent *evt){
    hide();
    evt->ignore();
}
