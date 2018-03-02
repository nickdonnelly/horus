#include "toggleswitch.h"

ToggleSwitch::ToggleSwitch(QWidget *parent) : QSlider(parent)
{
    this->setMaximum(this->width()); // smooth movement
    this->setOrientation(Qt::Horizontal);
    this->setMinimumWidth(60);
    this->setMinimumHeight(40);
    this->setMaximumWidth(60);
    this->setMaximumHeight(40);
    this->setPageStep(this->maximum());
    this->setSingleStep(this->maximum());
    isOn = false;
    isPressed = false;
    connect(this, SIGNAL(sliderReleased()), this, SLOT(sRelease()));
    connect(this, SIGNAL(sliderPressed()), this, SLOT(sPress()));
}

bool ToggleSwitch::getOn()
{
    return this->value() > this->maximum() / 2;
}

void ToggleSwitch::setOn(bool on)
{
    isOn = on;
    if(isOn){
        this->setValue(this->maximum());
    } else {
        this->setValue(0);
    }

}

void ToggleSwitch::sRelease()
{
    isOn = this->value() > this->maximum() / 2;
    isPressed = false;
    if(isOn) {
        this->setValue(this->maximum());
    } else {
        this->setValue(0);
    }
}

void ToggleSwitch::sPress()
{
    isPressed = true;
}
