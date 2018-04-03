#include "uploadfileswindow.h"
#include "ui_uploadfileswindow.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QKeyEvent>
#include <QLabel>
#include <QStatusBar>
#include <QMimeData>
#include <QProgressBar>
#include <QClipboard>
#include <QApplication>
#include <QTextStream>
#include <QTemporaryDir>
#include <QMessageBox>
#include <QProcess>

UploadFilesWindow::UploadFilesWindow(QStringList files, std::shared_ptr<HorusSettings> sets,
                                     std::shared_ptr<HorusUploader> upl, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UploadFilesWindow),
    ctrlHeld(false),
    settings(sets),
    uploader(upl)
{
    setAcceptDrops(true);
    ui->setupUi(this);

    ui->pbCurrentFile->setMinimum(0);
    ui->pbCurrentFile->setMaximum(100);
    ui->pbCurrentFile->setValue(0);
    filelist = files;

    zipSwitch = new ToggleSwitch(this);
    zipSwitch->setOn(settings->value("file/multipleUpload", "") == "zip");
    ui->layoutQueueLabel->addWidget(zipSwitch);

    settings->sync();
    url = settings->value("auth/serverURL").toString();
    port = settings->value("auth/serverPort").toString();
    token = settings->value("auth/authToken").toString();
    usessl = port == "443";
    QObject::connect(settings.get(), SIGNAL(settingsUpdated()), this, SLOT(setsUpdated()));

    QObject::connect(uploader.get(), SIGNAL(uploadCompleted(QString)), this, SLOT(fileUploaded(QString)));
    QObject::connect(uploader.get(), SIGNAL(uploadFailed(QString)), this, SLOT(fileUploadFailed(QString)));
    QObject::connect(uploader.get(), SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(updateProgressBar(qint64,qint64)));

    QObject::connect(ui->lvCompleted, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(selectedCompleteChanged(QListWidgetItem*,QListWidgetItem*)));

    for(int i = 0; i < files.size(); i++){
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(files.at(i));
        ui->lvFiles->insertItem(i, newItem);
    }
    show();
    ui->sb->showMessage("Ready. Drag-and-drop or paste files to upload.");
    if(ui->lvFiles->count() > 0) startNextFile();
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
        ui->sb->showMessage("Uploads complete.");
        emit complete();
    }
}

void UploadFilesWindow::dragEnterEvent(QDragEnterEvent *evt){
    evt->acceptProposedAction();
}

void UploadFilesWindow::dragLeaveEvent(QDragLeaveEvent *evt){
    evt->accept();
}

void UploadFilesWindow::dropEvent(QDropEvent *evt){
    evt->accept();
    const QMimeData* mimeData = evt->mimeData();

    processMimeData(mimeData);
}

void UploadFilesWindow::keyPressEvent(QKeyEvent *evt){
    if(evt->key() == Qt::Key_Control && evt->type() == QEvent::KeyPress) {
        ctrlHeld = true;
    }else if(evt->key() == Qt::Key_Control && evt->type() == QEvent::KeyRelease) {
        ctrlHeld = false;
    }else if(ctrlHeld && evt->key() == Qt::Key_V) {
        QClipboard *clip = QApplication::clipboard();
        const QMimeData *data = clip->mimeData();
        processMimeData(data);
    }
    evt->accept();
}


void UploadFilesWindow::processMimeData(const QMimeData* mimeData){
    if(mimeData->hasUrls()){
        bool startNext = ui->lvFiles->count() == 0;
        QList<QUrl> urls = mimeData->urls();

        // One file -> no zipping no matter what
        // Otherwise check setting
        if(urls.size() == 1 || !zipSwitch->getOn()) {
            for(int i = 0; i < urls.size(); i++){
                QListWidgetItem *newItem = new QListWidgetItem;
                newItem->setText(urls.at(i).toLocalFile());
                ui->lvFiles->insertItem(i, newItem);
            }
        } else { // zip first
            QString zipString = "";
            QString archive_name = getArchiveName();
            QTemporaryDir tempD;
            tempD.setAutoRemove(false);
#ifdef Q_OS_WIN
            zipString += "bin/zip";
#else
            zipString += "zip";
#endif
            zipString += " -r9 -j \"" + tempD.path() + "/" + archive_name + ".zip\" ";
            for(int i = 0; i < urls.size(); i++){
                zipString += "\"" + urls.at(i).toLocalFile().trimmed() + "\" ";
            }

            int exitCode = QProcess::execute(zipString);

            if(exitCode != 0){
                QMessageBox *box = new QMessageBox();
                box->setWindowIcon(QIcon(":/res/horus.png"));
                box->setText(QString("Unable to zip dropped files. zip exited with exit code ")
                             + QString::number(exitCode));
                box->show();
            } else {
                QListWidgetItem *newItem = new QListWidgetItem;
                newItem->setText(QString(tempD.path() + "/" + archive_name + ".zip"));
                ui->lvFiles->insertItem(0, newItem);
            }
        }
        // If there are files in the lv, we don't want to cancel an upload.
        if(startNext) startNextFile();
    }
}

void UploadFilesWindow::setsUpdated() {
    settings->sync();
    url = settings->value("auth/serverURL").toString();
    port = settings->value("auth/serverPort").toString();
    token = settings->value("auth/authToken").toString();
    usessl = port == "443";

}

QString UploadFilesWindow::getArchiveName()
{
    const QString alphabet("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890");
    QString result;
    for(int i = 0; i < 6; i++){
        int index = qrand() % alphabet.length();
        QChar c = alphabet.at(index);
        result.append(c);
    }
    return result;
}
