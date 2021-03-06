include(../defaults.pri)

CONFIG( debug, debug|release ) {
    # debug
     TARGET = crystalBoot_image_creatord
} else {
    # release
    TARGET = crystalBoot_image_creator

}

TEMPLATE = lib

DEFINES += EXPORT_LIBRARY

SOURCES += main.cpp
SOURCES += aeskeyfile.cpp
SOURCES += $$PWD/../../../qt_common/imagecreatorsettings.cpp
SOURCES += $$PWD/../../../qt_common/firmwareimage.cpp
SOURCES += $$PWD/../../../qt_common/firmwaremetadata.cpp
SOURCES += $$PWD/../../../qt_common/hexfile.cpp
SOURCES += firmwareencoder.cpp
SOURCES += mainwindow.cpp

SOURCES += $$PWD/../libs/Intel-HEX-Class/intelhex_class/intelhexclass.cpp
SOURCES += $$PWD/../libs/tiny-AES128-C/aes.c
HEADERS += $$PWD/../libs/tiny-AES128-C/aes.h

HEADERS += $$PWD/../libs/Intel-HEX-Class/intelhex_class/intelhexclass.h
HEADERS += aeskeyfile.h
HEADERS += $$PWD/../../../qt_common/imagecreatorsettings.h
HEADERS += $$PWD/../../../qt_common/firmwareimage.h
HEADERS += $$PWD/../../../qt_common/firmwaremetadata.h
HEADERS += $$PWD/../../../qt_common/stm32_fimware_descriptor.h
HEADERS += $$PWD/../../../qt_common/hexfile.h
HEADERS += firmwareencoder.h

HEADERS += mainwindow.h

FORMS    += mainwindow.ui

