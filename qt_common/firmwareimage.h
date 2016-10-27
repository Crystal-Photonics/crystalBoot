#ifndef FIRMWAREIMAGE_H
#define FIRMWAREIMAGE_H

#include <QString>
#include <QDateTime>

class FirmwareImage{
public:
    explicit FirmwareImage();
    void clear();

    uint32_t firmware_githash;
    QDateTime firmware_gitdate;
    QString firmware_version;
    QString firmware_name;

    uint32_t firmware_entryPoint;
    uint32_t firmware_size;
private:

};
#endif // FIRMWAREIMAGE_H
