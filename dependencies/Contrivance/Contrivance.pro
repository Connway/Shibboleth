#-------------------------------------------------
#
# Project created by QtCreator 2014-12-10T20:12:17
#
#-------------------------------------------------

QT       += core gui widgets
CONFIG   += c++11

TARGET = Contrivance
TEMPLATE = app

INCLUDEPATH += ContrivanceLib

CONFIG(debug, debug|release) {
	LIBS += -L../../ContrivanceLib/Debug/debug
}

CONFIG(release, debug|release) {
	LIBS += -L../../ContrivanceLib/Release/release
}

LIBS += -lContrivanceLib

SOURCES += \
	main.cpp

HEADERS +=

FORMS +=
