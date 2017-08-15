#ifndef UPLOADFILESWINDOW_H
#define UPLOADFILESWINDOW_H

#include <horusuploader.h>
#include <QMainWindow>
#include <QStringList>
#include <QListWidgetItem>
#include <QSettings>

namespace Ui {
class UploadFilesWindow;
}

class UploadFilesWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UploadFilesWindow(QStringList files, QSettings * sets, QWidget *parent = 0);
    ~UploadFilesWindow();

signals:
    void complete();

private slots:
    void fileUploaded(QString fileURL);
    void fileUploadFailed(QString reason);
    void updateProgressBar(qint64 sent, qint64 total);
    void selectedCompleteChanged(QListWidgetItem *current, QListWidgetItem*);

private:
    Ui::UploadFilesWindow *ui;
    QStringList filelist;
    QString url, port, token;
    bool usessl;
    QSettings * settings;
    HorusUploader * uploader;
    void startNextFile();
};

#endif // UPLOADFILESWINDOW_H
