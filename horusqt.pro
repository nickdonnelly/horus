#-------------------------------------------------
#
# Project created by QtCreator 2017-06-02T14:00:49
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = horusqt
TEMPLATE = app

CONFIG +=

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
    settingsdialog.cpp \
    horusuploader.cpp \
    imagehelper.cpp \
    editimagewindow.cpp \
    horusrectitem.cpp \
    horusgraphicsscene.cpp \
    updatedownloaddialog.cpp \
    filedropper.cpp \
    uploadfileswindow.cpp \
    textdropper.cpp \
    horustextitem.cpp

HEADERS += \
        screenwindow.h \
    horus.h \
    settingsdialog.h \
    horusuploader.h \
    imagehelper.h \
    editimagewindow.h \
    horusrectitem.h \
    horusgraphicsscene.h \
    updatedownloaddialog.h \
    filedropper.h \
    uploadfileswindow.h \
    textdropper.h \
    horustextitem.h

FORMS += \
        screenwindow.ui \
    settingsdialog.ui \
    editimagewindow.ui \
    updatedownloaddialog.ui \
    uploadfileswindow.ui

DISTFILES += \
    res/edit_last.png \
    res/paste.png

RESOURCES += \
    horus-resources.qrc
