#ifndef HORUSUPLOADER_H
#define HORUSUPLOADER_H

#include <QObject>

class HorusUploader : public QObject
{
    Q_OBJECT
public:
    HorusUploader();

    void upload(QString filename);


signals:
    void uploadCompleted(QString url);
};

#endif // HORUSUPLOADER_H
