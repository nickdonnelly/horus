#include <Qt>
#include <X11/Xutil.h>

#ifndef X11HELPER_H
#define X11HELPER_H

static int qt_key_to_x11_sym(int keycode) {
    switch(keycode){
    case Qt::Key_F1:
        return XK_F1;
    case Qt::Key_F2:
        return XK_F2;
    case Qt::Key_F3:
        return XK_F3;
    case Qt::Key_F4:
        return XK_F4;
    case Qt::Key_F5:
        return XK_F5;
    case Qt::Key_F6:
        return XK_F6;
    case Qt::Key_F7:
        return XK_F7;
    case Qt::Key_F8:
        return XK_F8;
    case Qt::Key_F9:
        return XK_F9;
    case Qt::Key_F10:
        return XK_F10;
    case Qt::Key_F11:
        return XK_F11;
    case Qt::Key_F12:
        return XK_F12;
    case Qt::Key_F13:
        return XK_F13;
    case Qt::Key_F14:
        return XK_F14;
    case Qt::Key_F15:
        return XK_F15;
    case Qt::Key_F16:
        return XK_F16;
    case Qt::Key_F17:
        return XK_F17;
    case Qt::Key_F18:
        return XK_F18;
    case Qt::Key_F19:
        return XK_F19;
    case Qt::Key_F20:
        return XK_F20;
    case Qt::Key_F21:
        return XK_F21;
    case Qt::Key_F22:
        return XK_F22;
    case Qt::Key_F23:
        return XK_F23;
    case Qt::Key_F24:
        return XK_F24;
    case Qt::Key_F25:
        return XK_F25;
    case Qt::Key_F26:
        return XK_F26;
    case Qt::Key_F27:
        return XK_F27;
    case Qt::Key_F28:
        return XK_F28;
    case Qt::Key_F29:
        return XK_F29;

    default:
        return keycode; // most are ok
    }

}

#endif // X11HELPER_H
