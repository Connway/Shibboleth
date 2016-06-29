#-------------------------------------------------
#
# Project created by QtCreator 2014-12-25T17:42:29
#
#-------------------------------------------------

QT       += widgets
CONFIG   += c++11

TARGET = ShibbolethExtensions
TEMPLATE = lib

INCLUDEPATH += ../../dependencies/Contrivance/ContrivanceLib

CONFIG(debug, debug|release) {
	!contains(QMAKE_TARGET.arch, x86_64): TARGET = $$join(TARGET,,,32d)
	else: TARGET = $$join(TARGET,,,64d)

	LIBS += -L../../ContrivanceLib/Debug/debug
}

CONFIG(release, debug|release) {
	!contains(QMAKE_TARGET.arch, x86_64): TARGET = $$join(TARGET,,,32)
	else: TARGET = $$join(TARGET,,,64)

	LIBS += -L../../ContrivanceLib/Release/release
}

LIBS += -lContrivanceLib

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
