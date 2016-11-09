#ifndef FIRMWAREIMAGE_H
#define FIRMWAREIMAGE_H

#include <QString>
#include <QDateTime>
#include <QBuffer>

class FirmwareImage{
public:
    enum Crypto{Plain,AES128};
    explicit FirmwareImage();
    void clear();
    bool save(QString targetFile);
    bool open(QString fileName);
    bool isValid();
    bool isFileModified();

    QString getGitHash_str();
    QString getNameShort(int len);
    QString fileName;
    uint16_t getNameCRC16();

    uint32_t firmware_githash;
    uint32_t firmware_gitdate_unix;
    QDateTime firmware_gitdate_dt;
    QString firmware_version;
    QString firmware_name;
    QDateTime imageCreationDate;


    uint32_t firmware_entryPoint;
    uint32_t firmware_size;

    Crypto crypto;
    QByteArray binary;
    QByteArray sha256;
    QByteArray aes128_iv;

    bool firmwareLoadConsistency;

private:
    QDateTime lastModified;
    void createXML_for_checksum(QBuffer &buf);
    QString getBase64CheckSum(QBuffer &bufferForCheckSum);

};
#endif // FIRMWAREIMAGE_H

