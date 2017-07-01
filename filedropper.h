#ifndef FILEDROPPER_H
#define FILEDROPPER_H

#include <QObject>
#include <QSettings>
#include <QClipboard>

class FileDropper : public QObject
{
    Q_OBJECT
public:
    explicit FileDropper(QSettings * sets, QObject *parent = nullptr);

signals:

public slots:
    void fileDropped();

private:
    void runUpload(QString files);

    QSettings * settings;
};

#endif // FILEDROPPER_H
