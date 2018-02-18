#include "hotkeyselector.h"
#include <QTextStream>

HotkeySelector::HotkeySelector(QWidget *parent) :
    QWidget(parent),
    isModifying(false),
    ui(new Ui::HotkeySelector)
{
    ui->setupUi(this);
    connect(ui->lineEdit, SIGNAL(focusIn()), this, SLOT(leFocused()));
    connect(ui->lineEdit, SIGNAL(focusOut()), this, SLOT(leDefocused()));
    connect(ui->btnCancel, SIGNAL(pressed()), this, SLOT(cancelPressed()));
}

void HotkeySelector::keyPressEvent(QKeyEvent *evt)
{
    if(evt->isAutoRepeat()) return; // do nothing on repeats.

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
    }
}

void HotkeySelector::keyReleaseEvent(QKeyEvent *evt)
{


}

void HotkeySelector::updateLE()
{
    int mod1 = 0;
    int mod2 = 0;
    int mod3 = 0;

    switch(modifier){
    case Qt::ShiftModifier:
        mod1 = Qt::Key_Shift;
        break;
    case Qt::AltModifier:
        mod1 = Qt::Key_Alt;
        break;
    case Qt::MetaModifier:
        mod1 = Qt::Key_Meta;
        break;
    case Qt::ControlModifier:
        mod1 = Qt::Key_Control;
        break;

    case (Qt::ShiftModifier|Qt::AltModifier):
        mod1 = Qt::Key_Shift;
        mod2 = Qt::Key_Alt;
        break;
    case (Qt::ControlModifier|Qt::AltModifier):
        mod1 = Qt::Key_Control;
        mod2 = Qt::Key_Alt;
        break;
    case (Qt::ControlModifier|Qt::ShiftModifier):
        mod1 = Qt::Key_Control;
        mod2 = Qt::Key_Shift;
        break;

    case (Qt::ControlModifier|Qt::AltModifier|Qt::ShiftModifier):
        mod1 = Qt::Key_Control;
        mod2 = Qt::Key_Alt;
        mod3 = Qt::Key_Shift;
    }

    s = QKeySequence(modifier, key);
    ui->lineEdit->setText(s.toString(QKeySequence::NativeText).replace(", ", ""));
}

void HotkeySelector::leFocused()
{
    ui->btnCancel->setEnabled(true);
}

void HotkeySelector::leDefocused()
{
}

void HotkeySelector::cancelPressed()
{
    if(s.isEmpty()) {
        ui->lineEdit->clear();
    } else {
        ui->lineEdit->setText(s.toString(QKeySequence::NativeText).replace(", ", ""));
    }
}
