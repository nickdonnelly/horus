#ifndef TEXTDROPPER_H
#define TEXTDROPPER_H

#include <horussettings.h>
#include "horusuploader.h"
#include <QObject>
#include <QSettings>

class TextDropper : public QObject
{
    Q_OBJECT
public:
    explicit TextDropper(HorusSettings * sets, QObject *parent = 0);

signals:
    void complete(QString url);
    void failure(QString reason);

public slots:
    void textDropped();
    void uploadComplete(QString url);
    void uploadFail(QString reason);
    void setsUpdated();

private:
    HorusSettings * settings;
    HorusUploader * uploader;
    QString url, port, token;
    bool usessl;

    void upload(QString text);
};

#endif // TEXTDROPPER_H
