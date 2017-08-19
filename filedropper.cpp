#include "filedropper.h"
#include <uploadfileswindow.h>
#include <horusuploader.h>
#include <QTemporaryDir>
#include <QString>
#include <QProcess>
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
    bool isZip = settings->value("uploadMode", "standalone").toString() == "zip";

    if(isZip){
        QString zipString = "";
        tempDir = new QTemporaryDir();
        tempDir->setAutoRemove(false);
#ifdef Q_OS_WIN
        zipString += "bin/zip";
#else
        zipString += "zip";
#endif
        zipString += " -r9 -j \"" + tempDir->path() + "/temp.zip\" ";

        for(int i = 0; i < eachfile.size(); i++){
            zipString += "\"" + eachfile.at(i).trimmed()  + "\" ";
        }

        int exitCode = QProcess::execute(zipString);

        eachfile.clear();
        if(exitCode != 0){
            QMessageBox *box = new QMessageBox();
            box->setWindowIcon(QIcon(":/res/horus.png"));
            box->setText(QString("Unable to zip files! zip exited with exit code") + QString::number(exitCode));
            box->show();
        }else{
            eachfile.push_back(QString(tempDir->path() + "/temp.zip"));
        }
    }
    // invoke new window with eachfile
    UploadFilesWindow * win = new UploadFilesWindow(eachfile, settings);
    win->setWindowIcon(QIcon(":/res/dropfile.png"));
    win->show();
    if(isZip) connect(win, SIGNAL(complete()), this, SLOT(cleanupTempDir()));
}

void FileDropper::cleanupTempDir(){
    bool deleted = tempDir->remove();
    if(!deleted){
        QMessageBox *box = new QMessageBox();
        box->setWindowIcon(QIcon(":/res/horus.png"));
        box->setText("Could not remove temporary zip directory: " + tempDir->path());
    }
}

// SIGNALS
void FileDropper::fileDropped(){
    QClipboard *clip = QApplication::clipboard();
    const QMimeData *data = clip->mimeData(QClipboard::Clipboard);
    QString actualData = QString(data->data("text/uri-list")).replace("%20", " ").trimmed();
    if(actualData.length() > 0 && actualData.startsWith("file://")){
        actualData.remove("file:///"); // yay windows
        actualData.remove("file://");
        QMessageBox *box = new QMessageBox();
        box->setWindowIcon(QIcon(":/res/horus.png"));
        box->setIconPixmap(QPixmap(":/res/filedrop.png"));
        box->addButton(QMessageBox::Yes);
        box->addButton(QMessageBox::No);
        if(settings->value("uploadMode", "standalone").toString() == "zip"){
            box->setText("Are you sure you want to zip and upload the following files?\n" + actualData);
        }else{
            box->setText("Are you sure you want to upload the following files?\n" + actualData);
        }
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

