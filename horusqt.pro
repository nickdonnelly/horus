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

CONFIG +=

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
        screenwindow.cpp \
    horus.cpp \
    horusuploader.cpp \
    imagehelper.cpp \
    editimagewindow.cpp \
    horusrectitem.cpp \
    horusgraphicsscene.cpp \
    updatedownloaddialog.cpp \
    filedropper.cpp \
    uploadfileswindow.cpp \
    textdropper.cpp \
    horustextitem.cpp \
    expirationselectiondialog.cpp \
    horusstyle.cpp \
    horussettings.cpp \
    editsettingswindow.cpp \
    models.cpp \
    horuspixmapgraphicsitem.cpp \
    components/hotkeyselector.cpp \
    components/hotkeyselectorlineedit.cpp \
    nativekeyeventfilter.cpp

linux: SOURCES +=

HEADERS += \
        screenwindow.h \
    horus.h \
    horusuploader.h \
    imagehelper.h \
    editimagewindow.h \
    horusrectitem.h \
    horusgraphicsscene.h \
    updatedownloaddialog.h \
    filedropper.h \
    uploadfileswindow.h \
    textdropper.h \
    horustextitem.h \
    expirationselectiondialog.h \
    horusstyle.h \
    horussettings.h \
    editsettingswindow.h \
    models.h \
    horuspixmapgraphicsitem.h \
    components/hotkeyselector.h \
    components/hotkeyselectorlineedit.h \
    nativekeyeventfilter.h

linux: HEADERS += 

FORMS += \
        screenwindow.ui \
    editimagewindow.ui \
    updatedownloaddialog.ui \
    uploadfileswindow.ui \
    expirationselectiondialog.ui \
    editsettingswindow.ui \
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
