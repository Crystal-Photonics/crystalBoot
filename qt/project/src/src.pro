include(../defaults.pri)

CONFIG( debug, debug|release ) {
    # debug
     TARGET = qTestProjectTemplateAppd
} else {
    # release
    TARGET = qTestProjectTemplateApp

}

TEMPLATE = lib

DEFINES += EXPORT_LIBRARY

SOURCES += main.cpp
SOURCES += serialworker.cpp
SOURCES += mainwindow.cpp


SOURCES += ../libs/rpc/src/server/app/rpc_func_qt.cpp
SOURCES += ../libs/rpc/src/server/app/rpc_service_qt.cpp

HEADERS += mainwindow.h
HEADERS += serialworker.h

SOURCES += ../libs/rpc/src/client/generated_app/RPC_TRANSMISSION_qt2mcu.c
SOURCES += ../libs/rpc/src/server/generated_app/RPC_TRANSMISSION_parser.c

HEADERS += ../libs/rpc/include/rpc_transmission/client/generated_general/RPC_TRANSMISSION_network.h
HEADERS += ../libs/rpc/include/rpc_transmission/client/generated_app/RPC_TRANSMISSION_qt2mcu.h

HEADERS += ../libs/rpc/include/rpc_transmission/server/generated_general/RPC_TRANSMISSION_network.h
HEADERS += ../libs/rpc/include/rpc_transmission/server/generated_general/RPC_TRANSMISSION_parser.h
HEADERS += ../libs/rpc/include/rpc_transmission/server/generated_general/RPC_types.h
HEADERS += ../libs/rpc/include/rpc_transmission/server/app/mcu2qt.h

SOURCES += ../libs/RPC-ChannelCodec/src/channel_codec/channel_codec.c
SOURCES += ../libs/RPC-ChannelCodec/src/channel_codec/crc16.c

HEADERS += ../libs/RPC-ChannelCodec/include/channel_codec/channel_codec.h
HEADERS +=
HEADERS += ../libs/rpc/include/channel_codec/channel_codec_config.h


INCLUDEPATH += ../libs/RPC-ChannelCodec/include/errorlogger_dummy
#INCLUDEPATH += ../libs/RPC-ChannelCodec/tests/include

INCLUDEPATH += ../libs/rpc/include/
INCLUDEPATH += ../libs/rpc/include/rpc_transmission/client/generated_app


FORMS    += mainwindow.ui



#INCLUDEPATH += $$(PYTHON_PATH)/include

#exists( $$(PYTHON_PATH)/include/python.h ) {
#    #message(found python windows)
#    INCLUDEPATH += $$(PYTHON_PATH)/include
#    PYTHON_FOUND = 1
#}

#exists( $$(PYTHON_PATH)/Python.h ) {
    #message(found python linux)crystalTestFrameworkApp
#    INCLUDEPATH += $$(PYTHON_PATH)
#    PYTHON_FOUND = 1
#}

#!equals( PYTHON_FOUND , 1){
#    error (Python directory needs to be configured in environment variable PYTHON_PATH. eg. C:/Python27 )
#}
