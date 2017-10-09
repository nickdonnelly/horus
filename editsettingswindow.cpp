#include "editsettingswindow.h"
#include "ui_editsettingswindow.h"
#include <QFileDialog>
#include <QIntValidator>
#include <QTextStream>
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
    connect(ui->btnDays, SIGNAL(pressed()), this, SLOT(selectLocalFolder()));

    ui->leServerPort->setValidator( new QIntValidator(0, 65535, this) );

}

EditSettingsWindow::~EditSettingsWindow()
{
    delete ui;
}


void EditSettingsWindow::selectLocalFolder()
{
    QFileDialog d(this);
    QString directory = d.getExistingDirectory(this, QString(""), QString("/"));
    QTextStream(stdout) << directory << endl;
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
