#ifndef FILEDROPPER_H
#define FILEDROPPER_H

#include <horussettings.h>
#include <net/horusuploader.h>

#include <memory>

#include <QObject>
#include <QSettings>
#include <QClipboard>
#include <QTemporaryDir>

class FileDropper : public QObject
{
    Q_OBJECT
public:
    explicit FileDropper(std::shared_ptr<HorusSettings> sets, std::shared_ptr<HorusUploader>, QObject *parent = nullptr);

signals:

public slots:
    void fileDropped();
    void cleanupTempDir();

private:
    void runUpload(QString files);

    std::shared_ptr<HorusSettings> settings;
    std::shared_ptr<HorusUploader> uploader;
    QTemporaryDir * tempDir;
};

#endif // FILEDROPPER_H
