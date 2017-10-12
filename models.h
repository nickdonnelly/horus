#ifndef MODELS
#define MODELS

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

static QJsonDocument get_paste_form(QString title, QString paste_data){
    QJsonObject serialized;
    serialized.insert("title", title);
    serialized.insert("paste_data", paste_data);
    serialized.insert("is_expiry", false);

    return QJsonDocument(serialized);
}

enum Duration {
    Minute,
    Hour,
    Day
};

struct ExpirationDuration {
    Duration dur;
    int multiplier;
};

#endif // MODELS
