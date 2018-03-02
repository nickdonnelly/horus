#ifndef UPLOADFILESWINDOW_H
#define UPLOADFILESWINDOW_H

#include <horusuploader.h>
#include <horussettings.h>
#include <components/toggleswitch.h>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QKeyEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
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
    explicit UploadFilesWindow(QStringList files, HorusSettings * sets, QWidget *parent = 0);
    ~UploadFilesWindow();
    static QString getArchiveName();

signals:
    void complete();

private slots:
    void fileUploaded(QString fileURL);
    void fileUploadFailed(QString reason);
    void updateProgressBar(qint64 sent, qint64 total);
    void selectedCompleteChanged(QListWidgetItem *current, QListWidgetItem*);
    void setsUpdated();

private:
    Ui::UploadFilesWindow *ui;
    QStringList filelist;
    QString url, port, token;
    bool usessl;
    bool ctrlHeld;
    HorusSettings * settings;
    HorusUploader * uploader;
    ToggleSwitch * zipSwitch;

    void startNextFile();
    void processMimeData(const QMimeData *mimeData);

protected:
    void dragEnterEvent(QDragEnterEvent *evt);
    void dragLeaveEvent(QDragLeaveEvent *evt);
    void dropEvent(QDropEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
};

#endif // UPLOADFILESWINDOW_H
