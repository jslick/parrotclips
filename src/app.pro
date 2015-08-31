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
    clipstable.cpp

SOURCES += ../ext/sqlite3/sqlite3.c

HEADERS += \
    mainwindow.h \
    clip.h \
    clipboardmanager.h \
    clipsstorage.h \
    clipsgroup.h \
    view.h \
    clipswidget.h \
    clipstable.h

INCLUDEPATH += ../ext/sqlite3/

!win32:  LIBS += -ldl

RESOURCES += \
    app.qrc \
    sql.qrc

win32:  RC_FILE = app.rc
