QT += testlib
QT += gui
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES +=  tst_maintests.cpp \
    tst_settings.cpp

DESTDIR = $$PWD/../build

HEADERS += \
    autotest.h
