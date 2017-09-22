#include "modelserialize.h"

ModelSerialize::ModelSerialize(){}

QJsonDocument ModelSerialize::get_paste_form(QString title, QString paste_data)
{
    QJsonObject serialized;
    serialized.insert("title", title);
    serialized.insert("paste_data", paste_data);
    serialized.insert("is_expiry", false);

    return QJsonDocument(serialized);
}
