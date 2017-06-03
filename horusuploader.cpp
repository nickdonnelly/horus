#include "horusuploader.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QByteArray>
#include <QUrlQuery>
#include <QEventLoop>

HorusUploader::HorusUploader()
{

}

void HorusUploader::upload(QString filename){
    QFile toUpload(filename);
    if(toUpload.exists()){
        toUpload.open(QIODevice::ReadOnly);
        QEventLoop el;
        QNetworkAccessManager nMgr;
        QObject::connect(&nMgr, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));

        QNetworkRequest req(QUrl(QString("http://localhost:3350/image/upload")));
        req.setHeader(QNetworkRequest::ContentTypeHeader, "image/png");
        QByteArray imgData = toUpload.readAll();
        int origSize = imgData.length();

        QByteArray postData;
        postData.append("data:image/png;base64,");
        postData.append(QString(imgData.toBase64()));
        req.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
        QNetworkReply *reply = nMgr.post(req, postData);

        el.exec();

        if(reply->error() == QNetworkReply::NoError){
            QTextStream(stdout) << "No error" << endl;
            reply->open(QIODevice::ReadOnly);
            QTextStream(stdout) << "Reply " << QString(reply->readAll()) << endl;
            reply->close();
        }else{
            QTextStream(stdout) << reply->error() << endl;
        }

        emit uploadCompleted(filename);
    }
}
