#ifndef FIRMWAREENCODER_H
#define FIRMWAREENCODER_H

#include "firmwareimage.h"
#include "intelhexclass.h"
#include "imagecreatorsettings.h"
#include <QString>

bool readHexFile(QString fileName, QByteArray &result, uint32_t &startAddress);
bool readBinFile(QString fileName, QByteArray &result);
QByteArray AES_CBC_128_encrypt(QByteArray &key, QByteArray &iv, QByteArray &plain);
QByteArray AES_CBC_128_decrypt(QByteArray key, QByteArray iv, QByteArray &cipher);

class FirmwareImageContainer {
  public:
    explicit FirmwareImageContainer(ImageCreatorSettings &imageCreatorSettings);
    bool load_plain_image();
    bool saveImage();

    FirmwareImage fwImage;

  private:
    ImageCreatorSettings &imageCreatorSettings;
    //   bool parseTheDefine(QString defineName, DefineType defineType, QString line, QString &value);
    intelhex ihex;
};
#endif // FIRMWAREENCODER_H
