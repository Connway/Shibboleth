#-------------------------------------------------
#
# Project created by QtCreator 2014-12-10T20:12:17
#
#-------------------------------------------------

QT       += core gui widgets
CONFIG   += c++11

TARGET = Contrivance
TEMPLATE = app

SOURCES += \
	main.cpp \
	Contrivance_ShortcutEditor.cpp \
	Contrivance_ContrivanceWindow.cpp \
	Contrivance_ExtensionSpawner.cpp \
	Contrivance_Console.cpp \
    Contrivance_DockWidgetWrapper.cpp \
    Contrivance_DockWidgetHelper.cpp

HEADERS  += \
	Contrivance_ShortcutEditor.h \
	Contrivance_ContrivanceWindow.h \
	Contrivance_Shortcut.h \
	Contrivance_ExtensionSpawner.h \
	Contrivance_Defines.h \
	Contrivance_Console.h \
	Contrivance_IContrivanceWindow.h \
    Contrivance_DockWidgetWrapper.h \
    Contrivance_DockWidgetHelper.h

FORMS    += \
	Contrivance_ShortcutEditor.ui \
	Contrivance_ContrivanceWindow.ui \
	Contrivance_ExtensionSpawner.ui \
	Contrivance_Console.ui

#TRANSLATIONS += \
#	Contrivance_<OtherLang>.ts

#DISTFILES +=
