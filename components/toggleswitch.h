#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

#include <QObject>
#include <QSlider>

class ToggleSwitch : public QSlider
{
    Q_OBJECT
public:
    explicit ToggleSwitch(QWidget *parent = 0);
    bool getOn();
    void setOn(bool on);

private slots:
    void sRelease();
    void sPress();

private:
    bool isPressed;
    bool isOn;
};

#endif // TOGGLESWITCH_H
