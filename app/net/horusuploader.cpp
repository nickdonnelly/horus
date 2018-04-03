#include "horus.h"
#include "horusuploader.h"
#include <window/expirationselectiondialog.h>
#include "models.h"
#include <QString>
#include <QFile>
#include <QFileInfo>
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
    gmgr = new QNetworkAccessManager(this);
    QObject::connect(gmgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileUploadComplete(QNetworkReply*)));
}

HorusUploader::HorusUploader(std::shared_ptr<QSettings> settings)
{
    sets = settings;
    sets->sync();
    SERVER_URL = sets->value("auth/serverURL", "").toString();
    SERVER_PORT = sets->value("auth/serverPort", "").toString();
    AUTH_TOKEN = sets->value("auth/authToken", "").toString();
    sslOn = SERVER_PORT == "443";

    ask_title_image = sets->value("image/askTitle", false).toBool();
    ask_title_video = sets->value("video/askTitle", false).toBool();
    ask_title_paste = sets->value("paste/askTitle", false).toBool();

    ask_exp_image = sets->value("image/askExp", false).toBool();
    ask_exp_video = sets->value("video/askExp", false).toBool();
    ask_exp_paste = sets->value("paste/askExp", false).toBool();

    gmgr = new QNetworkAccessManager(this);
    QObject::connect(gmgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileUploadComplete(QNetworkReply*)));
    QObject::connect(sets.get(), SIGNAL(settingsUpdated()), this, SLOT(settingsUpdated()));
}

/// Generates the base URL string (without a trailing slash) for the object's server for use in building
/// API endpoints.
QString HorusUploader::build_base_req_string(){
    QString reqURL("");
    reqURL += "http";
    if(sslOn){ reqURL += "s"; }
    reqURL += "://" + SERVER_URL + ":" + SERVER_PORT;
    return reqURL;
}

/// Does the same thing as build_base_req_string but abstracts away the
/// port number as it shouldn't be required to open the URL in the browser.
QString HorusUploader::build_base_browser_string(){
    QString reqURL("");
    reqURL += "http";
    if(sslOn){ reqURL += "s"; }
    reqURL += "://" + SERVER_URL;
    return reqURL;
}

/// [DEPRECATED] Appends the authentication string required by the server for versions below 1.3.1. Do not use.
void HorusUploader::append_auth_str(QString * req, bool firstParam){
    if(firstParam){
        req->append("?license_key=" + AUTH_TOKEN);
    }else{
        req->append("&license_key=" + AUTH_TOKEN);
    }
}

