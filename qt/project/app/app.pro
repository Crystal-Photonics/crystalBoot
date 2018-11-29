include(../defaults.pri)

TEMPLATE = app


SOURCES +=  main.cpp

RC_ICONS += app_icon_multisize.ico

CONFIG(debug, debug|release) {
   # message(Debug)
}

TARGET = crystalBoot

CONFIG( debug, debug|release ) {
    # debug
     #message(Debug)


     LIBS += -L../src/debug/
     LIBS += -L../src/
     LIBS +=  -lcrystalBootd
} else {
    # release
    #message(Release)
    LIBS += -L../src/release/
    LIBS += -L../src/
    LIBS +=  -lcrystalBoot

}

