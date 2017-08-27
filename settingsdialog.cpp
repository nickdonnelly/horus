#include "settingsdialog.h"
#include <horus.h>
#include "ui_settingsdialog.h"
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QSettings>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QSettings * settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    // Get rid of stupid help button
    setWindowFlags(windowFlags() & ~(Qt::WindowContextHelpButtonHint));
    connect(ui->btnAbout, SIGNAL(pressed()), this, SLOT(aboutClick()));
    // Load the settings into the UI.
    sets = settings;
    sets->sync();
    updateUIFromSettings(sets);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}


void SettingsDialog::updateUIFromSettings(QSettings * settings){
    ui->cbOpenInBrowser->setChecked(settings->value("openInBrowser", true).toBool());
    ui->cbUpload->setChecked(settings->value("uploadImages", true).toBool());
    ui->cbAskTitle->setChecked(settings->value("askTitle", true).toBool());
    ui->tbServerURL->setText(settings->value("serverURL", "").toString());
    ui->tbServerPort->setText(settings->value("serverPort", "").toString());
    ui->tbAuthToken->setText(settings->value("authToken", "").toString());
    ui->cbSSL->setChecked(settings->value("useSSL", false).toBool());
    QString scopyMode = settings->value("copyMode", "none").toString();
    if(scopyMode == "url"){
        ui->rbCopyURL->setChecked(true);
    }else if(scopyMode == "image"){
        ui->rbCopyImage->setChecked(true);
    }else{
        ui->rbNoCopy->setChecked(true);
    }

    QString uploadMode = settings->value("uploadMode", "standalone").toString();
    if(uploadMode == "zip"){
        ui->rbUploadZip->setChecked(true);
    }else{
        ui->rbUploadIndividual->setChecked(true);
    }
}

void SettingsDialog::saveSettings(QSettings * settings){
    settings->setValue("openInBrowser", ui->cbOpenInBrowser->isChecked());
    settings->setValue("uploadImages", ui->cbUpload->isChecked());
    settings->setValue("askTitle", ui->cbAskTitle->isChecked());

    if(ui->rbNoCopy->isChecked()){
        settings->setValue("copyMode", "none");
    }else if(ui->rbCopyURL->isChecked()){
        settings->setValue("copyMode", "url");
    }else if(ui->rbCopyImage->isChecked()){
        settings->setValue("copyMode", "image");
    }else{
        settings->setValue("copyMode", "none");
    }

    if(ui->rbUploadIndividual->isChecked()){
        settings->setValue("uploadMode", "standalone");
    }else{
        settings->setValue("uploadMode", "zip");
    }

    settings->setValue("serverURL", ui->tbServerURL->text());
    settings->setValue("serverPort", ui->tbServerPort->text());
    settings->setValue("authToken", ui->tbAuthToken->text());
    settings->setValue("useSSL", ui->cbSSL->isChecked());

    settings->sync();
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
   if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Ok)){
       saveSettings(sets);
       close();
   }else{
       close();
   }
}

void SettingsDialog::aboutClick(){
    QMessageBox * aboutBox = new QMessageBox(this);
    aboutBox->setWindowIcon(QIcon(":/res/horus.png"));
    aboutBox->setIcon(QMessageBox::Information);
    aboutBox->setText("Horus © 2017.\n"
                      "Version number: " + Horus::HORUS_VERSION
                      + "\nContext menu icons courtesy of EmojiOne.");
    aboutBox->addButton("OK", QMessageBox::AcceptRole);
    QAbstractButton * changelogBtn = aboutBox->addButton("View Changelogs", QMessageBox::HelpRole);
    aboutBox->exec();

    if(aboutBox->clickedButton()== changelogBtn) {
        QDesktopServices::openUrl(QString("https://horus.donnelly.cc/changelogs"));
    }
}
