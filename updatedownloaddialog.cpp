#include "updatedownloaddialog.h"
#include "ui_updatedownloaddialog.h"

UpdateDownloadDialog::UpdateDownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateDownloadDialog)
{
    ui->setupUi(this);
}

UpdateDownloadDialog::~UpdateDownloadDialog()
{
    delete ui;
}

void UpdateDownloadDialog::downloadUpdate(){

}
