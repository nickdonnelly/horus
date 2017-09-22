#ifndef MODELSERIALIZE_H
#define MODELSERIALIZE_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

class ModelSerialize
{
public:
    static QJsonDocument get_paste_form(QString title, QString paste_data);

private:
    ModelSerialize();
};

#endif // MODELSERIALIZE_H
