#include "hotkeyselectorlineedit.h"

HotkeySelectorLineEdit::HotkeySelectorLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
}


void HotkeySelectorLineEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    emit focusIn();
}

void HotkeySelectorLineEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit focusOut();
}