/// Catch all for uploading images and videos. Internal use only (only called by member functions that the client
/// class invokes to upload each individual media type).
void HorusUploader::upload(bool isVideo, QString filename){
    filename = filename.trimmed();
    QFile toUpload(filename);
    if(toUpload.exists()){
        toUpload.open(QIODevice::ReadOnly);
        QEventLoop el;
        QNetworkAccessManager nMgr;
        QObject::connect(&nMgr, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));


        QString reqURL;
        ExpirationDuration dur;
        if(isVideo) {
            reqURL = build_base_req_string().append("/video/new");
        }else{
            reqURL = build_base_req_string().append("/image/new");
        }


        if((isVideo && ask_exp_video) || (!isVideo && ask_exp_image)) {
            dur = getExpirationDuration();
        }

        if((isVideo && ask_title_video) || (!isVideo && ask_title_image)) {
            bool ok;
            QInputDialog * dialog = new QInputDialog();
            dialog->move(QApplication::desktop()->screenGeometry().center() - dialog->pos()/2); // center it

            QString _title = dialog->getText(NULL, "Resource Title", "Enter a title for the resource.", QLineEdit::Normal, "Horus Upload", &ok);
            dialog->deleteLater();
            reqURL.append("/");
            reqURL.append(_title.toLatin1());
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


/// Uploads a single file to the server (which in turn stores it on s3) after cleaning the URL.
/// Internal use for the same reasons as previous.
void HorusUploader::uploadFile(QString filename){
    filename = filename.trimmed();
    QFile toUpload(filename);
    if(toUpload.exists()){
        // Open file, initialize request string, allocate event loop
        QFileInfo toUploadInfo(filename);
        toUpload.open(QIODevice::ReadOnly);

        if(toUpload.size() > 10485760){ // 10mb
            //emit uploadFailed("File: " + filename + " larger than 10MB! Upload aborted.");
            //return;
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

        QNetworkReply *reply = gmgr->post(req, postData);
        QObject::connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(uploadProgressSlot(qint64,qint64)));
    }
}

/// Uploads pastes. Generic escaping is processed before the request is sent and the server will HTML escape any pastes
/// that contain browser-executable code like javascript.
void HorusUploader::sendText(QString text){
    QEventLoop el;
    QNetworkAccessManager nMgr;
    QObject::connect(&nMgr, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));
    QNetworkRequest req;
    QString reqURL = build_base_req_string().append("/paste/new");
    req.setRawHeader(QString("x-api-key").toUtf8(), AUTH_TOKEN.toUtf8());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString title = "Horus Paste";
    if(ask_title_paste){
        bool ok;
        QInputDialog * dialog = new QInputDialog();
        dialog->move(QApplication::desktop()->screenGeometry().center() - dialog->pos()/2); // center it
        QString _title = dialog->getText(NULL, "Paste Title", "Enter a title for the image.", QLineEdit::Normal, "Horus Paste", &ok);
        dialog->deleteLater();
        title = _title;
    }
    ExpirationDuration dur;
    if(ask_exp_paste) {
        dur = getExpirationDuration();
    }

    req.setUrl(QUrl(QString("").append(reqURL)));
    QJsonDocument doc = get_paste_form(title, text);
    QByteArray postData = doc.toJson(QJsonDocument::Compact);

    gmgr->post(req, postData);
}

void HorusUploader::fileUploadComplete(QNetworkReply *reply){
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 201){
        QString location = build_base_req_string();
        location += QString(reply->rawHeader("Location"));
        emit uploadCompleted(location);

    }else{
        QUrlQuery q(reply->request().url());
        if(statusCode == 413) {
            emit uploadFailed("Upload failed: file too large.");
        } else {
            emit uploadFailed("Upload failed. Check your license key!");
        }
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
    QString reqURL = build_base_req_string().append("/dist/version/").append(PLATFORM);
    QEventLoop el;
    QNetworkAccessManager manager;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));
    QNetworkRequest req(QUrl(QString("").append(reqURL)));
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::RedirectPolicy::NoLessSafeRedirectPolicy);
    req.setMaximumRedirectsAllowed(MAX_REDIRECTS);
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

/// Need to add error handling for this.
ExpirationDuration HorusUploader::getExpirationDuration()
{
    ExpirationSelectionDialog diag;
    diag.exec();
    return diag.GetResult(); // this will fail if they just exit the window
}

/// Just reinitializes everything done in the constructor to prevent the need to reinstantiate.
void HorusUploader::settingsUpdated()
{
    sets->sync();
    SERVER_URL = sets->value("auth/serverURL", "").toString();
    SERVER_PORT = sets->value("auth/serverPort", "").toString();
    AUTH_TOKEN = sets->value("auth/authToken", "").toString();
    sslOn = (SERVER_PORT == "443");

    ask_title_image = sets->value("image/askTitle", false).toBool();
    ask_title_video = sets->value("video/askTitle", false).toBool();
    ask_title_paste = sets->value("paste/askTitle", false).toBool();

    ask_exp_image = sets->value("image/askExp", false).toBool();
    ask_exp_video = sets->value("video/askExp", false).toBool();
    ask_exp_paste = sets->value("paste/askExp", false).toBool();
}
