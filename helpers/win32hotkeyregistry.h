#ifndef WIN32HOTKEYREGISTRY_H
#define WIN32HOTKEYREGISTRY_H

#include <QObject>

class Win32HotkeyRegistry : public QObject
{
    Q_OBJECT
public:
    explicit Win32HotkeyRegistry(QObject *parent = 0);

signals:

public slots:
};

#endif // WIN32HOTKEYREGISTRY_H