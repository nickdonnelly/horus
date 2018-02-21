#include "editsettingswindow.h"
#include "screenwindow.h"
#include "ui_editsettingswindow.h"
#include <horus.h>
#include <QIntValidator>
#include <QDesktopServices>
#include <QSettings>
#include <QStandardPaths>
#include <QKeySequence>

#include <QStackedWidget>
#include <QFileDialog>
#include <QLineEdit>
#include <QRadioButton>

#include <QTextStream>

EditSettingsWindow::EditSettingsWindow(HorusSettings *settings,
                                       HorusUploader *upl, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditSettingsWindow),
    sets(settings),
    uploader(upl)
{
    ui->setupUi(this);
    setUIElementValues();

    ui->leServerPort->setValidator( new QIntValidator(0, 65535, this) );
    ui->lblAbout->setText("<html><head/><body><p align=\"center\">Horus Version "
                          + Horus::HORUS_VERSION + "</p>"
                          + "<p align=\"center\">Copyright <span style=\"font-family:'arial,sans-serif'; font-size:16px; color:#222222; background-color:#ffffff;\">©</span> 2017</p><p align=\"center\">Context menu icons courtesy of EmojiOne.</p></body></html>");

    setupHotkeysPanel();

    connect(ui->rbUploads, SIGNAL(pressed()), this, SLOT(switchPageUpload()));
    connect(ui->rbLocal, SIGNAL(pressed()), this, SLOT(switchPageLocal()));
    connect(ui->rbAuth, SIGNAL(pressed()), this, SLOT(switchPageServer()));
    connect(ui->rbAbout, SIGNAL(pressed()), this, SLOT(switchPageAbout()));
    connect(ui->rbHotkeys, SIGNAL(pressed()), this, SLOT(switchPageHotkeys()));
    connect(ui->btnDays, SIGNAL(pressed()), this, SLOT(selectLocalFolder()));
    connect(ui->btnSave, SIGNAL(pressed()), this, SLOT(saveAllAndClose()));
    connect(ui->btnChangelogs, SIGNAL(pressed()), this, SLOT(showChangelogs()));
    connect(ui->btnUpdates, SIGNAL(pressed()), this, SLOT(checkForUpdates()));
}

EditSettingsWindow::~EditSettingsWindow()
{
    delete ui;
}

void EditSettingsWindow::saveAllAndClose() {
    QString copyMode;
    QString saveDirectory, serverURL, serverPort, lkey;
    bool openInBrowser;
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

    openInBrowser = ui->rbOpenBrowser->isChecked();

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
    sets->setValue("other/openInBrowser", openInBrowser);

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

    saveHotkeys();

    // Update on disk
    sets->sync();

    // Notify observers
    sets->notifyUpdated();

    close();
}

void EditSettingsWindow::setUIElementValues() {
    QString copyMode = sets->value("other/copyMode", "url").toString().toLower();
    QString saveDirectory = sets->value("other/saveDirectory", getSystemImagesFolder()).toString();

    // Open in browser
    bool openInBrowser = sets->value("other/openInBrowser", true).toBool();

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
    ui->rbUplImg->setChecked(uploadImages);
    ui->rbUplVid->setChecked(uploadVideos);
    ui->rbOpenBrowser->setChecked(openInBrowser);

    ui->rbTitleImg->setChecked(askTitleImg);
    ui->rbTitleVid->setChecked(askTitleVid);
    ui->rbTitlePaste->setChecked(askTitlePaste);

    ui->rbExpImg->setChecked(askExpImg);
    ui->rbExpVid->setChecked(askExpVid);
    ui->rbExpFile->setChecked(askExpFile);

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

void EditSettingsWindow::saveHotkeys()
{
    sets->setValue("hotkeys/screenshot",
                   hksScreenshot->getValue().toString(QKeySequence::PortableText)
                   .replace(",", "").replace(" ", ""));

    sets->setValue("hotkeys/videodur",
                   hksVideoDur->getValue().toString(QKeySequence::PortableText)
                   .replace(",", "").replace(" ", ""));

    sets->setValue("hotkeys/videocustom",
                   hksVideoCustom->getValue().toString(QKeySequence::PortableText)
                   .replace(",", "").replace(" ", ""));
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

void EditSettingsWindow::switchPageHotkeys()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void EditSettingsWindow::switchPageServer()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void EditSettingsWindow::switchPageAbout()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void EditSettingsWindow::showChangelogs()
{
    QString url_base = uploader->build_base_browser_string();
    url_base += "/meta/changelogs";
    QDesktopServices::openUrl(url_base);
}

void EditSettingsWindow::checkForUpdates()
{
    ui->btnUpdates->setText("Checking...");
    uploader->checkLatestVersion();
    ui->btnUpdates->setText("Check for Updates");
}

QString EditSettingsWindow::getSystemImagesFolder()
{
    QString picPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/horus";
    if(!QDir(picPath).exists()){
        QDir().mkpath(picPath);
    }

    return picPath;
}

// TODO: Just create a registry intead.
void EditSettingsWindow::setupHotkeysPanel()
{
    QString sScreen = sets->value("hotkeys/screenshot").toString();
    QString sVideoDur = sets->value("hotkeys/videodur").toString();
    QString sVideoCus = sets->value("hotkeys/videocustom").toString();

    QKeySequence ksScreen(sScreen, QKeySequence::PortableText);
    QKeySequence ksVidDur(sVideoDur, QKeySequence::PortableText);
    QKeySequence ksVidCus(sVideoCus, QKeySequence::PortableText);

    // TODO: Translations
    hksScreenshot = new HotkeySelector("Open Screenshot Window", this);
    hksVideoDur = new HotkeySelector("Open Video Window (10s)", this);
    hksVideoCustom = new HotkeySelector("Open Video Window (Custom)", this);

    hksScreenshot->setKeySequence(ksScreen);
    hksVideoDur->setKeySequence(ksVidDur);
    hksVideoCustom->setKeySequence(ksVidCus);

    ui->layoutHotkeys->addWidget(hksScreenshot);
    ui->layoutHotkeys->addWidget(hksVideoDur);
    ui->layoutHotkeys->addWidget(hksVideoCustom);
}
