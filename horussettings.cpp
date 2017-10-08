#include "horussettings.h"
#include <QSettings>


HorusSettings::HorusSettings()
    :QSettings("horus-settings.ini", Format::IniFormat)
{

}

void HorusSettings::notifyUpdated() {
    emit settingsUpdated();
}
