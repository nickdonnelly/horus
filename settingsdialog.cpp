#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    // Get rid of stupid help button
    setWindowFlags(windowFlags() & ~(Qt::WindowContextHelpButtonHint));
    // Load the settings into the UI.
    sets = new QSettings("horus-settings.ini", QSettings::IniFormat);
    updateUIFromSettings(sets);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}


void SettingsDialog::updateUIFromSettings(QSettings * settings){
    ui->cbOpenInBrowser->setChecked(settings->value("openInBrowser", true).toBool());
    ui->cbUpload->setChecked(settings->value("uploadImages", true).toBool());
    ui->cbUploadGif->setChecked(settings->value("uploadGIFs", true).toBool());
    ui->tbServerURL->setText(settings->value("serverURL", "").toString());
    ui->tbServerPort->setText(settings->value("serverPort", "").toString());
    ui->tbAuthToken->setText(settings->value("authToken", "").toString());
    QString scopyMode = settings->value("copyMode", "none").toString();
    if(scopyMode == "url"){
        ui->rbCopyURL->setChecked(true);
    }else if(scopyMode == "image"){
        ui->rbCopyImage->setChecked(true);
    }else{
        ui->rbNoCopy->setChecked(true);
    }
}

void SettingsDialog::saveSettings(QSettings * settings){
    settings->setValue("openInBrowser", ui->cbOpenInBrowser->isChecked());
    settings->setValue("uploadImages", ui->cbUpload->isChecked());
    settings->setValue("uploadGIFs", ui->cbUploadGif->isChecked());

    if(ui->rbNoCopy->isChecked()){
        settings->setValue("copyMode", "none");
    }else if(ui->rbCopyURL->isChecked()){
        settings->setValue("copyMode", "url");
    }else if(ui->rbCopyImage->isChecked()){
        settings->setValue("copyMode", "image");
    }else{
        settings->setValue("copyMode", "none");
    }

    settings->setValue("serverURL", ui->tbServerURL->text());
    settings->setValue("serverPort", ui->tbServerPort->text());
    settings->setValue("authToken", ui->tbAuthToken->text());

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
