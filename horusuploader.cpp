#include "horus.h"
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

HorusUploader::HorusUploader(QString serverURL, QString serverPort, QString authToken, bool useSSL)
{
    sslOn = useSSL;
    SERVER_URL = serverURL;
    SERVER_PORT = serverPort;
    AUTH_TOKEN = authToken;
}


void HorusUploader::upload(bool isVideo, QString filename){
    filename = filename.trimmed();
    QFile toUpload(filename);
    if(toUpload.exists()){
        toUpload.open(QIODevice::ReadOnly);
        QEventLoop el;
        QNetworkAccessManager nMgr;
        QObject::connect(&nMgr, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));


        QString reqURL("");
        reqURL += "http";
        if(sslOn){ reqURL += "s"; }
        reqURL += "://" + SERVER_URL + ":" + SERVER_PORT + "/image/upload";
        QNetworkRequest req(QUrl(QString("").append(reqURL)));
        if(isVideo){
            req.setHeader(QNetworkRequest::ContentTypeHeader, "video/webm");
        }else{
            req.setHeader(QNetworkRequest::ContentTypeHeader, "image/png");
        }

        QByteArray imgData = toUpload.readAll();
        int origSize = imgData.length();

        QByteArray postData;
        if(isVideo){
            postData.append("data:video/webm;base64,");
        }else{
            postData.append("data:image/png;base64,");
        }

        postData.append(QString(imgData.toBase64()));
        req.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
        QNetworkReply *reply = nMgr.post(req, postData);


        el.exec();


        if(reply->error() == QNetworkReply::NoError){
            reply->open(QIODevice::ReadOnly);
            emit uploadCompleted(QString(reply->readAll()));
            reply->close();
        }else{
            emit uploadFailed(QString(reply->error()));
            reply->close();
        }
        toUpload.close();
        delete reply;
        postData.clear();
        imgData.clear();
    }
}


void HorusUploader::checkLatestVersion(){
    QString reqURL("http");
    if(sslOn){ reqURL += "s"; }
    reqURL += "://" + SERVER_URL + ":" + SERVER_PORT + "/version/"; // that trailing slash is actually important.
    QEventLoop el;
    QNetworkAccessManager manager;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));
    QNetworkRequest req(QUrl(QString("").append(reqURL)));
    QNetworkReply * reply = manager.get(req);

    el.exec();

    reply->open(QIODevice::ReadOnly);
    if(reply->error()){
        emit version(Horus::HORUS_VERSION);
    }else{
        emit version(QString(reply->readAll()));
    }
    reply->close();
    delete reply;
}
