include(../defaults.pri)

TEMPLATE = app
TARGET = crystalBoot_image_creator


RC_ICONS += app_icon_multisize.ico

SOURCES +=  main.cpp

CONFIG(debug, debug|release) {
   # message(Debug)
}
CONFIG( debug, debug|release ) {
    # debug
     #message(Debug)


     LIBS += -L../src/debug/
     LIBS += -L../src/
     LIBS +=  -lcrystalBoot_image_creatord
} else {
    # release
    #message(Release)
    LIBS += -L../src/release/
    LIBS += -L../src/
    LIBS +=  -lcrystalBoot_image_creator

}

