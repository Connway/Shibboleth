#-------------------------------------------------
#
# Project created by QtCreator 2014-12-10T20:12:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Contrivance
TEMPLATE = app


SOURCES += main.cpp\
    Contrivance_ShortcutEditor.cpp \
    Contrivance_ContrivanceWindow.cpp

HEADERS  += \
    Contrivance_Defines.h \
    Contrivance_ShortcutEditor.h \
    Contrivance_ContrivanceWindow.h \
    Contrivance_Shortcut.h

FORMS    += \
    Contrivance_ShortcutEditor.ui \
    Contrivance_ContrivanceWindow.ui
