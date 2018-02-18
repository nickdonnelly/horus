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
    explicit HotkeySelector(QWidget *parent = nullptr);

signals:
    void modifierChanged();
    void keyChanged();

public slots:
    void leFocused();
    void leDefocused();
    void cancelPressed();

protected:
    void keyPressEvent(QKeyEvent *evt);
    void keyReleaseEvent(QKeyEvent *evt);


private:
    Ui::HotkeySelector *ui;
    QKeySequence s;
    int modifier;
    int key;

    bool isModifying;

    void updateLE();
};

#endif // HOTKEYSELECTOR_H
