#-------------------------------------------------
#
# Project created by QtCreator 2014-12-25T17:42:29
#
#-------------------------------------------------

QT       += widgets
CONFIG   += c++11

TARGET = ShibbolethExtensions
TEMPLATE = lib

DEFINES += SHIBBOLETHEXTENSIONS_LIBRARY

INCLUDEPATH += ../../dependencies/Contrivance

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

SOURCES += \
	ShibbolethExtensions.cpp \
	ObjectEditor.cpp \
	ComponentList.cpp

HEADERS += \
	ObjectEditor.h \
	ComponentList.h

FORMS += \
	ObjectEditor.ui \
	ComponentList.ui
