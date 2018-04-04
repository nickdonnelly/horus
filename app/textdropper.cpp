#include "textdropper.h"
#include <QClipboard>
#include <QApplication>
#include <QIcon>
#include <QMessageBox>

TextDropper::TextDropper(std::shared_ptr<HorusSettings> sets,
                         std::shared_ptr<HorusUploader> upl, QObject *parent) : QObject(parent)
{
    settings = sets;
    uploader = upl;

    settings->sync();
    url = settings->value("auth/serverURL").toString();
    port = settings->value("auth/serverPort").toString();
    token = settings->value("auth/authToken").toString();
    usessl = port == "443";

    QObject::connect(settings.get(), SIGNAL(settingsUpdated()), this, SLOT(setsUpdated()));

    // Since we are uploading text (lightweight), we will ignore the progress
    // indicator signal for now and only connect succcess/failure signals.
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

void TextDropper::setsUpdated() {
    url = settings->value("auth/serverURL").toString();
    port = settings->value("auth/serverPort").toString();
    token = settings->value("auth/authToken").toString();
    usessl = port == "443";
}
