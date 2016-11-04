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

SOURCES += main.cpp \
    options.cpp \
    crystalsettings.cpp \
    flashresultdocumentation.cpp \
    namedplacemarker.cpp
SOURCES += infodialog.cpp
SOURCES += serialworker.cpp
SOURCES += mainwindow.cpp


SOURCES += ../libs/rpc/src/server/app/rpc_func_qt.cpp
SOURCES += ../libs/rpc/src/server/app/rpc_service_qt.cpp

SOURCES += $$PWD/../../../qt_common/firmwareimage.cpp
HEADERS += $$PWD/../../../qt_common/firmwareimage.h \
    options.h \
    crystalsettings.h \
    flashresultdocumentation.h \
    namedplacemarker.h

HEADERS += mainwindow.h
HEADERS += infodialog.h
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


FORMS    += mainwindow.ui \
    optionsdiag.ui
FORMS    += infodialog.ui


SH = c:\Program Files (x86)\Git\usr\bin\sh.exe

system($$system_quote($$SH) $$PWD/git.sh)

  qmakeforce.target = dummy
  qmakeforce.commands = rm -f Makefile ##to force rerun of qmake
  qmakeforce.depends = FORCE
  PRE_TARGETDEPS += $$qmakeforce.target
  QMAKE_EXTRA_TARGETS += qmakeforce


HEADERS += vc.h
