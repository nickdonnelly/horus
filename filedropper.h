#ifndef FILEDROPPER_H
#define FILEDROPPER_H

#include <QObject>
#include <QSettings>
#include <QClipboard>
#include <QTemporaryDir>

class FileDropper : public QObject
{
    Q_OBJECT
public:
    explicit FileDropper(QSettings * sets, QObject *parent = nullptr);

signals:

public slots:
    void fileDropped();
    void cleanupTempDir();

private:
    void runUpload(QString files);

    QSettings * settings;
    QTemporaryDir * tempDir;
};

#endif // FILEDROPPER_H
