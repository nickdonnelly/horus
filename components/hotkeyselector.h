#ifndef HOTKEYSELECTOR_H
#define HOTKEYSELECTOR_H

#include <ui_hotkeyselector.h>
#include <QWidget>
#include <QKeyEvent>
#include <QVector>

class HotkeySelector : public QWidget
{
    Q_OBJECT
public:
    explicit HotkeySelector(QString label, QWidget *parent = nullptr);

    void setKeySequence(QKeySequence s);

    QString getValueString();
    QKeySequence getValue();

signals:
    void keyChanged(int m, int k);

public slots:
    void leFocused();
    void leDefocused();
    void cancelPressed();
    void clearPressed();

protected:
    void keyPressEvent(QKeyEvent *evt);
    void keyReleaseEvent(QKeyEvent *evt);


private:
    Ui::HotkeySelector *ui;
    QKeySequence s;
    int modifier;
    int key;

    bool isModifying;
    bool cancelFocused;

    void updateLE();
};

#endif // HOTKEYSELECTOR_H
