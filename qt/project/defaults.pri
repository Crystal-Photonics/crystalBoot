
QT = gui core network testlib xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SRC_DIR = $$PWD

INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/src/CommunicationDevices

INCLUDEPATH += $$PWD/../../qt_common/

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


