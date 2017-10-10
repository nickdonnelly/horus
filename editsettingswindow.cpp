#include "editsettingswindow.h"
#include "screenwindow.h"
#include "ui_editsettingswindow.h"
#include <QFileDialog>
#include <QIntValidator>
#include <QSettings>
#include <QStackedWidget>
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
    bool askTitleImg, askTitleVid, askTitleFile;

    if(ui->rbCopyURL->isChecked()){
        copyMode = "url";
    }else if(ui->rbCopyURL->isCheckable()){
        copyMode = "image";
    }else{
        copyMode = "none";
    }

    uploadImages = ui->rbUplImg->isChecked();
    uploadVideos = ui->rbUplVid->isChecked();

    uploadZIP = ui->rbZip->isChecked();

    askTitleImg = ui->rbTitleImg->isChecked();
    askTitleVid = ui->rbTitleVid->isChecked();
    askTitleFile = ui->rbTitleFile->isChecked();

    saveDirectory = ui->leSaveFolder->text();

    serverURL = ui->leServerAddr->text();
    serverPort = ui->leServerPort->text();
    lkey = ui->leLicenseKey->text();


    // *** SAVE SETTINGS ***
    sets->setValue("general/copyMode", copyMode);
    sets->setValue("general/saveDirectory", saveDirectory);

    sets->setValue("image/upload", uploadImages);
    sets->setValue("image/askTitle", askTitleImg);

    sets->setValue("video/upload", uploadVideos);
    sets->setValue("video/askTitle", askTitleVid);

    sets->setValue("file/askTitle", askTitleFile);

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
    QString copyMode = sets->value("general/copyMode", "url").toString().toLower();
    QString saveDirectory = sets->value("general/saveDirectory", ScreenWindow::getImagesDirectory()).toString();

    // Auto Upload
    bool uploadImages = sets->value("image/upload", true).toBool();
    bool uploadVideos = sets->value("video/upload", true).toBool();

    // Ask For Titles
    bool askTitleImg = sets->value("image/askTitle", false).toBool();
    bool askTitleVid = sets->value("video/askTitle", false).toBool();
    bool askTitleFile = sets->value("file/askTitle", false).toBool();

    bool uploadZIP = sets->value("file/multipleUpload", "nozip").toString().toLower() == "zip";

    QString serverURL = sets->value("auth/serverURL", "").toString();
    QString serverPort = sets->value("auth/serverPort", "443").toString();
    QString lkey = sets->value("auth/authToken", "").toString();

    // ** SETTING UI STATE **
    if(uploadImages) ui->rbUplImg->setChecked(true);
    if(uploadVideos) ui->rbUplVid->setChecked(true);

    if(askTitleImg) ui->rbTitleImg->setChecked(true);
    if(askTitleVid) ui->rbTitleVid->setChecked(true);
    if(askTitleFile) ui->rbTitleFile->setChecked(true);

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
    QString directory = d.getExistingDirectory(this, QString(""), QString("/"));
    QTextStream(stdout) << directory << endl;
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
