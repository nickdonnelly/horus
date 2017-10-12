#include "expirationselectiondialog.h"
#include "ui_expirationselectiondialog.h"
#include <QLabel>
#include <QSlider>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextStream>

ExpirationSelectionDialog::ExpirationSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExpirationSelectionDialog)
{
    ui->setupUi(this);
    sel = Duration::Hour;
    ui->slider->setMinimum(1);
    ui->slider->setMaximum(24);
    ui->slider->setValue(12);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setIconSize(QSize(0,0));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setIconSize(QSize(0,0));
    connect(ui->btnMinutes, SIGNAL(clicked(bool)), this, SLOT(minPressed(bool)));
    connect(ui->btnHours, SIGNAL(clicked(bool)), this, SLOT(hourPressed(bool)));
    connect(ui->btnDays, SIGNAL(clicked(bool)), this, SLOT(dayPressed(bool)));
    connect(ui->slider, SIGNAL(valueChanged(int)), this, SLOT(update_ui(int)));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(pressed()), this, SLOT(confirmCurrent()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(pressed()), this, SLOT(close()));
    selectedTime = QDateTime::currentDateTimeUtc(); // Server uses UTC, before you ask.
    update_ui(ui->slider->value());
}

ExpirationSelectionDialog::~ExpirationSelectionDialog()
{
    delete ui;
}

void ExpirationSelectionDialog::minPressed(bool c)
{
    if(c) {
        sel = Duration::Minute;
        update_ui(ui->slider->value() < 60 ? ui->slider->value() : 60);
    }
}

void ExpirationSelectionDialog::hourPressed(bool c)
{
    if(c) {
        sel = Duration::Hour;
        update_ui(ui->slider->value() < 24 ? ui->slider->value() : 24);
    }
}

void ExpirationSelectionDialog::dayPressed(bool c)
{
    if(c){
        sel = Duration::Day;
        update_ui(ui->slider->value() < 30 ? ui->slider->value() : 30);
    }
}

void ExpirationSelectionDialog::update_ui(int val)
{
    QString enumStr;
    switch(sel) {
    case Duration::Day:
        enumStr = "Days";
        ui->slider->setMaximum(30);
        break;
    case Duration::Hour:
        enumStr = "Hours";
        ui->slider->setMaximum(24);
        break;
    case Duration::Minute:
        enumStr = "Minutes";
        ui->slider->setMaximum(60);
        break;
    }
    ui->lblNum->setText(QString::number(val) + " " + enumStr);
}

void ExpirationSelectionDialog::confirmCurrent()
{
    accept();
}

ExpirationDuration ExpirationSelectionDialog::GetResult() {
    ExpirationDuration d;
    d.dur = sel;
    d.multiplier = ui->slider->value();

    return d;
}
