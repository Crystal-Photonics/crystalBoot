#ifndef FIRMWAREIMAGE_H
#define FIRMWAREIMAGE_H

#include <QString>
#include <QDateTime>
#include <QBuffer>
#include "firmwaremetadata.h"
#include "imagecreatorsettings.h"

class FirmwareImage {
  public:
    explicit FirmwareImage();
    void clear();
    bool save_compiled_image(QString targetFile);
    bool load_compiled_image(QString fileName);
    bool load_plain_image(ImageCreatorSettings &settings);
    bool isValid();
    bool isFileModified();

    QString fileName;

    QByteArray binary_encoded;
    QByteArray binary_plain;
    QByteArray sha256;
    QByteArray aes128_iv;

    bool firmwareLoadConsistency;
    const FirmwareMetaData &get_firmware_meta_data();

    void set_crypto(FirmwareMetaData::Crypto crypto);

  private:
    FirmwareMetaData firmware_meta_data;
    QDateTime lastModified;
    void createXML_for_checksum(QBuffer &buf);
    QString getBase64CheckSum(QBuffer &bufferForCheckSum);
};
#endif // FIRMWAREIMAGE_H
