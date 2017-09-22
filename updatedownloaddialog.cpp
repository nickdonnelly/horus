#include "updatedownloaddialog.h"
#include "ui_updatedownloaddialog.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QCoreApplication>
#include <QNetworkReply>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QLabel>
#include <QStringList>

UpdateDownloadDialog::UpdateDownloadDialog(QString url, QString authtoken, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateDownloadDialog),
    reqURL(url),
    AUTH_TOKEN(authtoken)
{
    ui->setupUi(this);
    ui->btnRestart->setVisible(false);
    ui->pbDownload->setMinimum(0);
    ui->pbDownload->setMaximum(100);
    ui->pbDownload->setValue(0);
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint); // get rid of help button
    manager = new QNetworkAccessManager(this);
    update_dir_str = qApp->applicationDirPath() + "/update_package";
    if(!QDir(update_dir_str).exists()){
        QDir(update_dir_str).mkpath(update_dir_str);
    }
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded(QNetworkReply*)));
    connect(ui->btnRestart, SIGNAL(pressed()), this, SLOT(restartClicked()));
    downloadUpdate();
}

UpdateDownloadDialog::~UpdateDownloadDialog()
{
    delete ui;
}

void UpdateDownloadDialog::restartClicked(){
    QProcess proc;
#ifdef Q_OS_WIN
    proc.startDetached(qApp->applicationDirPath() + "/horusqt.exe");
#elif defined Q_OS_LINUX
    proc.startDetached(qApp->applicationDirPath() + "/horusqt");
#endif
    qApp->exit(0);
}

void UpdateDownloadDialog::progressUpdate(qint64 bytesReceived, qint64 bytesTotal){
    int percent = 100 * bytesReceived / bytesTotal;
    ui->pbDownload->setValue(percent);
}

void UpdateDownloadDialog::downloadUpdate(){
    QNetworkRequest req(QUrl(QString("").append(reqURL)));
    req.setRawHeader(QString("x-api-key").toUtf8(), AUTH_TOKEN.toUtf8());
    QNetworkReply *reply = manager->get(req);
    reply->setParent(this);
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progressUpdate(qint64, qint64)));
}

void UpdateDownloadDialog::downloaded(QNetworkReply* reply){
    downloadedData = reply->readAll();
    reply->close();

    QFile updatePackage(update_dir_str + "/horus_update.zip");
    bool isOpened = updatePackage.open(QIODevice::WriteOnly);
    if(isOpened){
        updatePackage.write(downloadedData);
    }
    updatePackage.close();
    attemptExtract();
}

void UpdateDownloadDialog::attemptExtract(){
    ui->lblDownload->setText("Extracting update package...");

    QDir update_extract_dir(update_dir_str);
    update_extract_dir.mkdir(update_dir_str);
    QProcess proc(this);
    QString procStr("");
#ifdef Q_OS_WIN
    procStr += "\"" + qApp->applicationDirPath() + "/bin/unzip\" -o \"" + update_dir_str + "/horus_update.zip\" -d \"" + qApp->applicationDirPath() + "\"";
#elif defined Q_OS_LINUX
    procStr += "unzip -o \"" + update_dir_str + "/horus_update.zip\" -d \"" + qApp->applicationDirPath() + "\"";
#endif
    QTextStream(stdout) << procStr << " end " << endl;
    int exitCode = proc.execute(procStr);
    if(exitCode == 0){
        // success
        ui->lblDownload->setText("Restart Horus for the update to take effect.");
        ui->btnRestart->setVisible(true);
        ui->pbDownload->setVisible(false);
    }else{
        ui->lblDownload->setText(tr("Could not extract update package. Are you sure you have write permission?"));
        QTextStream(stdout) << "Unzip exited with code " << exitCode << endl;
    }
}
