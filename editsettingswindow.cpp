#include "editsettingswindow.h"
#include "screenwindow.h"
#include "ui_editsettingswindow.h"
#include <QIntValidator>
#include <QSettings>
#include <QStandardPaths>

#include <QStackedWidget>
#include <QFileDialog>
#include <QLineEdit>
#include <QRadioButton>

#include <QTextStream>

EditSettingsWindow::EditSettingsWindow(QSettings *settings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditSettingsWindow),
    sets(settings)
{
    ui->setupUi(this);
    setUIElementValues();

    ui->leServerPort->setValidator( new QIntValidator(0, 65535, this) );

    connect(ui->rbUploads, SIGNAL(pressed()), this, SLOT(switchPageUpload()));
    connect(ui->rbLocal, SIGNAL(pressed()), this, SLOT(switchPageLocal()));
    connect(ui->rbAuth, SIGNAL(pressed()), this, SLOT(switchPageServer()));
    connect(ui->rbAbout, SIGNAL(pressed()), this, SLOT(switchPageAbout()));
    connect(ui->btnDays, SIGNAL(pressed()), this, SLOT(selectLocalFolder()));
    connect(ui->btnSave, SIGNAL(pressed()), this, SLOT(saveAllAndClose()));
}

EditSettingsWindow::~EditSettingsWindow()
{
    delete ui;
}

void EditSettingsWindow::saveAllAndClose() {
    QString copyMode;
    QString saveDirectory, serverURL, serverPort, lkey;
    bool uploadImages, uploadVideos, uploadZIP;
    bool askTitleImg, askTitleVid, askTitlePaste;
    bool askExpImg, askExpVid, askExpFile, askExpPaste;

    if(ui->rbCopyURL->isChecked()){
        copyMode = "url";
    }else if(ui->rbCopyURL->isCheckable()){
        copyMode = "image";
    }else{
        copyMode = "none";
    }

    saveDirectory = ui->leSaveFolder->text().trimmed();

    uploadImages = ui->rbUplImg->isChecked();
    uploadVideos = ui->rbUplVid->isChecked();

    uploadZIP = ui->rbZip->isChecked();

    askTitleImg = ui->rbTitleImg->isChecked();
    askTitleVid = ui->rbTitleVid->isChecked();
    askTitlePaste = ui->rbTitlePaste->isChecked();

    askExpImg = ui->rbExpImg->isChecked();
    askExpVid = ui->rbExpVid->isChecked();
    askExpFile = ui->rbExpFile->isChecked();
    askExpPaste = ui->rbExpPaste->isChecked();

    serverURL = ui->leServerAddr->text().trimmed();
    serverPort = ui->leServerPort->text().trimmed();
    lkey = ui->leLicenseKey->text().trimmed();


    // *** SAVE SETTINGS ***
    sets->setValue("other/copyMode", copyMode);
    sets->setValue("other/saveDirectory", saveDirectory);

    sets->setValue("image/upload", uploadImages);
    sets->setValue("image/askTitle", askTitleImg);
    sets->setValue("image/askExp", askExpImg);

    sets->setValue("video/upload", uploadVideos);
    sets->setValue("video/askTitle", askTitleVid);
    sets->setValue("video/askExp", askExpVid);

    sets->setValue("paste/askTitle", askTitlePaste);
    sets->setValue("paste/askExp", askExpPaste);

    sets->setValue("file/askExp", askExpFile);

    if(uploadZIP)
        sets->setValue("file/multipleUpload", "zip");
    else
        sets->setValue("file/multipleUpload", "nozip");

    sets->setValue("auth/serverURL", serverURL);
    sets->setValue("auth/serverPort", serverPort);
    sets->setValue("auth/authToken", lkey);

    // Update on disk
    sets->sync();

    // Notify observers
    emit notifyUpdated();

    close();
}

void EditSettingsWindow::setUIElementValues() {
    QString copyMode = sets->value("other/copyMode", "url").toString().toLower();
    QString saveDirectory = sets->value("other/saveDirectory", getSystemImagesFolder()).toString();

    // Auto Upload
    bool uploadImages = sets->value("image/upload", true).toBool();
    bool uploadVideos = sets->value("video/upload", true).toBool();

    // Ask For Titles
    bool askTitleImg = sets->value("image/askTitle", false).toBool();
    bool askTitleVid = sets->value("video/askTitle", false).toBool();
    bool askTitlePaste = sets->value("paste/askTitle", false).toBool();

    bool askExpImg = sets->value("image/askExp", false).toBool();
    bool askExpVid = sets->value("video/askExp", false).toBool();
    bool askExpFile = sets->value("file/askExp", false).toBool();
    bool askExpPaste = sets->value("paste/askExp", false).toBool();

    bool uploadZIP = sets->value("file/multipleUpload", "nozip").toString().toLower() == "zip";

    QString serverURL = sets->value("auth/serverURL", "").toString();
    QString serverPort = sets->value("auth/serverPort", "443").toString();
    QString lkey = sets->value("auth/authToken", "").toString();

    // ** SETTING UI STATE **
    if(uploadImages) ui->rbUplImg->setChecked(true);
    if(uploadVideos) ui->rbUplVid->setChecked(true);

    if(askTitleImg) ui->rbTitleImg->setChecked(true);
    if(askTitleVid) ui->rbTitleVid->setChecked(true);
    if(askTitlePaste) ui->rbTitlePaste->setChecked(true);

    if(askExpImg) ui->rbExpImg->setChecked(true);
    if(askExpVid) ui->rbExpVid->setChecked(true);
    if(askExpFile) ui->rbExpFile->setChecked(true);

    if(copyMode == "url"){
        ui->rbCopyURL->setChecked(true);
    }else if(copyMode == "image"){
        ui->rbCopyRes->setChecked(true);
    }else{
        ui->rbCopyNone->setChecked(true);
    }

    if(uploadZIP)
        ui->rbZip->setChecked(true);
    else
        ui->rbNoZip->setChecked(true);

    ui->leSaveFolder->setText(saveDirectory);
    ui->leServerAddr->setText(serverURL);
    ui->leServerPort->setText(serverPort);
    ui->leLicenseKey->setText(lkey);
}

void EditSettingsWindow::selectLocalFolder()
{
    QFileDialog d(this);
    QString directory = d.getExistingDirectory(this, QString("Select A Folder..."));
    ui->leSaveFolder->setText(directory);
}

void EditSettingsWindow::switchPageUpload()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void EditSettingsWindow::switchPageLocal()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void EditSettingsWindow::switchPageServer()
{
    ui->stackedWidget->setCurrentIndex(2);

}
void EditSettingsWindow::switchPageAbout()
{
    ui->stackedWidget->setCurrentIndex(3);
}

QString EditSettingsWindow::getSystemImagesFolder() {
    QString picPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/horus";
    if(!QDir(picPath).exists()){
        QDir().mkpath(picPath);
    }

    return picPath;
}
