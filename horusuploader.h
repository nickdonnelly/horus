#ifndef HORUSUPLOADER_H
#define HORUSUPLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>

class HorusUploader : public QObject
{
    Q_OBJECT
public:
    HorusUploader(QString serverURL, QString serverPort, QString authToken, bool useSSL);
    HorusUploader(QSettings * settings);

    void upload(bool isVideo, QString filename);
    void uploadFile(QString filename);
    void sendText(QString text);
    void checkLatestVersion();
    void resetCreds(QString serverURL, QString serverPort, QString authToken, bool useSSL);
    QString build_base_req_string();
    QString build_base_browser_string();
    QString get_auth_str();

private:
    bool ask_title_image, ask_title_video, ask_title_paste;
    QString SERVER_URL, SERVER_PORT, AUTH_TOKEN;
    QNetworkAccessManager * gmgr;
    QSettings * sets;
    void append_auth_str(QString * req, bool firstParam);
    bool sslOn;

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
