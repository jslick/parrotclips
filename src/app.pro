#-------------------------------------------------
#
# Project created by QtCreator 2015-04-30T20:30:47
#
#-------------------------------------------------

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = parrotclips
TEMPLATE = app
CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    clip.cpp \
    clipboardmanager.cpp \
    clipsstorage.cpp \
    clipsgroup.cpp \
    clipswidget.cpp \
    clipstable.cpp \
    switcherdialog.cpp \
    mruswitcher.cpp

SOURCES += ../ext/sqlite3/sqlite3.c

HEADERS += \
    ../include/parrotclips/view.h \
    ../include/parrotclips/switcher.h \
    mainwindow.h \
    clip.h \
    clipboardmanager.h \
    clipsstorage.h \
    clipsgroup.h \
    clipswidget.h \
    clipstable.h \
    switcherdialog.h \
    mruswitcher.h

INCLUDEPATH += ../include/
INCLUDEPATH += ../ext/
INCLUDEPATH += ../ext/sqlite3/

!win32:  LIBS += -ldl
win32-msvc* {
    CONFIG(debug, debug|release) {
        LIBS += -L$$OUT_PWD/../ext/UGlobalHotkey/debug/ -lUGlobalHotkey
    }
    else {
        LIBS += -L$$OUT_PWD/../ext/UGlobalHotkey/release/ -lUGlobalHotkey
    }
}
else {
    LIBS += -L$$OUT_PWD/../ext/UGlobalHotkey/
}
LIBS += -lUGlobalHotkey

RESOURCES += \
    app.qrc \
    sql.qrc

win32:  RC_FILE = app.rc

g++: PRE_TARGETDEPS += $$OUT_PWD/../uglobalhotkey/libUGlobalHotkey.so
