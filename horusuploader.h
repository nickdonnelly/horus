#ifndef HORUSUPLOADER_H
#define HORUSUPLOADER_H

#include <QObject>

class HorusUploader : public QObject
{
    Q_OBJECT
public:
    HorusUploader(QString serverURL, QString serverPort, QString authToken, bool useSSL);

    void upload(bool isVideo, QString filename);

private:
    QString SERVER_URL, SERVER_PORT, AUTH_TOKEN;
    bool sslOn;

signals:
    void uploadCompleted(QString url);
};

#endif // HORUSUPLOADER_H
