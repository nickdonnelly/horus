#ifndef NATIVEEVENTFILTER_H
#define NATIVEEVENTFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>

class NativeEventFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit NativeEventFilter(QObject *parent = 0);

    bool nativeEventFilter(const QByteArray &eventType, void *msg, long *result);
    void setShortcut();
    void unsetShortcut();

signals:
    void activated();

public slots:

};

#endif // NATIVEEVENTFILTER_H
