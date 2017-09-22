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


const QString Horus::HORUS_VERSION = QString("2.0.0");

Horus::Horus(){
    main_icon = QIcon(":/res/horus.png");
    recording_icon = QIcon(":/res/horus_recording.png");
    sets = new QSettings("horus-settings.ini", QSettings::IniFormat);
    fileDropper = new FileDropper(sets);
    textDropper = new TextDropper(sets);

    connect(textDropper, SIGNAL(complete(QString)), this, SLOT(uploadComplete(QString)));
    connect(textDropper, SIGNAL(failure(QString)), this, SLOT(uploadFailed(QString)));
    createTrayIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    firstTime = true;
    setWindowIcon(main_icon);
    trayIcon->show();

    //uploader = new HorusUploader(sets->value("serverURL", "").toString(), sets->value("serverPort", "80").toString(), sets->value("authToken", "").toString(), sets->value("useSSL", false).toBool());
    uploader = new HorusUploader(sets);
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
        mb->setText("The upload failed: the license key you have provided is invalid.");
        mb->exec();
    }else{
        sets->sync();
        if(sets->value("openInBrowser", true).toBool()){
            QTextStream(stdout) << url << endl;
            QDesktopServices::openUrl(url);
        }
        if(sets->value("copyMode", "url").toString() == "url"){
            QClipboard* board = QApplication::clipboard();
            board->setText(url);
        }
    }
}

void Horus::openManage() {
    QString authstr = uploader->get_auth_str();
    QDesktopServices::openUrl(authstr);
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
    sets->sync();
    if(sets->value("copyMode", "none").toString() == "image"){
        QClipboard* board = QApplication::clipboard();
        board->setPixmap(still);
    }
    // Hide all the windows until upload complete.
    for(int i = 0; i < windows.size(); i++){
        windows.at(i)->hide();
    }
}

void Horus::recordingStart(){
   trayIcon->setIcon(recording_icon);
}

void Horus::recordingFinished(int exitcode){
    trayIcon->setIcon(main_icon);
    if(exitcode != 0){
        QMessageBox * confBox = new QMessageBox();
        confBox->setWindowIcon(main_icon);
        confBox->setWindowTitle("Video error");
        confBox->setText("There was a problem with ffmpeg (exit status" + QString::number(exitcode));
        confBox->show();
    }
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
    screens = QGuiApplication::screens();
    windows = QList<QMainWindow*>();
    for(int i = 0; i < screens.size(); i++){
        QMainWindow * window = new ScreenWindow(screens.at(i), uploader, -1);
        connect(window, SIGNAL(stillTaken(QPixmap)), this, SLOT(stillImageTaken(QPixmap)));
        connect(window, SIGNAL(closing()), this, SLOT(screenWindowClosed()));
        windows.push_back(window);
        window->show();
    }
}


void Horus::openVideoWindow10(){
    screens = QGuiApplication::screens();
    windows = QList<QMainWindow*>();
    for(int i = 0; i < screens.size(); i++){
        QMainWindow * window = new ScreenWindow(screens.at(i), uploader, 10);
        connect(window, SIGNAL(recordStarted()), this, SLOT(recordingStart()));
        connect(window, SIGNAL(recordEnded(int)), this, SLOT(recordingFinished(int)));
        connect(window, SIGNAL(closing()), this, SLOT(screenWindowClosed()));
        windows.push_back(window);
        window->show();
    }
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
        screens = QGuiApplication::screens();
        windows = QList<QMainWindow*>();
        for(int i = 0; i < screens.size(); i++){
            QMainWindow * window = new ScreenWindow(screens.at(i), uploader, dur);
            connect(window, SIGNAL(recordStarted()), this, SLOT(recordingStart()));
            connect(window, SIGNAL(recordEnded(int)), this, SLOT(recordingFinished(int)));
            connect(window, SIGNAL(closing()), this, SLOT(screenWindowClosed()));
            windows.push_back(window);
            window->show();
        }
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
            *actionDropFile, *actionPaste, *actionManage;
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

    actionPaste = trayIconMenu->addAction(tr("Paste Clipboard Text"));
    actionPaste->setIcon(QIcon(":/res/paste.png"));

    trayIconMenu->addSeparator();

    actionManage = trayIconMenu->addAction(tr("Manage Uploads"));
    actionManage->setIcon(QIcon(":/res/manage.png"));

    actionSettings = trayIconMenu->addAction(tr("Settings"));
    actionSettings->setIcon(QIcon(":/res/settings.png"));

    actionQuit = trayIconMenu->addAction(tr("Quit"));
    actionQuit->setIcon(QIcon(":/res/stop.png"));

    connect(actionTakeScreenshot, SIGNAL(triggered()), this, SLOT(openScreenshotWindow()));
    connect(actionBoxVideo, SIGNAL(triggered()), this, SLOT(openVideoWindow10()));
    connect(actionBoxVideoDur, SIGNAL(triggered()), this, SLOT(openVideoWindowDur()));
    connect(actionEditLast, SIGNAL(triggered()), this, SLOT(openEditLastWindow()));
    connect(actionDropFile, SIGNAL(triggered()), fileDropper, SLOT(fileDropped()));

    connect(actionManage, SIGNAL(triggered()), this, SLOT(openManage()));
    connect(actionSettings, SIGNAL(triggered()), this, SLOT(openSettingsWindow()));
    connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(actionPaste, SIGNAL(triggered()), textDropper, SLOT(textDropped()));
    // CONNECT SUCCESS/FAIL!

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
            sets->sync();
            QString reqStr = uploader->build_base_req_string();
            reqStr += "/meta/get_latest";
            reqStr += platformString;
            UpdateDownloadDialog * downloadDialog = new UpdateDownloadDialog(
                        reqStr, sets->value("authToken", "").toString());
            downloadDialog->setWindowIcon(main_icon);
            downloadDialog->show();
        }
    }
}

void Horus::screenWindowClosed(){
    while(windows.size() > 0){
        QMainWindow * win = windows.front();
        windows.pop_front();
        win->close();
        win->deleteLater();
    }
}
