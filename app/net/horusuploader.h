#ifndef HORUSUPLOADER_H
#define HORUSUPLOADER_H

#include "models.h"
#include <horussettings.h>

#include <memory>

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>

class HorusUploader : public QObject
{
    Q_OBJECT
public:
    HorusUploader(std::shared_ptr<HorusSettings> settings);

    void upload(bool isVideo, QString filename);
    void uploadFile(QString filename);
    void sendText(QString text);
    void checkLatestVersion();
    void resetCreds(QString serverURL, QString serverPort, QString authToken, bool useSSL);

    QString build_base_req_string();
    QString build_base_browser_string();
    QString get_auth_str();

private:
    const int MAX_REDIRECTS = 2;

    bool ask_title_image, ask_title_video, ask_title_paste;
    bool ask_exp_image, ask_exp_video, ask_exp_paste;
    bool sslOn;

    std::shared_ptr<QSettings> sets;

    QString SERVER_URL, SERVER_PORT, AUTH_TOKEN;
    QNetworkAccessManager * gmgr;

    void append_auth_str(QString * req, bool firstParam);
    ExpirationDuration getExpirationDuration();


public slots:
    void uploadProgressSlot(qint64 bytesSend, qint64 bytesTotal);
    void fileUploadComplete(QNetworkReply *reply);

    void settingsUpdated();

signals:
    void uploadCompleted(QString url);
    void uploadFailed(QString reason);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void version(QString version);
};

#endif // HORUSUPLOADER_H
