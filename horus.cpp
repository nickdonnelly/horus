#include "horus.h"
#include <settingsdialog.h>
#include <updatedownloaddialog.h>
#include <screenwindow.h>
#include <editimagewindow.h>
#include <horusuploader.h>
#include <QInputDialog>
#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QStyle>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QMenu>
#include <QSettings>
#include <QDir>
#include <QAction>
#include <QIcon>
#include <QClipboard>


const QString Horus::HORUS_VERSION = QString("1.3.0");

Horus::Horus(){
    main_icon = QIcon(":/res/horus.png");
    recording_icon = QIcon(":/res/horus_recording.png");
    sets = new QSettings("horus-settings.ini", QSettings::IniFormat);
    fileDropper = new FileDropper(sets);
    createTrayIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    firstTime = true;
    setWindowIcon(main_icon);
    trayIcon->show();

    uploader = new HorusUploader(sets->value("serverURL", "").toString(), sets->value("serverPort", "80").toString(), sets->value("authToken", "").toString(), sets->value("useSSL", false).toBool());
    connect(uploader, SIGNAL(uploadCompleted(QString)), this, SLOT(uploadComplete(QString)));
    connect(uploader, SIGNAL(uploadFailed(QString)), this, SLOT(uploadFailed(QString)));
    connect(uploader, SIGNAL(version(QString)), this, SLOT(versionStringReturned(QString)));
    uploader->checkLatestVersion();
}

void Horus::uploadComplete(QString url){
    if(url == "license_key_invalid"){
        QMessageBox * mb = new QMessageBox();
        mb->setWindowIcon(main_icon);
        mb->setIcon(QMessageBox::Critical);
        mb->setWindowTitle("Upload failed");
        mb->setText("The image upload failed: the license key you have provided is invalid.");
        mb->exec();
    }else{
        if(sets->value("openInBrowser", true).toBool()){
            QDesktopServices::openUrl(url);
        }
        if(sets->value("copyMode", "url").toString() == "url"){
            QClipboard* board = QApplication::clipboard();
            board->setText(url);
        }
    }
}

void Horus::uploadFailed(QString failure){
    QMessageBox * mb = new QMessageBox();
    mb->setWindowIcon(main_icon);
    mb->setIcon(QMessageBox::Critical);
    mb->setWindowTitle("Upload failed");
    mb->setText("There was a failure in uploading the image:\n" + failure);
    mb->exec();
}

void Horus::stillImageTaken(QPixmap still){
    if(sets->value("copyMode", "none").toString() == "image"){
        QClipboard* board = QApplication::clipboard();
        board->setPixmap(still);
    }
}

void Horus::recordingStart(){
   trayIcon->setIcon(recording_icon);
}

void Horus::recordingFinished(){
   trayIcon->setIcon(main_icon);
}

void Horus::messageClicked(){
}

void Horus::iconActivated(QSystemTrayIcon::ActivationReason reason){
    switch(reason){
    case QSystemTrayIcon::Trigger:
        openScreenshotWindow();
        break;
    case QSystemTrayIcon::DoubleClick:
        openScreenshotWindow();
        break;
    case QSystemTrayIcon::Context:
    default:
        break;
    }
}

void Horus::openScreenshotWindow(){
    if(!firstTime){
        sw->close();
        sw->deleteLater();
    }
    firstTime = false;
    sw = new ScreenWindow(uploader, -1);
    connect(sw, SIGNAL(stillTaken(QPixmap)), this, SLOT(stillImageTaken(QPixmap)));
    sw->show();
}

void Horus::openVideoWindow10(){
    if(!firstTime){
        sw->close();
        sw->deleteLater();
    }
    firstTime = false;
    sw = new ScreenWindow(uploader, 10);
    connect(sw, SIGNAL(recordStarted()), this, SLOT(recordingStart()));
    connect(sw, SIGNAL(recordEnded()), this, SLOT(recordingFinished()));
    sw->show();
}

