include(../defaults.pri)

TEMPLATE = app


RC_ICONS += app_icon_multisize.ico

SOURCES +=  main.cpp

CONFIG(debug, debug|release) {
   # message(Debug)
}
TARGET = crystalImageCreator
CONFIG( debug, debug|release ) {
    # debug
     #message(Debug)


     LIBS += -L../src/debug/
     LIBS += -L../src/
     LIBS +=  -lqTestProjectTemplateAppd
} else {
    # release
    #message(Release)
    LIBS += -L../src/release/
    LIBS += -L../src/
    LIBS +=  -lqTestProjectTemplateApp

}

