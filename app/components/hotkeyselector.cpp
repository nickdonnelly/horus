#include "hotkeyselector.h"
#include <QTextStream>

HotkeySelector::HotkeySelector(QString label, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HotkeySelector),
    isModifying(false),
    cancelFocused(false)
{
    ui->setupUi(this);
    ui->lblTitle->setText(label);
    connect(ui->lineEdit, SIGNAL(focusIn()), this, SLOT(leFocused()));
    connect(ui->lineEdit, SIGNAL(focusOut()), this, SLOT(leDefocused()));
    connect(ui->btnCancel, SIGNAL(pressed()), this, SLOT(cancelPressed()));
    connect(ui->btnClear, SIGNAL(pressed()), this, SLOT(clearPressed()));
}

void HotkeySelector::keyPressEvent(QKeyEvent *evt)
{
    if(cancelFocused || evt->isAutoRepeat()) return; // do nothing on repeats.

    int ekey = evt->key();
    if(evt->modifiers() == 0){
        ui->lineEdit->setText("You must use a modifier (ctrl, alt, etc.).");
    } else if(ekey != Qt::Key_Control &&
              ekey != Qt::Key_Alt &&
              ekey != Qt::Key_Meta &&
              ekey != Qt::Key_Shift) {

        modifier = evt->modifiers();
        key = ekey;
        updateLE();
        emit keyChanged(modifier, key);
    }
}

void HotkeySelector::setLabel(QString label)
{
    ui->lblTitle->setText(label);
}

void HotkeySelector::setKeySequence(QKeySequence a)
{
    if(a.count() > 1) return;
    s = a;
    key = 0; // everything is in modifier because fromString not working with toString output
    modifier = a[0];

    updateLE();
}

QKeySequence HotkeySelector::getValue()
{
    updateLE();
    return s;
}

QString HotkeySelector::getValueString()
{
    QKeySequence a(modifier, key);
    return a.toString(QKeySequence::NativeText);
}

void HotkeySelector::clearPressed()
{
    modifier = 0;
    key = 0;
    ui->lineEdit->setText("");
}

void HotkeySelector::updateLE()
{
    s = QKeySequence(modifier, key);
    ui->lineEdit->setText(s.toString(QKeySequence::NativeText).replace(", ", ""));
}

void HotkeySelector::leFocused()
{
    cancelFocused = false;
}

void HotkeySelector::leDefocused()
{
    cancelFocused = true;
}

void HotkeySelector::cancelPressed()
{
    if(s.isEmpty()) {
        ui->lineEdit->clear();
    } else {
        ui->lineEdit->setText(s.toString(QKeySequence::NativeText).replace(", ", ""));
    }
}
