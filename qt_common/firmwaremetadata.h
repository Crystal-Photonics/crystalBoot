#ifndef FIRMWAREMETADATA_H
#define FIRMWAREMETADATA_H
#include "imagecreatorsettings.h"
#include <QDateTime>
#include <QDomElement>
#include <QXmlStreamWriter>
#include <QByteArray>

class HexFileWriter {
  public:
    HexFileWriter();
    QStringList get_hex_file(QByteArray indata, uint32_t start_address);

  private:
    QStringList dump_data_line(uint8_t byte_buffer[], uint32_t buffer_length, uint32_t old_address, uint32_t actual_address);
    QString get_extended_linear_address(uint32_t buffer_addr);
    QString get_checksum(QString line);
};

class FirmwareMetaData {
  public:
    enum Crypto { Plain, AES128 };

    FirmwareMetaData();
    bool load_meta_data_from_source(ImageCreatorSettings &settings);
    bool load_meta_data_from_xml(QDomElement &metaNodeElement);
    void write_meta_data_to_xml(QXmlStreamWriter &xml);

    QStringList compile_firmware_descriptor_hex(const QByteArray &sha256) const;
    QByteArray compile_firmware_descriptor_binary(const QByteArray &sha256) const;
    uint32_t firmware_githash;
    uint32_t firmware_gitdate_unix;
    QDateTime firmware_gitdate_dt;
    QString firmware_version;
    QString firmware_name;
    uint32_t firmware_entryPoint;

    uint32_t address_firmware_descriptor_in_bootloader;
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
