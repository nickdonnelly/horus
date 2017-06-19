#ifndef HORUSUPLOADER_H
#define HORUSUPLOADER_H

#include <QObject>

class HorusUploader : public QObject
{
    Q_OBJECT
public:
    HorusUploader(QString serverURL, QString serverPort, QString authToken, bool useSSL);

    void upload(bool isVideo, QString filename);
    void checkLatestVersion();

private:
    QString SERVER_URL, SERVER_PORT, AUTH_TOKEN;
    bool sslOn;

signals:
    void uploadCompleted(QString url);
    void uploadFailed(QString reason);
    void version(QString version);
};

#endif // HORUSUPLOADER_H
