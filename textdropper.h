#ifndef TEXTDROPPER_H
#define TEXTDROPPER_H

#include "horusuploader.h"
#include <QObject>
#include <QSettings>

class TextDropper : public QObject
{
    Q_OBJECT
public:
    explicit TextDropper(QSettings * sets, QObject *parent = 0);

signals:
    void complete(QString url);
    void failure(QString reason);

public slots:
    void textDropped();
    void uploadComplete(QString url);
    void uploadFail(QString reason);

private:
    QSettings * settings;
    HorusUploader * uploader;
    QString url, port, token;
    bool usessl;

    void upload(QString text);
};

#endif // TEXTDROPPER_H
