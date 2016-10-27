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
SOURCES += imagecreatorsettings.cpp

SOURCES += $$PWD/../../../qt_common/firmwareimage.cpp
SOURCES += firmwareencoder.cpp
SOURCES += mainwindow.cpp

SOURCES += $$PWD/../libs/Intel-HEX-Class/intelhex_class/intelhexclass.cpp

HEADERS += $$PWD/../libs/Intel-HEX-Class/intelhex_class/intelhexclass.h
HEADERS += imagecreatorsettings.h
HEADERS += $$PWD/../../../qt_common/firmwareimage.h
HEADERS += firmwareencoder.h

HEADERS += mainwindow.h

FORMS    += mainwindow.ui

