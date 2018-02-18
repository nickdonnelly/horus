#ifndef NATIVEEVENTFILTER_H
#define NATIVEEVENTFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <X11/Xlib.h>

#ifdef None // XLib defines none and it will interfere with a
#undef None // qt definition of none in an enum if you dont undefine it.
#endif

class NativeEventFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit NativeEventFilter(QObject *parent = 0);

    bool nativeEventFilter(const QByteArray &eventType, void *msg, long *result);
    void setShortcut();
    void unsetShortcut();

    enum KeyModifier {
        NoModifer = 0,
        Alt = Mod1Mask,
        Ctrl = ControlMask,
        WindowsKey = Mod4Mask,
        Shift = ShiftMask
    };

signals:
    void activated();

public slots:

};

#endif // NATIVEEVENTFILTER_H
