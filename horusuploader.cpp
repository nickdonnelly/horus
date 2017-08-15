#include "horus.h"
#include "horusuploader.h"
#include <QString>
#include <QFile>
#include <QFileInfo>
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
    gmgr = new QNetworkAccessManager(this)3
    QObject::connect(gmgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileUploadComplete(QNetworkReply*)));
}

QString HorusUploader::build_base_req_string(){
    QString reqURL("");
    reqURL += "http";
    if(sslOn){ reqURL += "s"; }
    reqURL += "://" + SERVER_URL + ":" + SERVER_PORT + "/";
    return reqURL;
}

void HorusUploader::append_auth_str(QString * req, bool firstParam){
    if(firstParam){
        req->append("?license_key=" + AUTH_TOKEN);
    }else{
        req->append("&license_key=" + AUTH_TOKEN);
    }
}

void HorusUploader::upload(bool isVideo, QString filename){
    filename = filename.trimmed();
    QFile toUpload(filename);
    if(toUpload.exists()){
        toUpload.open(QIODevice::ReadOnly);
        QEventLoop el;
        QNetworkAccessManager nMgr;
        QObject::connect(&nMgr, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));


        QString reqURL = build_base_req_string().append("image/upload");
        append_auth_str(&reqURL, true);
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

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // 201 == created, 200 == ok
        if(statusCode == 201 && reply->error() == QNetworkReply::NoError){
            emit uploadCompleted(QString(reply->readAll()));
        }else{
            emit uploadFailed("Server returned status " + QString::number(statusCode));
        }
        reply->close();
        toUpload.close();
        reply->deleteLater();
        postData.clear();
        imgData.clear();
    }
}

void HorusUploader::uploadFile(QString filename){
    filename = filename.trimmed();
    QFile toUpload(filename);
    if(toUpload.exists()){
        // Open file, initialize request string, allocate event loop
        QFileInfo toUploadInfo(filename);
        toUpload.open(QIODevice::ReadOnly);

        if(toUpload.size() > 10485760){ // 10mb
            emit uploadFailed("File: " + filename + " larger than 10MB! Upload aborted.");
            return;
        }else if(toUploadInfo.isDir()){
            emit uploadFailed("File: " + filename + " is a directory! Upload aborted.");
            return;
        }
        QNetworkRequest req;

        QString reqURL = build_base_req_string().append("file/upload/");
        if(filename.endsWith(".txt")){
            reqURL += "text";
            req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
        }else{
            reqURL += "binary";
            req.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
        }
        append_auth_str(&reqURL, true);
        reqURL += "&filename=" + toUploadInfo.fileName().replace(" ", "").replace("%20", ""); // just the raw filename without path
        req.setUrl(QUrl(QString("").append(reqURL)));

        // Add data, determine length
        QByteArray postData = toUpload.readAll();
        req.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
        toUpload.close();

        // Fire request and wait for completion
        QNetworkReply *reply = gmgr->post(req, postData);
        QObject::connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(uploadProgressSlot(qint64,qint64)));
    }
}

void HorusUploader::fileUploadComplete(QNetworkReply *reply){
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201){
        emit uploadCompleted(QString(reply->readAll()));
    }else{
        QUrlQuery q(reply->request().url());
        emit uploadFailed("Failed on file: " + q.queryItemValue("filename"));
    }
    reply->close();
    reply->deleteLater();
}

void HorusUploader::uploadProgressSlot(qint64 bytesSent, qint64 bytesTotal){
    emit uploadProgress(bytesSent, bytesTotal);
}


void HorusUploader::checkLatestVersion(){
    QString reqURL = build_base_req_string().append("version/");
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

void HorusUploader::resetCreds(QString serverURL, QString serverPort, QString authToken, bool useSSL){
    sslOn = useSSL;
    SERVER_URL = serverURL;
    SERVER_PORT = serverPort;
    AUTH_TOKEN = authToken;
}
