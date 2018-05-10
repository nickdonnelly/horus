#-------------------------------------------------
#
# Project created by QtCreator 2017-06-02T14:00:49
#
#-------------------------------------------------

QT       += core gui network
linux: QT += x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = horusqt
TEMPLATE = app

CONFIG += c++11

linux: CONFIG += link_pkgconfig
linux: PKGCONFIG += x11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    horus.cpp \
    filedropper.cpp \
    textdropper.cpp \
    horusstyle.cpp \
    horussettings.cpp \
    net/models.cpp \
    net/horusuploader.cpp \
    window/screenwindow.cpp \
    window/editimagewindow.cpp \
    window/updatedownloaddialog.cpp \
    window/editsettingswindow.cpp \
    window/uploadfileswindow.cpp \
    window/expirationselectiondialog.cpp \
    components/horusrectitem.cpp \
    components/horusgraphicsscene.cpp \
    components/horustextitem.cpp \
    components/horuspixmapgraphicsitem.cpp \
    components/hotkeyselector.cpp \
    components/hotkeyselectorlineedit.cpp \
    components/toggleswitch.cpp \

linux: SOURCES += \
    helpers/xkeyeventfilter.cpp

win32: SOURCES += \
    helpers/win32hotkeyregistry.cpp

HEADERS += \
    horus.h \
    filedropper.h \
    textdropper.h \
    horusstyle.h \
    horussettings.h \
    horusshortcut.h \
    net/models.h \
    net/horusuploader.h \
    window/screenwindow.h \
    window/editsettingswindow.h \
    window/editimagewindow.h \
    window/updatedownloaddialog.h \
    window/uploadfileswindow.h \
    window/expirationselectiondialog.h \
    components/horusrectitem.h \
    components/horusgraphicsscene.h \
    components/horuspixmapgraphicsitem.h \
    components/horustextitem.h \
    components/hotkeyselector.h \
    components/hotkeyselectorlineedit.h \
    components/toggleswitch.h \

linux: HEADERS += \
        helpers/x11helper.h \
        helpers/xkeyeventfilter.h

win32: HEADERS += \
    helpers/win32hotkeyregistry.h

FORMS += \
    window/screenwindow.ui \
    window/editimagewindow.ui \
    window/updatedownloaddialog.ui \
    window/uploadfileswindow.ui \
    window/expirationselectiondialog.ui \
    window/editsettingswindow.ui \
    components/hotkeyselector.ui

DISTFILES += \
    res/edit_last.png \
    res/paste.png \
    res/manage.png \
    master.qss \
    res/icon_screenshot_mode.png \
    res/icon_video_mode.png

RESOURCES += \
    horus-resources.qrc

DESTDIR = $$PWD/../build
