#ifndef HOTKEYSELECTORLINEEDIT_H
#define HOTKEYSELECTORLINEEDIT_H

#include <QObject>
#include <QLineEdit>
#include <QFocusEvent>

class HotkeySelectorLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit HotkeySelectorLineEdit(QWidget *parent = 0);

signals:
    void focusIn();
    void focusOut();


protected:
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
};

#endif // HOTKEYSELECTORLINEEDIT_H
