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

UpdateDownloadDialog::UpdateDownloadDialog(QString url, QWidget *parent) :
    QDialog(parent),
    reqURL(url),
    ui(new Ui::UpdateDownloadDialog)
{
    ui->setupUi(this);
    ui->pbDownload->setMinimum(0);
    ui->pbDownload->setMaximum(100);
    ui->pbDownload->setValue(0);
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloaded(QNetworkReply*)));
    downloadUpdate();
}

UpdateDownloadDialog::~UpdateDownloadDialog()
{
    delete ui;
}

void UpdateDownloadDialog::progressUpdate(qint64 bytesReceived, qint64 bytesTotal){
    int percent = 100 * bytesReceived / bytesTotal;
    ui->pbDownload->setValue(percent);
}

void UpdateDownloadDialog::downloadUpdate(){
    QNetworkRequest req(QUrl(QString("").append(reqURL)));
    QNetworkReply *reply = manager->get(req);
    reply->setParent(this);
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progressUpdate(qint64, qint64)));
}

void UpdateDownloadDialog::downloaded(QNetworkReply* reply){
    downloadedData = reply->readAll();
    reply->close();

    QFile updatePackage("horus_update.zip");
    bool isOpened = updatePackage.open(QIODevice::WriteOnly);
    if(isOpened){
        int bytesWritten = updatePackage.write(downloadedData);
        QTextStream(stdout) << "Wrote update package to horus_update.zip: " << bytesWritten << " bytes written" << endl;
    }
    updatePackage.close();
    attemptExtract();
}

void UpdateDownloadDialog::attemptExtract(){
    QString platform_string("");
    QDir update_extract_dir("update_package");
    if(update_extract_dir.exists()){
        update_extract_dir.removeRecursively(); // remove files from previous updates.
    }

    update_extract_dir.mkdir("update_package");
#ifdef Q_OS_LINUX
    QProcess proc(this);
    int exitCode = proc.execute("unzip horus_update.zip -d update_package");
    if(exitCode == 0){
        // success
        QTextStream(stdout) << "Unzipped horus_update.zip to update_package" << endl;
        QDir processDirectory(qApp->applicationDirPath());
        if(processDirectory.exists()){
            QStringList fileList = processDirectory.entryList();
            for(int i = 0; i < fileList.size(); i++){
                if(fileList.at(i) != QString("horus-settings.ini") && fileList.at(i) != QString("update_package")){
                    processDirectory.remove(fileList.at(i));
                }
            }
            // Below line doesn't execute correctly
            //proc.execute("mv update_package/* .");
        }
    }else{
        ui->lblDownload->setText(tr("Could not extract update package. Are you sure you have write permission?"));
    }

#elif Q_OS_WIN
    platform_string += "win64";
    // Figure this shit out later
#endif
}
