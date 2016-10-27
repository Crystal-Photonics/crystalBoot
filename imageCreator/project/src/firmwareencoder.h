#ifndef FIRMWAREENCODER_H
#define FIRMWAREENCODER_H

#include "firmwareimage.h"
#include "intelhexclass.h"
#include "imagecreatorsettings.h"
#include <QString>

class FirmwareEncoder{
public:
    explicit FirmwareEncoder(ImageCreatorSettings imageCreatorSettings);
    void fetchMetaData();
    void createImage();


    FirmwareImage fwImage;
private:
    enum DefineType {None,Number,String} ;
    ImageCreatorSettings imageCreatorSettings;
    bool parseTheDefine(QString defineName, DefineType defineType ,QString line,  QString &value);
    intelhex ihex;
};
#endif // FIRMWAREENCODER_H