void Horus::openVideoWindowDur(){
    bool ok;
    QInputDialog * dialog = new QInputDialog(this);
    // TODO: This move operation is not working on linux.
    dialog->move(QApplication::desktop()->screenGeometry().center() - dialog->pos()/2);
    int dur = dialog->getInt(this, tr(""), tr("Record duration (s):"),
                                       QLineEdit::Normal, 3, 20, 1, &ok);
    dialog->deleteLater();
    if(ok){
        if(!firstTime){
            sw->close();
            sw->deleteLater();
        }
        firstTime = false;
        sw = new ScreenWindow(uploader, dur);
        connect(sw, SIGNAL(recordStarted()), this, SLOT(recordingStart()));
        connect(sw, SIGNAL(recordEnded()), this, SLOT(recordingFinished()));
        sw->show();
    }
}

void Horus::openSettingsWindow(){
    SettingsDialog *sd = new SettingsDialog(sets);
    sd->setWindowIcon(main_icon);
    sd->show();
}

void Horus::openEditLastWindow(){
    QString fileStr(ScreenWindow::getAppSaveDirectory() + "/cache_last_taken.png");
    if(QFile::exists(fileStr)){
        EditImageWindow *editWindow = new EditImageWindow(fileStr, uploader, this);
        editWindow->show();
    }
}

void Horus::createTrayIcon(){
    QAction *actionTakeScreenshot, *actionBoxVideo, *actionBoxVideoDur,
            *actionEditLast, *actionSettings, *actionQuit,
            *actionDropFile;
    trayIconMenu = new QMenu(this);
    actionTakeScreenshot = trayIconMenu->addAction(tr("Take Screenshot"));
    actionTakeScreenshot->setIcon(QIcon(":/res/screenshot.png"));

    actionBoxVideo = trayIconMenu->addAction(tr("Take 10s Recording"));
    actionBoxVideo->setIcon(QIcon(":/res/recording.png"));

    actionBoxVideoDur = trayIconMenu->addAction(tr("Take Custom Recording"));
    actionBoxVideoDur->setIcon(QIcon(":/res/recording.png"));

    actionEditLast = trayIconMenu->addAction(tr("Edit Previous Screenshot"));
    actionEditLast->setIcon(QIcon(":/res/edit_last.png"));

    actionDropFile = trayIconMenu->addAction(tr("Drop File(s) On Clipboard"));
    actionDropFile->setIcon(QIcon(":/res/dropfile.png"));

    trayIconMenu->addSeparator();

    actionSettings = trayIconMenu->addAction(tr("Settings"));
    actionSettings->setIcon(QIcon(":/res/settings.png"));

    actionQuit = trayIconMenu->addAction(tr("Quit"));
    actionQuit->setIcon(QIcon(":/res/stop.png"));

    connect(actionTakeScreenshot, SIGNAL(triggered()), this, SLOT(openScreenshotWindow()));
    connect(actionBoxVideo, SIGNAL(triggered()), this, SLOT(openVideoWindow10()));
    connect(actionBoxVideoDur, SIGNAL(triggered()), this, SLOT(openVideoWindowDur()));
    connect(actionEditLast, SIGNAL(triggered()), this, SLOT(openEditLastWindow()));
    connect(actionSettings, SIGNAL(triggered()), this, SLOT(openSettingsWindow()));
    connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(actionDropFile, SIGNAL(triggered()), fileDropper, SLOT(fileDropped()));

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(main_icon);
    trayIcon->setContextMenu(trayIconMenu);
}

void Horus::closeEvent(QCloseEvent *evt){
    hide();
    evt->ignore();
}

void Horus::versionStringReturned(QString version){
    if(version != HORUS_VERSION){
        QMessageBox * confBox = new QMessageBox();
        confBox->setWindowIcon(main_icon);
        confBox->addButton(QMessageBox::Yes);
        confBox->addButton(QMessageBox::No);
        confBox->setWindowTitle("Horus Update Available");
        QString platformString("");

#ifdef Q_OS_WIN
        platformString += "win64";
#elif defined Q_OS_LINUX
        platformString += "linux";
#endif

        confBox->setText("A Horus update is available. Would you like to download it?\nYour version: " + HORUS_VERSION + "\nNew version: " + version + "\nPlatform: " + platformString);

        int res = confBox->exec();

        if(res == QMessageBox::Yes){
            UpdateDownloadDialog * downloadDialog = new UpdateDownloadDialog("https://horus.donnelly.cc/getlatestversion/?license_key=" + sets->value("authToken", "").toString() + "&platform=" + platformString);
            downloadDialog->setWindowIcon(main_icon);
            downloadDialog->show();
        }
    }
}
