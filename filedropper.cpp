#include "filedropper.h"
#include <uploadfileswindow.h>
#include <horusuploader.h>
#include <QIcon>
#include <QApplication>
#include <QMimeData>
#include <QTextStream>
#include <QMessageBox>
#include <QSettings>

FileDropper::FileDropper(QSettings * sets, QObject *parent) : QObject(parent)
{
    settings = sets;
}


void FileDropper::runUpload(QString files){
    QStringList eachfile = files.split("\n");
    // invoke new window with eachfile
    UploadFilesWindow * win = new UploadFilesWindow(eachfile, settings);
    win->setWindowIcon(QIcon(":/res/dropfile.png"));
    win->show();
}

// SIGNALS
void FileDropper::fileDropped(){
    QClipboard *clip = QApplication::clipboard();
    const QMimeData *data = clip->mimeData(QClipboard::Clipboard);
    QString actualData = QString(data->data("text/uri-list")).replace("%20", " ").trimmed();
    if(actualData.length() > 0 && actualData.startsWith("file://")){
        actualData.remove("file://");
        QMessageBox *box = new QMessageBox();
        box->setWindowIcon(QIcon(":/res/horus.png"));
        box->setIconPixmap(QPixmap(":/res/filedrop.png"));
        box->addButton(QMessageBox::Yes);
        box->addButton(QMessageBox::No);
        box->setText("Are you sure you want to upload the following files?\n" + actualData);
        int result = box->exec();

        if(result == QMessageBox::Yes){
            runUpload(actualData);
        }
    }else{
        QMessageBox *box = new QMessageBox();
        box->setWindowIcon(QIcon(":/res/horus.png"));
        box->setIconPixmap(QPixmap(":/res/filedrop.png"));
        box->setText("Found nothing on clipboard to upload!");
        box->exec();
    }
}

