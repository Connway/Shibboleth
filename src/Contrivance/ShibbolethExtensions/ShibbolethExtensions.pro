#-------------------------------------------------
#
# Project created by QtCreator 2014-12-25T17:42:29
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = ShibbolethExtensions
TEMPLATE = lib

DEFINES += SHIBBOLETHEXTENSIONS_LIBRARY

SOURCES += \
	ShibbolethExtensions.cpp \
	TestExtension.cpp

# Modify to include the path to "Contrivance_IContrivanceWindow.h"
INCLUDEPATH += ..

CONFIG(debug, debug|release) {
	!contains(QMAKE_TARGET.arch, x86_64): TARGET = $$join(TARGET,,,32d)
	else: TARGET = $$join(TARGET,,,64d)
}

CONFIG(release, debug|release) {
	!contains(QMAKE_TARGET.arch, x86_64): TARGET = $$join(TARGET,,,32)
	else: TARGET = $$join(TARGET,,,64)
}

unix {
	target.path = /usr/lib
	INSTALLS += target
}

FORMS += \
	TestExtension.ui

HEADERS += \
	TestExtension.h
