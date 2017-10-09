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

    connect(ui->rbUploads, SIGNAL(pressed()), this, SLOT(switchPageUpload()));
    connect(ui->rbLocal, SIGNAL(pressed()), this, SLOT(switchPageLocal()));
    connect(ui->rbAuth, SIGNAL(pressed()), this, SLOT(switchPageServer()));
    connect(ui->rbAbout, SIGNAL(pressed()), this, SLOT(switchPageAbout()));
    connect(ui->btnDays, SIGNAL(pressed()), this, SLOT(selectLocalFolder()));

    ui->leServerPort->setValidator( new QIntValidator(0, 65535, this) );

}

EditSettingsWindow::~EditSettingsWindow()
{
    delete ui;
}

void EditSettingsWindow::setUIElementValues() {
    // Copy behavior
    QString copyMode = sets->value("copyMode", "url").toString().toLower();

    // Auto Upload
    bool uploadImages = sets->value("uploadImages", true).toBool();
    bool uploadVideos = sets->value("uploadVideos", true).toBool();

    // Ask For Titles
    bool askTitleImg = sets->value("titleImg", false).toBool();
    bool askTitleVid = sets->value("titleVid", false).toBool();
    bool askTitleFile = sets->value("titleFile", false).toBool();

    QString saveDirectory = sets->value("localSaveDir", ScreenWindow::getImagesDirectory()).toString();
    QString serverURL = sets->value("serverURL", "").toString();
    QString serverPort = sets->value("serverPort", "443").toString();
    QString lkey = sets->value("authToken", "").toString();

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
