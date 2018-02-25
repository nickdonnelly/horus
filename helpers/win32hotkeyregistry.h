#ifndef WIN32HOTKEYREGISTRY_H
#define WIN32HOTKEYREGISTRY_H

#include <QObject>
#include <QKeySequence>
#include <QAbstractNativeEventFilter>
#include <windows.h>

class Win32HotkeyRegistry : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit Win32HotkeyRegistry(int winId, QObject *parent = 0);

    void registerHotkey(int identifier, QKeySequence seq);
    void unRegisterHotkey(int identifier);
    char mapFunctionKey(QString key);

signals:
    void hotkeyPressed(int identifier);

public slots:

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *msg, long *result);
private:
    int windowId;
};

#endif // WIN32HOTKEYREGISTRY_H
