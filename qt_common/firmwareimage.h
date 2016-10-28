#ifndef FIRMWAREIMAGE_H
#define FIRMWAREIMAGE_H

#include <QString>
#include <QDateTime>

class FirmwareImage{
public:
    enum Crypto{Plain,AES};
    explicit FirmwareImage();
    void clear();
    bool save(QString targetFile);
    bool open(QString fileName);
    bool isValid();

    QString getNameShort(int len);
    uint16_t getNameCRC16();

    uint32_t firmware_githash;
    uint32_t firmware_gitdate;
    QDateTime firmware_gitdate_dt;
    QString firmware_version;
    QString firmware_name;

    uint32_t firmware_entryPoint;
    uint32_t firmware_size;

    Crypto crypto;
    QByteArray binary;


private:

};
#endif // FIRMWAREIMAGE_H

