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
SOURCES += aeskeyfile.cpp
SOURCES += imagecreatorsettings.cpp

SOURCES += $$PWD/../../../qt_common/firmwareimage.cpp
SOURCES += firmwareencoder.cpp
SOURCES += mainwindow.cpp

SOURCES += $$PWD/../libs/Intel-HEX-Class/intelhex_class/intelhexclass.cpp
SOURCES += $$PWD/../libs/tiny-AES128-C/aes.c
HEADERS += $$PWD/../libs/tiny-AES128-C/aes.h
    #tiny-aes128-c_wrapper_.h

HEADERS += $$PWD/../libs/Intel-HEX-Class/intelhex_class/intelhexclass.h
HEADERS += aeskeyfile.h
HEADERS += imagecreatorsettings.h
HEADERS += $$PWD/../../../qt_common/firmwareimage.h
HEADERS += firmwareencoder.h
HEADERS +=

HEADERS += mainwindow.h

FORMS    += mainwindow.ui

