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
    Contrivance_ExtensionSpawner.cpp \
    Contrivance_Console.cpp

HEADERS  += \
	Contrivance_ShortcutEditor.h \
	Contrivance_ContrivanceWindow.h \
	Contrivance_Shortcut.h \
    Contrivance_ExtensionSpawner.h \
    Contrivance_Defines.h \
    Contrivance_Console.h \
    Contrivance_IContrivanceWindow.h

FORMS    += \
	Contrivance_ShortcutEditor.ui \
	Contrivance_ContrivanceWindow.ui \
    Contrivance_ExtensionSpawner.ui \
    Contrivance_Console.ui

#TRANSLATIONS += \
#	Contrivance_<OtherLang>.ts
