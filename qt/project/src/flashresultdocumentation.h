#ifndef FLASHRESULTDOCUMENTATION_H
#define FLASHRESULTDOCUMENTATION_H
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_qt2mcu.h"
#include <QMap>
#include <QString>
#include <set>
#include "firmwareimage.h"
#include "crystalsettings.h"

enum class PlausibilityResult{
    error_firmwareimage_too_big,
    error_wrong_entrypoint,
    error_inconsistency,
    error_crypto_required,
    warning_different_name_hash,
    warning_downgrade_date,

    warning_equal_gitHash,
    warning_equal_gitHash_but_different_wersion,
    warning_equal_version_but_different_gitHash
};


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
    QString getMCU_ProtectionLevel();

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

class FirmwareUpdatePlausibilityCheck{
public:
    FirmwareUpdatePlausibilityCheck();

    void checkPlausibiltity(RemoteDeviceInfo remoteDevInfo, FirmwareImage firmwareImage);
    void clear();
    std::set<PlausibilityResult> getPlausibilityResult();

    bool plausibilityResultIsError(PlausibilityResult plauRes);
    QString plausibilityResultToStrShort(PlausibilityResult plauRes);
    QString plausibilityResultToStrReadable(PlausibilityResult plauRes);

    bool showWarngingMessage(); //cancel if false
private:
    std::set<PlausibilityResult> plausibilityResults;
};


class FlashResultDocumentation
{
public:
    FlashResultDocumentation();

    void save(CrystalBootSettings crystalBootSettings);
    void addActionResult(QString action,  RPC_RESULT RPC_result);
    void setNewFirmwareDescriptor(FirmwareImage firmwareImage);
    void setRemoteDeviceInfo(RemoteDeviceInfo remoteDeviceInfo);
    void print();
    void checkPlausibility();
    bool showWarngingMessage();


private:
    FirmwareUpdatePlausibilityCheck plausibilityResult;
    RemoteDeviceInfo remoteDeviceInfo;
    bool getOverAllResult();

    QMap<QString,RPC_RESULT> actionResults;
    FirmwareImage  firmwareImage;
};

#endif // FLASHRESULTDOCUMENTATION_H
