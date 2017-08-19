#include "textdropper.h"
#include <QClipboard>
#include <QApplication>
#include <QIcon>
#include <QMessageBox>

TextDropper::TextDropper(QSettings * sets, QObject *parent) : QObject(parent)
{
    settings = sets;

    url = settings->value("serverURL").toString();
    port = settings->value("serverPort").toString();
    token = settings->value("authToken").toString();
    usessl = settings->value("useSSL").toBool();

    uploader = new HorusUploader(url, port, token, usessl);

    // Since we are uploading text (lightweight), we will ignore the progress
    // indicator signal for now and only connect succcess/failure signals.
    QObject::connect(uploader, SIGNAL(uploadCompleted(QString)), this, SLOT(uploadComplete(QString)));
    QObject::connect(uploader, SIGNAL(uploadFailed(QString)), this, SLOT(uploadFail(QString)));
}

void TextDropper::textDropped(){
    QClipboard *clip = QApplication::clipboard();
    QString text = clip->text();

    if(text.trimmed().isEmpty()){
        QMessageBox *box = new QMessageBox();
        box->setWindowIcon(QIcon(":/res/paste.png"));
        box->setText("You have no text on your clipboard!");
        box->show();
    }else{
        uploader->sendText(text);
    }
}

// For manual use (if you don't want to use a signal for whatever reason)
void TextDropper::upload(QString text){
    uploader->sendText(text);
}

void TextDropper::uploadComplete(QString url) {
    emit complete(url);
}

void TextDropper::uploadFail(QString reason) {
    emit failure(reason);
}
