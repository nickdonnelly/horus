#include <filedropper.h>
#include <window/uploadfileswindow.h>
#include <net/horusuploader.h>

#include <QTemporaryDir>
#include <QString>
#include <QProcess>
#include <QIcon>
#include <QApplication>
#include <QMimeData>
#include <QTextStream>
#include <QMessageBox>
#include <QSettings>

FileDropper::FileDropper(std::shared_ptr<HorusSettings> sets, std::shared_ptr<HorusUploader> upl, QObject *parent)
    : QObject(parent),
      settings(sets),
      uploader(upl)
{
}


void FileDropper::runUpload(QString files){
    QStringList eachfile = files.split("\n");
    settings->sync();
    bool isZip = (settings->value("file/multipleUpload", "nozip").toString() == "zip") && eachfile.length() > 1;

    if(isZip){
        QString zipString = "";
        QString archive_name = UploadFilesWindow::getArchiveName();
        tempDir = new QTemporaryDir();
        tempDir->setAutoRemove(false);
#ifdef Q_OS_WIN
        zipString += "bin/zip";
#else
        zipString += "zip";
#endif
        zipString += " -r9 -j \"" + tempDir->path() + "/" + archive_name + ".zip\" ";

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
            eachfile.push_back(QString(tempDir->path() + "/" + archive_name + ".zip"));
        }
    }
    // invoke new window with eachfile
    UploadFilesWindow * win = new UploadFilesWindow(eachfile, settings, uploader);
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
        settings->sync();
#ifdef Q_OS_WIN
        actualData.remove("file:///"); // yay windows
#else
        actualData.remove("file://"); // Keep the first slash so keep the path from root.
#endif

        QMessageBox *box = new QMessageBox();
        box->setWindowIcon(QIcon(":/res/horus.png"));
        box->setIconPixmap(QPixmap(":/res/filedrop.png"));
        box->addButton(QMessageBox::Yes);
        box->addButton(QMessageBox::No);
        if(settings->value("file/multipleUploader", "nozip").toString() == "zip"){
            box->setText("Do you want to zip and upload the following files from the clipboard?\n" + actualData);
        }else{
            box->setText("Do you want to upload the following files from the clipboard?\n" + actualData);
        }
        int result = box->exec();

        if(result == QMessageBox::Yes){
            runUpload(actualData);
        } else {
            openEmptyWindow();
        }
    }else{
        openEmptyWindow();
    }
}

void FileDropper::openEmptyWindow()
{
    QStringList dummyList;
    UploadFilesWindow * win = new UploadFilesWindow(dummyList, settings, uploader);
    win->setWindowIcon(QIcon(":/res/dropfile.png"));
    win->show();
}
