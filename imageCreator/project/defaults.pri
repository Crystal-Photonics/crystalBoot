
QT = gui core network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SRC_DIR = $$PWD

INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/src/CommunicationDevices

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Werror
CONFIG += c++14
CONFIG += warn

INCLUDEPATH += $$PWD/libs/Intel-HEX-Class/intelhex_class/
INCLUDEPATH += $$PWD/../../qt_common/
#message($$PWD)


