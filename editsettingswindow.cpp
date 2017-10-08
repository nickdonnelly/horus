#include "editsettingswindow.h"
#include "ui_editsettingswindow.h"
#include <QStackedWidget>

EditSettingsWindow::EditSettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditSettingsWindow)
{
    ui->setupUi(this);

    connect(ui->rbUploads, SIGNAL(pressed()), this, SLOT(switchPageUpload()));
    connect(ui->rbLocal, SIGNAL(pressed()), this, SLOT(switchPageLocal()));
    connect(ui->rbAuth, SIGNAL(pressed()), this, SLOT(switchPageServer()));
    connect(ui->rbAbout, SIGNAL(pressed()), this, SLOT(switchPageAbout()));
}

EditSettingsWindow::~EditSettingsWindow()
{
    delete ui;
}


void EditSettingsWindow::switchPageUpload()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void EditSettingsWindow::switchPageLocal()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void EditSettingsWindow::switchPageServer()
{
    ui->stackedWidget->setCurrentIndex(2);

}
void EditSettingsWindow::switchPageAbout()
{
    ui->stackedWidget->setCurrentIndex(3);
}
