#ifndef FIRMWAREMETADATA_H
#define FIRMWAREMETADATA_H
#include "imagecreatorsettings.h"
#include <QDateTime>
#include <QDomElement>
#include <QXmlStreamWriter>

class FirmwareMetaData {
  public:
    enum Crypto { Plain, AES128 };

    FirmwareMetaData();
    bool load_meta_data_from_source(ImageCreatorSettings &settings);
    bool load_meta_data_from_xml(QDomElement &metaNodeElement);
    void write_meta_data_to_xml(QXmlStreamWriter &xml);

    uint32_t firmware_githash;
    uint32_t firmware_gitdate_unix;
    QDateTime firmware_gitdate_dt;
    QString firmware_version;
    QString firmware_name;
    uint32_t firmware_entryPoint;

    size_t firmware_size;
    QDateTime imageCreationDate;

    uint16_t getNameCRC16() const;
    QString getGitHash_str() const;
    QString getNameShort(int len) const;
    void clear();
    bool is_valid();

    Crypto crypto;

  private:
    enum DefineType { None, Number, String };
    bool parseTheDefine(QString defineName, DefineType defineType, QString line, QString &value);
};

#endif // FIRMWAREDESCRIPTOR_H
