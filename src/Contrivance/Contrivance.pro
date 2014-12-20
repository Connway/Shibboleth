#-------------------------------------------------
#
# Project created by QtCreator 2014-12-10T20:12:17
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Contrivance
TEMPLATE = app


SOURCES += main.cpp\
    Contrivance_ShortcutEditor.cpp \
    Contrivance_ContrivanceWindow.cpp \
    Contrivance_ShortcutKeySelector.cpp

HEADERS  += \
    Contrivance_Defines.h \
    Contrivance_ShortcutEditor.h \
    Contrivance_ContrivanceWindow.h \
    Contrivance_Shortcut.h \
    Contrivance_ShortcutKeySelector.h

FORMS    += \
    Contrivance_ShortcutEditor.ui \
    Contrivance_ContrivanceWindow.ui \
    Contrivance_ShortcutKeySelector.ui
