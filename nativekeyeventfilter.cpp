#include "nativekeyeventfilter.h"

#include <QVector>
#include <QHash>
#include <QHashIterator>

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

    struct KeyRegistration {
        int modifier;
        int keycode;
    };

    QHash<QString, KeyRegistration> registrations;

}

NativeKeyEventFilter::NativeKeyEventFilter(QObject *parent) : QObject(parent)
{
    m_display = QX11Info::display();
    m_win = DefaultRootWindow(m_display);
}

bool NativeKeyEventFilter::nativeEventFilter(const QByteArray &eventType, void *msg, long *result)
{
    Q_UNUSED(result)

    xcb_key_press_event_t *keyEvent = 0;
    if(eventType == "xcb_generic_event_t") { // we have linux event
        xcb_generic_event_t *evt = static_cast<xcb_generic_event_t *>(msg);


        if((evt->response_type & 127) == XCB_KEY_PRESS) {
            // we are in a key event
            keyEvent = static_cast<xcb_key_press_event_t *>(msg);

            QHashIterator<QString, KeyRegistration> iter(registrations);

            while(iter.hasNext()){
                iter.next();
                foreach(quint32 pressedModifiers, maskModifiers()) {
                    if(keyEvent->state == (iter.value().modifier | pressedModifiers)
                        && keyEvent->detail == iter.value().keycode){

                        // the key was pressed!
                        emit shortcutPressed(iter.key());
                        return true;
                    }
                }

            }

        }
    }

    return false;
}

void NativeKeyEventFilter::addShortcut(QString identifier, QKeySequence seq)
{
    if(registrations.contains(identifier)) registrations.remove(identifier);

    int _modifier = 0;
    int _key = 0;

    foreach(QString key, seq.toString().split("+")){
        if(key.toLower() == "ctrl") {
            _modifier |= KeyModifier::Ctrl;
        } else if(key.toLower() == "alt"){
            _modifier |= KeyModifier::Alt;
        } else if(key.toLower() == "shift") {
            _modifier |= KeyModifier::Shift;
        } else if(key.toLower() == "meta") {
            _modifier |= KeyModifier::WindowsKey;
        } else {
            _key = QKeySequence::fromString(key)[0];
        }
    }

    KeyRegistration reg;
    reg.modifier = _modifier;
    reg.keycode = XKeysymToKeycode(m_display, _key);

    foreach(quint32 mod, maskModifiers()){
        XGrabKey(m_display, reg.keycode, reg.modifier | mod,
                 m_win, True, GrabModeAsync, GrabModeAsync);
    }

    registrations.insert(identifier, reg);
}

void NativeKeyEventFilter::removeShortcut(QString identifier)
{
    if(registrations.contains(identifier)){
        foreach(quint32 mod, maskModifiers()){
        XUngrabKey(m_display,
               registrations[identifier].keycode,
               registrations[identifier].modifier | mod,
               m_win);
        }
        registrations.remove(identifier);
    }
}
