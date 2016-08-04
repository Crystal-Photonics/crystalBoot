#include(libs/PythonQt3.0/build/python.prf)

QT = gui core network testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SRC_DIR = $$PWD

INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/src/CommunicationDevices

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Werror -Wno-unknown-pragmas


CONFIG += c++14
CONFIG += warn

INCLUDEPATH += $$PWD/libs/build/qtserialport/debug/include/
INCLUDEPATH += ../libs/rpc/include/
INCLUDEPATH += ../libs/RPC-ChannelCodec/include/

greaterThan(QT_MAJOR_VERSION, 4): QT += serialport

lessThan(QT_MAJOR_VERSION, 5){
    #message(qt version: $$QT_MAJOR_VERSION)
    #working with QT4 or less?
    CONFIG( debug, debug|release ) {
        LIBS += -L$$PWD/libs/build/qtserialport/debug/src/serialport/
        LIBS += -L$$PWD/libs/build/qtserialport/lib/
        LIBS += -lQtSerialPort
    } else {
        LIBS += -L$$PWD/libs/build/qtserialport/release/src/serialport/
        LIBS += -L$$PWD/libs/build/qtserialport/lib/
        LIBS += -lQtSerialPort

    }
}


#exists( $$PWD/libs/PythonQt3.0/src/PythonQt.h ) {
    #message(found python windows)
#    INCLUDEPATH += $$PWD/libs/PythonQt3.0/src/
#    LIBS += -L$$PWD/libs/PythonQt3.0/lib
#   PYTHONQT_FOUND = 1
#}

#exists( $$(PYTHONQT_PATH)/src/PythonQt.h ) {
    #message(found python windows)
#    INCLUDEPATH += $$(PYTHONQT_PATH)/src/PythonQt.h
#    LIBS += -L$$(PYTHONQT_PATH)/lib
#    PYTHONQT_FOUND = 1
#}

#exists( $$(PYTHONQT_PATH)/PythonQt.h ) {
    #message(found PythonQt linux)
#    INCLUDEPATH += $$(PYTHONQT_PATH)
#    PYTHONQT_FOUND = 1
#}

#!equals( PYTHONQT_FOUND , 1){
#    error (PythonQT directory needs to be configured in environment variable PYTHONQT_PATH. )
#}



#LIBS += -lPythonQt  #-lPythonQt_QtAll



