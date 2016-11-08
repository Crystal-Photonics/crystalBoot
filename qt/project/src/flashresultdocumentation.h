#ifndef FLASHRESULTDOCUMENTATION_H
#define FLASHRESULTDOCUMENTATION_H
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_qt2mcu.h"
#include <QMap>
#include <QString>
#include "firmwareimage.h"
#include "crystalsettings.h"

class RemoteDeviceInfo{
public:
    RemoteDeviceInfo();
    void setMCUDescriptor(mcu_descriptor_t *mcu_descriptor);
    void setDeviceDescriptor(device_descriptor_v1_t *deviceDescriptor);
    void setOldFirmwareDescriptor(firmware_descriptor_t  *firmwareDescriptor);





    QString getDeviceCategorieString();

    void unSet();
    bool isValid();

    mcu_descriptor_t mcu_descriptor;
    device_descriptor_v1_t deviceDescriptor;
    firmware_descriptor_t  firmwareDescriptor;

    static QString arrayToHexString(uint8_t *buff, const size_t length, int insertNewLine);


    QString getMCU_RevString();
    QString getMCU_FlashsizeString();
    QString getMCU_UniqueIDString(int length);
    QString getMCU_UniqueIDString();
    QString getMCU_AvailFlashSizeString();
    QString getMCU_EntryPointString();
    QString getMCU_MinimalEntryPointString();
    QString getMCU_DeviceIDString();

    QString getDEV_deviceID();
    QString getDEV_gitHash();
    QString getDEV_gitDate_str();
    QString getDEV_gitDate_unix();
    QString getDEV_name();
    QString getDEV_version();

    QString getFW_nameCRC();
    QString getFW_gitDate_str();
    QString getFW_gitDate_unix();
    QString getFW_gitHash();
    QString getFW_name();
    QString getFW_version();


    QString getFW_entryPoint();
    QString getFW_size();
    QString getMCU_cryptoRequired();
private:
    bool mcu_set;
    bool device_set;
    bool firmware_set;
};

class FlashResultDocumentation
{
public:
    FlashResultDocumentation();

    void save(CrystalBootSettings crystalBootSettings);
    void addActionResult(QString action, RPC_RESULT RPC_result);
    void setNewFirmwareDescriptor(FirmwareImage firmwareImage);
    void setRemoteDeviceInfo(RemoteDeviceInfo remoteDeviceInfo);
    void print();

private:
    RemoteDeviceInfo remoteDeviceInfo;
    bool getOverAllResult();

    QMap<QString,RPC_RESULT> actionResults;
    FirmwareImage  firmwareImage;
};

#endif // FLASHRESULTDOCUMENTATION_H
