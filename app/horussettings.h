#ifndef HORUSSETTINGS_H
#define HORUSSETTINGS_H

#include <QSettings>

class HorusSettings : public QSettings {
    Q_OBJECT
public:
    explicit HorusSettings();
    void notifyUpdated();

signals:
    void settingsUpdated();

};

#endif // HORUSSETTINGS_H
