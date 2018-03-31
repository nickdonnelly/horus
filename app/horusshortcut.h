#ifndef HORUSSHORTCUT_H
#define HORUSSHORTCUT_H

#include <QHash>

enum HorusShortcut {
    Screenshot = 1000,
    ScreenshotFull = 1001,
    ScreenshotFullUp = 1002,

    VideoDefault = 2001,
    VideoCustom = 2002,

    PasteClip = 3000,
    FileDrop = 3001
};

struct HShortcut {
    int ident; // identifier to register (comes from HorusShortcut)
    QString displayString; // display string for the tableview
};

/// Returns a QHash containing the shortcuts. The key is the SETTING NAME
/// of the shortcut, like `hotkeys/screenshotfull`, for example. The individual
/// ident for each shortcut can be found in the value struct.
static QHash<QString, HShortcut> getShortcutHash() {
    QHash<QString, HShortcut> result;

    result.insert("hotkeys/screenshot",
                  HShortcut { HorusShortcut::Screenshot, "Open Screenshot Window"});
    result.insert("hotkeys/fullscreenshot",
                  HShortcut { HorusShortcut::ScreenshotFull, "Take Full Screenshot and Open in Editor"});
    result.insert("hotkeys/fullupscreenshot",
                  HShortcut { HorusShortcut::ScreenshotFullUp , "Take Full Screenshot and Upload"});
    result.insert("hotkeys/videodur",
                  HShortcut { HorusShortcut::VideoDefault, "Open Video Window (10s)"});
    result.insert("hotkeys/videocustom",
                  HShortcut { HorusShortcut::VideoCustom, "Open Video Window (Custom Duration)"});
    result.insert("hotkeys/pasteclip",
                  HShortcut { HorusShortcut::PasteClip, "Paste Clipboard Text"});
    result.insert("hotkeys/filedrop",
                  HShortcut { HorusShortcut::FileDrop, "Open File Dropper"});

    return result;
}

#endif // HORUSSHORTCUT_H
