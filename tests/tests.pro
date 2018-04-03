QT += testlib
QT += gui widgets network
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

HEADERS += \
        $$PWD/../app/components/hotkeyselector.h \
        $$PWD/../app/components/hotkeyselectorlineedit.h

SOURCES +=  tst_maintests.cpp \
        tst_settings.cpp \
        $$PWD/../app/components/hotkeyselector.cpp \
        $$PWD/../app/components/hotkeyselectorlineedit.cpp

DESTDIR = $$PWD/../build

FORMS += $$PWD/../app/window/editsettingswindow.ui \
        $$PWD/../app/components/hotkeyselector.ui

INCLUDEPATH += $$PWD/../app

HEADERS += \
    autotest.h
