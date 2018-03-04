#include <QTextStream>
#include "win32hotkeyregistry.h"

namespace {
    struct KeyRegistration {
        int modifier;
        char key;
    };

    QHash<int, KeyRegistration> registrations;
}

Win32HotkeyRegistry::Win32HotkeyRegistry(int winId, QObject *parent) : QObject(parent)
{
    windowId = winId;
}

void Win32HotkeyRegistry::registerHotkey(int identifier, QKeySequence seq)
{
    if(seq.isEmpty()) return;
    int _mod = 0;
    char _key;

    foreach(QString key, seq.toString().split("+")) {
        if(key.toLower() == "shift") {
            _mod |= MOD_SHIFT;
        } else if(key.toLower() == "alt") {
            _mod |= MOD_ALT;
        } else if(key.toLower() == "meta") {
            _mod |= MOD_WIN;
        } else if(key.toLower() == "ctrl") {
            _mod |= MOD_CONTROL;
        } else {
            if(key.startsWith("F") && key.length() > 1) {
                _key = mapFunctionKey(key);
            } else { // for ascii keys we can just use the character form.
                _key = key.at(0).toLatin1();
            }
        }
    }

    RegisterHotKey((HWND) windowId,
                   identifier,
                   _mod, _key);

    KeyRegistration reg;
    reg.key = _key;
    reg.modifier = _mod;
    registrations.insert(identifier, reg);
}

void Win32HotkeyRegistry::unRegisterHotkey(int identifier)
{
    if(registrations.contains(identifier)) {
        UnregisterHotKey((HWND) windowId, identifier);
        registrations.remove(identifier);
    }
}

bool Win32HotkeyRegistry::nativeEventFilter(const QByteArray &eventType, void *msg, long *result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)

    MSG *m = reinterpret_cast<MSG*>(msg);
    // if a hotkey event is fired
    if(m->message == WM_HOTKEY) {
        QHashIterator<int, KeyRegistration> iter(registrations);
        while(iter.hasNext()) {
            iter.next();
            if((unsigned int)iter.key() == m->wParam) {
                emit hotkeyPressed(iter.key());
                return true;
            }
        }
    }
    return false;
}

/// Returns the character representing a key F1, F2, etc... in windows virtualkeys.
char Win32HotkeyRegistry::mapFunctionKey(QString key)
{
    char result = 0x70;
    int n = key.mid(1).toInt();
    if(n == 0) return 0;

    for(int i = 0; i < n - 1; i++) {
        result += 0x1;
    }

    return result;
}
