#include "horus.h"
#include "horusuploader.h"
#include "modelserialize.h"
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QApplication>
#include <QDesktopWidget>
#include <QInputDialog>
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
    ASK_TITLE = false;
    gmgr = new QNetworkAccessManager(this);
    QObject::connect(gmgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileUploadComplete(QNetworkReply*)));
}

HorusUploader::HorusUploader(QSettings * sets)
{
    sets->sync();
    sslOn = sets->value("useSSL", false).toBool();
    SERVER_URL = sets->value("serverURL", "").toString();
    SERVER_PORT = sets->value("serverPort", "").toString();
    AUTH_TOKEN = sets->value("authToken", "").toString();
    ASK_TITLE = sets->value("askTitle", false).toBool();
    gmgr = new QNetworkAccessManager(this);
    QObject::connect(gmgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileUploadComplete(QNetworkReply*)));
}

QString HorusUploader::build_base_req_string(){
    QString reqURL("");
    reqURL += "http";
    if(sslOn){ reqURL += "s"; }
    reqURL += "://" + SERVER_URL + ":" + SERVER_PORT;
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


        QString reqURL, title;
        if(isVideo) {
            reqURL = build_base_req_string().append("/video/new");
        }else{
            reqURL = build_base_req_string().append("/image/new");
        }

        //append_auth_str(&reqURL, true);
        if(ASK_TITLE) {
            bool ok;
            QInputDialog * dialog = new QInputDialog();
            dialog->move(QApplication::desktop()->screenGeometry().center() - dialog->pos()/2); // center it
            QString _title = dialog->getText(NULL, "Image Title", "Enter a title for the image.", QLineEdit::Normal, "Horus Screenshot", &ok);
            dialog->deleteLater();
            title = _title;
        } else {
            title = "";
        }
        QNetworkRequest req(QUrl(QString("").append(reqURL)));
        QByteArray imgData = toUpload.readAll();
        QByteArray postData;
        req.setRawHeader(QString("x-api-key").toUtf8(), AUTH_TOKEN.toUtf8());

        if(isVideo){
            req.setHeader(QNetworkRequest::ContentTypeHeader, "video/webm");
            postData.append("data:video/webm;base64,");
        }else{
            req.setHeader(QNetworkRequest::ContentTypeHeader, "image/png");
            postData.append("data:image/png;base64,");
        }

        postData.append(QString(imgData.toBase64()));

        req.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
        QNetworkReply *reply = nMgr.post(req, postData);

        el.exec();

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // 201 == created, 200 == ok
        if(statusCode == 201 && reply->error() == QNetworkReply::NoError){
            QString resStr = build_base_req_string().append(reply->rawHeader("Location"));
            emit uploadCompleted(resStr);
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
        QString reqURL = build_base_req_string().append("/file/new/");
        req.setUrl(QUrl(QString("").append(reqURL)));

        req.setRawHeader(QString("x-api-key").toUtf8(), AUTH_TOKEN.toUtf8());
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
        req.setHeader(QNetworkRequest::ContentDispositionHeader, toUploadInfo.fileName().replace(" ", "").replace("%20", ""));

        // Add data, determine length
        QByteArray postData = toUpload.readAll();
        toUpload.close();

        // Fire request and wait for completion
        QTextStream(stdout) << " Making request to " << reqURL << endl;
        QTextStream(stdout) << " HEADER " << req.header(QNetworkRequest::ContentDispositionHeader).toString() << endl;

        QNetworkReply *reply = gmgr->post(req, postData);
        QObject::connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(uploadProgressSlot(qint64,qint64)));
    }
}

void HorusUploader::sendText(QString text){
    QEventLoop el;
    QNetworkAccessManager nMgr;
    QObject::connect(&nMgr, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));
    QNetworkRequest req;
    QString reqURL = build_base_req_string().append("/paste/new");
    req.setUrl(QUrl(QString("").append(reqURL)));
    req.setRawHeader(QString("x-api-key").toUtf8(), AUTH_TOKEN.toUtf8());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc = ModelSerialize::get_paste_form("Horus Paste", text);
    QByteArray postData = doc.toJson(QJsonDocument::Compact);

    QNetworkReply *reply = gmgr->post(req, postData);
}

void HorusUploader::fileUploadComplete(QNetworkReply *reply){
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201){
        QString location = build_base_req_string();
        QTextStream(stdout) << "REPLY " << reply->readAll() << endl;
        location += QString(reply->rawHeader("Location"));
        emit uploadCompleted(location);

    }else{
        QUrlQuery q(reply->request().url());
        emit uploadFailed("Upload failed. Check your license key!");
    }
    reply->close();
    reply->deleteLater();
}

void HorusUploader::uploadProgressSlot(qint64 bytesSent, qint64 bytesTotal){
    emit uploadProgress(bytesSent, bytesTotal);
}

QString HorusUploader::get_auth_str() {
    QString reqURL = build_base_req_string().append("/manage/request_auth_url");
    QEventLoop el;
    QNetworkAccessManager manager;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));
    QNetworkRequest req(QUrl(QString("").append(reqURL)));
    req.setRawHeader(QString("x-api-key").toUtf8(), AUTH_TOKEN.toUtf8());
    QNetworkReply * reply = manager.get(req);
    el.exec();

    QString url = build_base_req_string();
    url += QString(reply->readAll());
    reply->close();
    delete reply;
    return url;
}

void HorusUploader::checkLatestVersion(){
    QString reqURL = build_base_req_string().append("/meta/version");
    QEventLoop el;
    QNetworkAccessManager manager;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));
    QNetworkRequest req(QUrl(QString("").append(reqURL)));
    QNetworkReply * reply = manager.get(req);

    el.exec();

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
