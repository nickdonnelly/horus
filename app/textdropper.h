#ifndef TEXTDROPPER_H
#define TEXTDROPPER_H

#include <net/horusuploader.h>
#include <horussettings.h>

#include <memory>

#include <QObject>
#include <QSettings>

class TextDropper : public QObject
{
    Q_OBJECT
public:
    explicit TextDropper(std::shared_ptr<HorusSettings> sets,
                         std::shared_ptr<HorusUploader> upl, QObject *parent = 0);
public slots:
    void textDropped();
    void setsUpdated();

private:
    std::shared_ptr<HorusSettings> settings;
    std::shared_ptr<HorusUploader> uploader;
    QString url, port, token;
    bool usessl;

    void upload(QString text);
};

#endif // TEXTDROPPER_H
