#ifndef FILEDROPPER_H
#define FILEDROPPER_H

#include <horussettings.h>
#include <QObject>
#include <QSettings>
#include <QClipboard>
#include <QTemporaryDir>

class FileDropper : public QObject
{
    Q_OBJECT
public:
    explicit FileDropper(HorusSettings * sets, QObject *parent = nullptr);

signals:

public slots:
    void fileDropped();
    void cleanupTempDir();

private:
    void runUpload(QString files);

    HorusSettings * settings;
    QTemporaryDir * tempDir;
};

#endif // FILEDROPPER_H
