#include "uploadfileswindow.h"
#include "ui_uploadfileswindow.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QStatusBar>
#include <QProgressBar>
#include <QClipboard>
#include <QApplication>
#include <QTextStream>

UploadFilesWindow::UploadFilesWindow(QStringList files, QSettings * sets, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UploadFilesWindow)
{
    ui->setupUi(this);
    ui->pbCurrentFile->setMinimum(0);
    ui->pbCurrentFile->setMaximum(100);
    ui->pbCurrentFile->setValue(0);
    settings = sets;
    filelist = files;

    url = settings->value("serverURL").toString();
    port = settings->value("serverPort").toString();
    token = settings->value("authToken").toString();
    usessl = settings->value("useSSL").toBool();

    uploader = new HorusUploader(url, port, token, usessl);
    QObject::connect(uploader, SIGNAL(uploadCompleted(QString)), this, SLOT(fileUploaded(QString)));
    QObject::connect(uploader, SIGNAL(uploadFailed(QString)), this, SLOT(fileUploadFailed(QString)));
    QObject::connect(uploader, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(updateProgressBar(qint64,qint64)));

    QObject::connect(ui->lvCompleted, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(selectedCompleteChanged(QListWidgetItem*,QListWidgetItem*)));

    for(int i = 0; i < files.size(); i++){
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(files.at(i));
        ui->lvFiles->insertItem(i, newItem);
    }
    show();
    ui->sb->showMessage("Ready");
    startNextFile();
}

UploadFilesWindow::~UploadFilesWindow()
{
    delete ui;
}

void UploadFilesWindow::fileUploaded(QString fileURL){
    QListWidgetItem *savedItem = new QListWidgetItem;
    savedItem->setText(fileURL);
    ui->lvCompleted->insertItem(0, savedItem);

    startNextFile();
}

void UploadFilesWindow::updateProgressBar(qint64 sent, qint64 total){
    if(total == 0){
        ui->pbCurrentFile->setValue(100);
    }else{
        ui->pbCurrentFile->setValue(100 * sent / total);
    }
}

void UploadFilesWindow::fileUploadFailed(QString reason){
    QListWidgetItem *failedItem = new QListWidgetItem;
    failedItem->setText(reason);
    ui->lvCompleted->insertItem(0, failedItem);
    startNextFile();
}

void UploadFilesWindow::selectedCompleteChanged(QListWidgetItem *current, QListWidgetItem*){
    QClipboard *clip = QApplication::clipboard();
    clip->setText(current->text().trimmed());
    ui->sb->showMessage("Copied " +  current->text().trimmed() + " to clipboard");

    // Notify user of copy? Status bars or something?
}


void UploadFilesWindow::startNextFile(){
    if(ui->lvFiles->count() > 0){
        QString currentFile("");
        QListWidgetItem * item = ui->lvFiles->takeItem(0);
        currentFile = item->text();
        ui->lblCurrentFile->setText("Current File: " + currentFile);
        delete item;

        ui->sb->showMessage("Uploading " + currentFile);
        uploader->uploadFile(currentFile);
    }else{
        ui->lblCurrentFile->setText("No more files!");
        ui->sb->showMessage("Uploading complete. Click entries in the completed list to copy them to the clipboard.");
    }
}
