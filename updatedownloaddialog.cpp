#include "updatedownloaddialog.h"
#include "ui_updatedownloaddialog.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

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
    updatePackage.close();
}
