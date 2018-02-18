#include "nativeeventfilter.h"

#include <QVector>
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xcb/xcb.h>

namespace {
    Display *m_display; // x11 server
    Window m_win; // system window
    int keycode;
    unsigned int modifier; // ctrl, shift, etc..

    QVector<quint32> maskModifiers()
    {
        // These prevent num lock and caps lock from preventing a trigger
        // of the hotkey.
        return QVector<quint32>() << 0 << Mod2Mask << LockMask << (Mod2Mask | LockMask);
    }
}

NativeEventFilter::NativeEventFilter(QObject *parent) : QObject(parent)
{
    m_display = QX11Info::display();
    m_win = DefaultRootWindow(m_display);
}

bool NativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *msg, long *result)
{
    Q_UNUSED(result)

    xcb_key_press_event_t *keyEvent = 0;
    if(eventType == "xcb_generic_event_t") { // we have linux event
        xcb_generic_event_t *evt = static_cast<xcb_generic_event_t *>(msg);


        if((evt->response_type & 127) == XCB_KEY_PRESS) {
            // we are in a key event
            keyEvent = static_cast<xcb_key_press_event_t *>(msg);

            foreach(quint32 pressedModifiers, maskModifiers()) {
                if(keyEvent->state == (modifier | pressedModifiers)
                        && keyEvent->detail == keycode){

                    // the key was pressed!
                    emit activated();
                    return true;
                }
            }
        }
    }

    return false;
}

void NativeEventFilter::setShortcut()
{
    unsetShortcut();
     // TODO: these will be set dynamically
    keycode = XKeysymToKeycode(m_display, XK_E);
    modifier = ControlMask;

    foreach(quint32 pressedModifiers, maskModifiers()) {
        XGrabKey(m_display, keycode, modifier | pressedModifiers,
                 m_win, True, GrabModeAsync, GrabModeAsync);
    }
}

void NativeEventFilter::unsetShortcut()
{
    foreach(quint32 pressedModifiers, maskModifiers()) {
        XUngrabKey(m_display, keycode, modifier | pressedModifiers, m_win);
    }
}

