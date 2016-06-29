#-------------------------------------------------
#
# Project created by QtCreator 2016-06-19T01:44:41
#
#-------------------------------------------------

QT       += widgets
QT       -= gui

TARGET = ContrivanceLib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
	Contrivance_Console.cpp \
	Contrivance_ContrivanceWindow.cpp \
	Contrivance_DockWidgetWrapper.cpp \
	Contrivance_ExtensionSpawner.cpp \
	Contrivance_ShortcutEditor.cpp \
    Contrivance_ContrivanceExtension.cpp

HEADERS += \
	Contrivance_Console.h \
	Contrivance_ContrivanceWindow.h \
	Contrivance_Defines.h \
	Contrivance_DockWidgetWrapper.h \
	Contrivance_ExtensionSpawner.h \
	Contrivance_IContrivanceWindow.h \
	Contrivance_Shortcut.h \
	Contrivance_ShortcutEditor.h \
    Contrivance_ContrivanceExtension.h
unix {
	target.path = /usr/lib
	INSTALLS += target
}

FORMS += \
	Contrivance_Console.ui \
	Contrivance_ContrivanceWindow.ui \
	Contrivance_ExtensionSpawner.ui \
	Contrivance_ShortcutEditor.ui

#TRANSLATIONS += \
#	Contrivance_<OtherLang>.ts

#DISTFILES +=
