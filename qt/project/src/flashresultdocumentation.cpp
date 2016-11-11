#include "flashresultdocumentation.h"
#include "namedplacemarker.h"
#include <QXmlStreamWriter>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QApplication>

QString RemoteDeviceInfo::arrayToHexString(uint8_t *buff, const size_t length, int insertNewLine){
    QString result;
    int insNewLineCounter=insertNewLine;
    for (size_t i=0;i<length;i++){
        result += "0x"+QString::number(buff[i],16).toUpper();
        if (i<length-1){
            result += ", ";
            if (insertNewLine){
                insNewLineCounter--;
                if (insNewLineCounter==0){
                    result += "\n";
                    insNewLineCounter = insertNewLine;
                }
            }
        }
    }
    return result;
}

FlashResultDocumentation::FlashResultDocumentation()
{

}

void FlashResultDocumentation::checkPlausibility()
{
    plausibilityResult.checkPlausibiltity(remoteDeviceInfo,firmwareImage);
}

bool FlashResultDocumentation::showWarngingMessage()
{
    return plausibilityResult.showWarngingMessage();
}


void FlashResultDocumentation::save(CrystalBootSettings crystalBootSettings)
{
    QXmlStreamWriter xml;

    NamedPlaceMarker npm;

    QString fileName=crystalBootSettings.flashResultDocumentationPath;

    npm.addVariable("%IMAGE_FILE_PATH%",QFileInfo(firmwareImage.fileName).absoluteDir().absolutePath());
    npm.addVariable("%FIRMWARE_GIT_HASH%",firmwareImage.getGitHash_str());
    npm.addVariable("%FIRMWARE_NAME%",firmwareImage.firmware_name);
    npm.addVariable("%FIRMWARE_VERSION%",firmwareImage.firmware_version);
    npm.addVariable("%OK_OR_FAIL%",getOverAllResult() ? "OK" : "FAIL");
    npm.addVariable("%DATE_TIME%",QDateTime::currentDateTime().toString("yyyy_MM_dd__HH_mm"));
    npm.addVariable("%MCU_GUID%",remoteDeviceInfo.getMCU_UniqueIDString());

    if (fileName == ""){
        return;
    }
    fileName = npm.replace(fileName);

    QDir(QDir::currentPath()).mkpath(QFileInfo(fileName).absoluteDir().absolutePath());

    QFile file(fileName);




    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << "cant open file"<< fileName;
        return ;
    }
    xml.setDevice(&file);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE crystalBoot_Flash_Result>");
    xml.writeStartElement("crystalBootFlashResult");
    xml.writeAttribute("version", "1.0");

    xml.writeStartElement("mcu");
    xml.writeAttribute("devID", remoteDeviceInfo.getMCU_DeviceIDString());
    xml.writeAttribute("revision", remoteDeviceInfo.getMCU_RevString());
    xml.writeAttribute("flashSize", remoteDeviceInfo.getMCU_FlashsizeString());
    xml.writeAttribute("uniqueID", remoteDeviceInfo.getMCU_UniqueIDString());
    xml.writeAttribute("availableFlashSize", remoteDeviceInfo.getMCU_AvailFlashSizeString());
    xml.writeAttribute("entryPoint", remoteDeviceInfo.getMCU_EntryPointString());
    xml.writeAttribute("MinimalEntryPoint", remoteDeviceInfo.getMCU_MinimalEntryPointString());
    xml.writeAttribute("CryptoRequired", remoteDeviceInfo.getMCU_cryptoRequired());
    xml.writeAttribute("ProtectionLevel", remoteDeviceInfo.getMCU_ProtectionLevel());
    xml.writeEndElement();

    xml.writeStartElement("bootloader");
    xml.writeAttribute("githash", remoteDeviceInfo.getDEV_gitHash());
    xml.writeAttribute("gitdate", remoteDeviceInfo.getDEV_gitDate_str());
    xml.writeAttribute("gitdate_unix", remoteDeviceInfo.getDEV_gitDate_unix());
    xml.writeAttribute("version",  remoteDeviceInfo.getDEV_version());
    xml.writeAttribute("name",  remoteDeviceInfo.getDEV_name());
    xml.writeEndElement();

    xml.writeStartElement("oldFirmware");
    xml.writeAttribute("githash", remoteDeviceInfo.getFW_gitHash());
    xml.writeAttribute("gitdate", remoteDeviceInfo.getFW_gitDate_str());
    xml.writeAttribute("gitdate_unix", remoteDeviceInfo.getFW_gitDate_unix());
    xml.writeAttribute("version",  remoteDeviceInfo.getFW_version());
    xml.writeAttribute("name",  remoteDeviceInfo.getFW_name());
    xml.writeAttribute("name_crc",  remoteDeviceInfo.getFW_nameCRC());
    xml.writeAttribute("entryPoint", remoteDeviceInfo.getFW_entryPoint());
    xml.writeAttribute("size_hex", remoteDeviceInfo.getFW_size());
    xml.writeAttribute("size_dec", QString::number(remoteDeviceInfo.firmwareDescriptor.size));
    xml.writeEndElement();

    xml.writeStartElement("newFirmware");
    xml.writeAttribute("isValid", firmwareImage.isValid() ? "true" : "false");
    xml.writeAttribute("sha256_for_verify", firmwareImage.sha256.toHex());
    xml.writeAttribute("githash", firmwareImage.getGitHash_str());
    xml.writeAttribute("gitdate", firmwareImage.firmware_gitdate_dt.toString("yyyy.MM.dd HH:mm"));
    xml.writeAttribute("gitdate_unix", QString::number(firmwareImage.firmware_githash));
    xml.writeAttribute("version",  firmwareImage.firmware_version);
    xml.writeAttribute("name",  firmwareImage.firmware_name);
    xml.writeAttribute("name_crc",  "0x"+QString::number(firmwareImage.getNameCRC16(),16).toUpper());
    xml.writeAttribute("entryPoint", "0x"+QString::number(firmwareImage.firmware_entryPoint,16).toUpper());
    xml.writeAttribute("size_hex", "0x"+QString::number(firmwareImage.firmware_size,16).toUpper());
    xml.writeAttribute("size_dec", QString::number(firmwareImage.firmware_size));

    if (firmwareImage.crypto == FirmwareImage::Crypto::Plain){
        xml.writeAttribute("crypto","plain");
    }else if(firmwareImage.crypto == FirmwareImage::Crypto::AES128){
        xml.writeAttribute("crypto","aes");
    }
    xml.writeEndElement();

    checkPlausibility();
    std::set<PlausibilityResult> plauResult = plausibilityResult.getPlausibilityResult();
    xml.writeStartElement("plausibility");
    std::set<PlausibilityResult>::iterator it;
    for (it = plauResult.begin(); it != plauResult.end(); ++it)
    {
        PlausibilityResult res = *it; // Note the "*" here
        xml.writeStartElement(plausibilityResult.plausibilityResultToStrShort(res));

        if (plausibilityResult.plausibilityResultIsError(res)){
            xml.writeAttribute("error",  "true");
            xml.writeAttribute("warning",  "false");
        }else{
            xml.writeAttribute("error",  "false");
            xml.writeAttribute("warning",  "true");
        }
        xml.writeAttribute("description",plausibilityResult.plausibilityResultToStrReadable(res));
        xml.writeEndElement();
    }
    xml.writeEndElement();


    xml.writeStartElement("flashResult");
    xml.writeAttribute("date_unix",QString::number(QDateTime::currentDateTime().toTime_t()));
    xml.writeAttribute("date",QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm"));
    QMap<QString,RPC_RESULT> ::iterator i;

    for (i = actionResults.begin(); i != actionResults.end(); ++i){
        QString valStr;
        RPC_RESULT rpcResult = i.value();
        switch (rpcResult){
        case RPC_SUCCESS:
            valStr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            valStr = "RPC_FAILURE";

            break;
        case RPC_COMMAND_UNKNOWN:
            valStr = "RPC_COMMAND_UNKNOWN";

            break;
        case RPC_COMMAND_INCOMPLETE:
            valStr = "RPC_COMMAND_INCOMPLETE";

            break;
        }

        xml.writeAttribute(i.key(),valStr);
    }
    xml.writeAttribute("overAllResult",getOverAllResult() ? "true" : "false");
    xml.writeEndElement();


    xml.writeEndElement();
    xml.writeEndDocument();
    file.close();

    npm.clear();
    QDir dir(QDir::currentPath());
    npm.addVariable("%PATH_TO_XML_REPORT%",dir.absoluteFilePath(fileName));
    if (crystalBootSettings.callProcessAfterProgrammuing){
        QStringList arguments;
        arguments.append(npm.replace(crystalBootSettings.callProcessArguments));
        QString proFileName = crystalBootSettings.callProcessPath;
        QProcess proc;
        proc.execute(proFileName,arguments);
        proc.waitForStarted();
        proc.waitForFinished();
    }
    return ;
}

void FlashResultDocumentation::addActionResult(QString action, RPC_RESULT RPC_result)
{
    actionResults.insert(action,RPC_result);
}

void FlashResultDocumentation::setNewFirmwareDescriptor(FirmwareImage firmwareImage)
{
    this->firmwareImage = firmwareImage;
}

void FlashResultDocumentation::setRemoteDeviceInfo(RemoteDeviceInfo remoteDeviceInfo)
{
    this->remoteDeviceInfo = remoteDeviceInfo;
}

void FlashResultDocumentation::print()
{
    qDebug() << "mcu data:";
    qDebug() << "devID"<< remoteDeviceInfo.getMCU_DeviceIDString();
    qDebug() << "revision"<<  remoteDeviceInfo.getMCU_RevString();
    qDebug() << "flashSize"<<  remoteDeviceInfo.getMCU_FlashsizeString();
    qDebug() << "uniqueID"<<  remoteDeviceInfo.getMCU_UniqueIDString();
    qDebug() << "availableFlashSize"<<  remoteDeviceInfo.getMCU_AvailFlashSizeString();
    qDebug() << "entryPoint"<<  remoteDeviceInfo.getMCU_EntryPointString();
    qDebug() << "MinimalEntryPoint"<<  remoteDeviceInfo.getMCU_MinimalEntryPointString();
    qDebug() << "CryptoRequired"<<  remoteDeviceInfo.getMCU_cryptoRequired();
    qDebug() << "";

    qDebug() << "bootloader info:";
    qDebug() << "githash"<<  remoteDeviceInfo.getDEV_gitHash();
    qDebug() << "gitdate"<<  remoteDeviceInfo.getDEV_gitDate_str();
    qDebug() << "gitdate_unix"<<  remoteDeviceInfo.getDEV_gitDate_unix();
    qDebug() << "version"<<   remoteDeviceInfo.getDEV_version();
    qDebug() << "name"<<   remoteDeviceInfo.getDEV_name();
    qDebug() << "";

    qDebug() << "remote firmware:";
    qDebug() << "githash"<<  remoteDeviceInfo.getFW_gitHash();
    qDebug() << "gitdate"<<  remoteDeviceInfo.getFW_gitDate_str();
    qDebug() << "gitdate_unix"<<  remoteDeviceInfo.getFW_gitDate_unix();
    qDebug() << "version"<<   remoteDeviceInfo.getFW_version();
    qDebug() << "name"<<   remoteDeviceInfo.getFW_name();
    qDebug() << "name_crc"<<   remoteDeviceInfo.getFW_nameCRC();
    qDebug() << "entryPoint"<<  remoteDeviceInfo.getFW_entryPoint();
    qDebug() << "size_hex"<<  remoteDeviceInfo.getFW_size();
    qDebug() << "size_dec"<<  QString::number(remoteDeviceInfo.firmwareDescriptor.size);

    qDebug() << "";
}


bool FlashResultDocumentation::getOverAllResult()
{
    bool result = true;
    QMap<QString,RPC_RESULT> ::iterator i;
    for (i = actionResults.begin(); i != actionResults.end(); ++i){
        if (i.value() != RPC_SUCCESS){
            result = false;
        }
    }
    return result;
}



RemoteDeviceInfo::RemoteDeviceInfo()
{
#if 1
    memset(&mcu_descriptor,0,sizeof  mcu_descriptor);
    memset(&deviceDescriptor,0,sizeof  deviceDescriptor);
    memset(&firmwareDescriptor,0,sizeof  firmwareDescriptor);
#endif
    unSet();
}

void RemoteDeviceInfo::setMCUDescriptor(mcu_descriptor_t *mcu_descriptor)
{
    memcpy(&this->mcu_descriptor,mcu_descriptor,sizeof this->mcu_descriptor);
    mcu_set = true;

}

void RemoteDeviceInfo::setDeviceDescriptor(device_descriptor_v1_t *deviceDescriptor)
{
    memcpy(&this->deviceDescriptor,deviceDescriptor,sizeof this->deviceDescriptor);
    device_set = true;

}

void RemoteDeviceInfo::setOldFirmwareDescriptor(firmware_descriptor_t *firmwareDescriptor)
{
    memcpy(&this->firmwareDescriptor,firmwareDescriptor,sizeof this->firmwareDescriptor);
    firmware_set = true;
}

QString RemoteDeviceInfo::getDEV_name()
{
    char blName[100];
    memset(blName,0, sizeof blName);
    memcpy(blName,deviceDescriptor.name, sizeof deviceDescriptor.name);
    return QString(blName);
}

QString RemoteDeviceInfo::getDEV_version()
{
    char blVersion[100];
    memset(blVersion,0, sizeof blVersion);
    memcpy(blVersion,deviceDescriptor.version, sizeof deviceDescriptor.version);
    return QString(blVersion);
}



QString RemoteDeviceInfo::getMCU_DeviceIDString(){
    return QString("0x")+QString::number(mcu_descriptor.devID,16).toUpper();
}

QString RemoteDeviceInfo::getMCU_ProtectionLevel()
{
    return QString::number(mcu_descriptor.protectionLevel);
}

QString RemoteDeviceInfo::getMCU_RevString(){
    return QString("0x")+QString::number(mcu_descriptor.revision,16).toUpper();
}

QString RemoteDeviceInfo::getMCU_FlashsizeString(){
    return QString("0x")+QString::number(mcu_descriptor.flashsize,16).toUpper();
}

QString RemoteDeviceInfo::getMCU_UniqueIDString(int length){
    return arrayToHexString(mcu_descriptor.guid,sizeof mcu_descriptor.guid,length);
}

QString RemoteDeviceInfo::getMCU_UniqueIDString(){
    QByteArray ba((char*)&mcu_descriptor.guid,sizeof mcu_descriptor.guid);
    return ba.toHex();
}

QString RemoteDeviceInfo::getMCU_AvailFlashSizeString(){
    return QString("0x")+QString::number(mcu_descriptor.availFlashSize,16).toUpper();
}

QString RemoteDeviceInfo::getMCU_EntryPointString(){
    return QString("0x")+QString::number(mcu_descriptor.firmwareEntryPoint,16).toUpper();
}

QString RemoteDeviceInfo::getMCU_MinimalEntryPointString(){
    return QString("0x")+QString::number(mcu_descriptor.minimalFirmwareEntryPoint,16).toUpper();
}

QString RemoteDeviceInfo::getMCU_cryptoRequired(){
    if (mcu_descriptor.cryptoRequired){
        return "true";
    }else{
        return "false"    ;
    }
}

QString RemoteDeviceInfo::getDEV_gitDate_str(){
    return QDateTime::fromTime_t(deviceDescriptor.gitDate_unix).toString("yyyy.MM.dd");
}

QString RemoteDeviceInfo::getDEV_gitDate_unix(){
    return QString::number( deviceDescriptor.gitDate_unix);
}


QString RemoteDeviceInfo::getDEV_gitHash(){
    return "0x"+QString::number(deviceDescriptor.githash,16).toUpper();
}


QString RemoteDeviceInfo::getDEV_deviceID(){
    return QString::number(deviceDescriptor.deviceID);
}

QString RemoteDeviceInfo::getFW_gitHash(){
    return "0x"+QString::number(firmwareDescriptor.githash,16).toUpper();
}


QString RemoteDeviceInfo::getFW_gitDate_str(){
    return QDateTime::fromTime_t(firmwareDescriptor.gitDate_unix).toString("yyyy.MM.dd HH:mm");
}

QString RemoteDeviceInfo::getFW_gitDate_unix(){
    return QString::number( firmwareDescriptor.gitDate_unix);
}

QString RemoteDeviceInfo::getFW_nameCRC(){
    return "0x"+QString::number(firmwareDescriptor.nameCRC16,16).toUpper();
}

QString RemoteDeviceInfo::getFW_name()
{

    char rfName[100];
    memset(rfName,0, sizeof rfName);
    memcpy(rfName,firmwareDescriptor.name, sizeof firmwareDescriptor.name);
    return QString(rfName);
}

QString RemoteDeviceInfo::getFW_version()
{
    char rfVersion[100];
    memset(rfVersion,0, sizeof rfVersion);
    memcpy(rfVersion,firmwareDescriptor.version, sizeof firmwareDescriptor.version);
    return QString(rfVersion);
}

QString RemoteDeviceInfo::getFW_entryPoint(){
    return "0x"+QString::number(firmwareDescriptor.entryPoint,16).toUpper();
}

QString RemoteDeviceInfo::getFW_size(){
    return "0x"+QString::number(firmwareDescriptor.size,16).toUpper();
}


QString RemoteDeviceInfo::getDeviceCategorieString()
{
    QString cat;
    if (mcu_descriptor.devID == 0x416){
        cat = "Cat.1 device";
    }else if(mcu_descriptor.devID == 0x429){
        cat = "Cat.2 device";
    }else if(mcu_descriptor.devID == 0x427){
        cat = "Cat.3 device";
    }else if(mcu_descriptor.devID == 0x436){
        cat = "Cat.4 device or Cat.3 device";
    }else if(mcu_descriptor.devID == 0x437){
        cat = "Cat.5 device or Cat.6 device";
    }
    return cat;
}

void RemoteDeviceInfo::unSet()
{
    mcu_set = false;
    device_set = false;
    firmware_set = false;

    memset(&mcu_descriptor,0, sizeof mcu_descriptor);
    memset(&deviceDescriptor,0, sizeof deviceDescriptor);
    memset(&firmwareDescriptor,0, sizeof firmwareDescriptor);
}

bool RemoteDeviceInfo::isValid()
{
    return mcu_set && device_set && firmware_set;
}



FirmwareUpdatePlausibilityCheck::FirmwareUpdatePlausibilityCheck()
{

}

void FirmwareUpdatePlausibilityCheck::checkPlausibiltity(RemoteDeviceInfo remoteDevInfo, FirmwareImage firmwareImage)
{
    plausibilityResults.clear();
    uint32_t fw_lastAddress = firmwareImage.firmware_entryPoint+firmwareImage.firmware_size;
    uint32_t mcu_lastAddress = remoteDevInfo.mcu_descriptor.minimalFirmwareEntryPoint +  remoteDevInfo.mcu_descriptor.availFlashSize;
    if (fw_lastAddress > mcu_lastAddress){
        plausibilityResults.insert(PlausibilityResult::error_firmwareimage_too_big);
    }

    if ((firmwareImage.firmware_entryPoint != remoteDevInfo.mcu_descriptor.firmwareEntryPoint) && (remoteDevInfo.mcu_descriptor.firmwareEntryPoint != 0xFFFFFFFF) ){
        plausibilityResults.insert(PlausibilityResult::error_wrong_entrypoint);
    }

    if (firmwareImage.firmware_entryPoint < remoteDevInfo.mcu_descriptor.minimalFirmwareEntryPoint){
        plausibilityResults.insert(PlausibilityResult::error_wrong_entrypoint);
    }

    if ((firmwareImage.crypto == FirmwareImage::Crypto::Plain) && (remoteDevInfo.mcu_descriptor.cryptoRequired)){
        plausibilityResults.insert(PlausibilityResult::error_crypto_required);
    }
    if (firmwareImage.firmwareLoadConsistency == false){
         plausibilityResults.insert(PlausibilityResult::error_inconsistency);
    }
    if (firmwareImage.getNameCRC16() != remoteDevInfo.firmwareDescriptor.nameCRC16){
         plausibilityResults.insert(PlausibilityResult::warning_different_name_hash);
    }

    if (firmwareImage.firmware_gitdate_unix < remoteDevInfo.firmwareDescriptor.gitDate_unix){
         plausibilityResults.insert(PlausibilityResult::warning_downgrade_date);
    }

    if (firmwareImage.firmware_githash == remoteDevInfo.firmwareDescriptor.githash){
         plausibilityResults.insert(PlausibilityResult::warning_equal_gitHash);
    }

    if ((firmwareImage.firmware_githash == remoteDevInfo.firmwareDescriptor.githash) && (firmwareImage.firmware_version != remoteDevInfo.getFW_version())){
         plausibilityResults.insert(PlausibilityResult::warning_equal_gitHash_but_different_wersion);
    }

    if ((firmwareImage.firmware_githash != remoteDevInfo.firmwareDescriptor.githash) && (firmwareImage.firmware_version == remoteDevInfo.getFW_version())){
         plausibilityResults.insert(PlausibilityResult::warning_equal_version_but_different_gitHash);
    }
}

void FirmwareUpdatePlausibilityCheck::clear()
{
    plausibilityResults.clear();
}

std::set<PlausibilityResult> FirmwareUpdatePlausibilityCheck::getPlausibilityResult()
{
    return plausibilityResults;
}

QString FirmwareUpdatePlausibilityCheck::plausibilityResultToStrReadable(PlausibilityResult plauRes)
{
    switch (plauRes){
    case PlausibilityResult::error_firmwareimage_too_big:
        return QString("error: firmwareimage too big.");
        break;
    case PlausibilityResult::error_wrong_entrypoint:
        return QString("error: wrong entrypoint.");
        break;
    case PlausibilityResult::error_inconsistency:
        return QString("error: firmwareimage inconsistent.");
        break;
    case PlausibilityResult::error_crypto_required:
        return QString("error: crypto required.");
        break;
    case PlausibilityResult::warning_different_name_hash:
        return QString("warning: different name hashes. Installed Firmware has different name than this one. You are installing a different software.");
        break;
    case PlausibilityResult::warning_downgrade_date:
        return QString("warning: downgrading firmware. Installed Firmware newer than this one.");
        break;

    case PlausibilityResult::warning_equal_gitHash:
        return QString("warning: equal githash, you are installing the same version.");
        break;
    case PlausibilityResult::warning_equal_gitHash_but_different_wersion:
        return QString("warning: equal githash, but different version. Shouldnt happen.");
        break;
    case PlausibilityResult::warning_equal_version_but_different_gitHash:
        return QString("warning: equal version, but different githash. Shouldnt happen.");
        break;
    }
    return "";
}

bool FirmwareUpdatePlausibilityCheck::showWarngingMessage()//cancel if false
{

    QString warnings;
    std::set<PlausibilityResult>::iterator it;
    bool hasError = false;
    for (it = plausibilityResults.begin(); it != plausibilityResults.end(); ++it)
    {
        PlausibilityResult res = *it; // Note the "*" here
        warnings += plausibilityResultToStrReadable(res)+"\n";
        if (plausibilityResultIsError(res)){
            hasError = true;
        }
    }
    if (QApplication::activeWindow()){
        QMessageBox msgBox;
        if (hasError){
            msgBox.setText("There are plausibility errors:");
            msgBox.setInformativeText(warnings);
            msgBox.setStandardButtons(QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            msgBox.setIcon(QMessageBox::Critical);
        }else{
            msgBox.setText("There are plausibility warnings, continue?");
            msgBox.setInformativeText(warnings);
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Warning);
        }
        int ret = msgBox.exec();
        if (ret == QMessageBox::Ok){
            return true;
        }else{
            return false;
        }
    }else{
        if (warnings.length()){
            qDebug() << warnings;
        }
        return !hasError;
    }

}

QString FirmwareUpdatePlausibilityCheck::plausibilityResultToStrShort(PlausibilityResult plauRes)
{
    switch (plauRes){
    case PlausibilityResult::error_firmwareimage_too_big:
        return QString("error_firmwareimage_too_big");
        break;
    case PlausibilityResult::error_wrong_entrypoint:
        return QString("error_wrong_entrypoint");
        break;
    case PlausibilityResult::error_inconsistency:
        return QString("error_inconsistency");
        break;
    case PlausibilityResult::error_crypto_required:
        return QString("error_crypto_required");
        break;
    case PlausibilityResult::warning_different_name_hash:
        return QString("warning_different_name_hash");
        break;
    case PlausibilityResult::warning_downgrade_date:
        return QString("warning_downgrade_date");
        break;

    case PlausibilityResult::warning_equal_gitHash:
        return QString("warning_equal_gitHash");
        break;
    case PlausibilityResult::warning_equal_gitHash_but_different_wersion:
        return QString("warning_equal_gitHash_but_different_wersion");
        break;
    case PlausibilityResult::warning_equal_version_but_different_gitHash:
        return QString("warning_equal_gitHash_but_different_wersion");
        break;
    }
    return "";
}

bool FirmwareUpdatePlausibilityCheck::plausibilityResultIsError(PlausibilityResult plauRes)
{
    switch (plauRes){
    case PlausibilityResult::error_firmwareimage_too_big:
        return true;
        break;
    case PlausibilityResult::error_wrong_entrypoint:
        return true;
        break;
    case PlausibilityResult::error_inconsistency:
        return true;
        break;
    case PlausibilityResult::error_crypto_required:
        return true;
        break;
    case PlausibilityResult::warning_different_name_hash:
        return false;
        break;
    case PlausibilityResult::warning_downgrade_date:
        return false;
        break;

    case PlausibilityResult::warning_equal_gitHash:
        return false;
        break;
    case PlausibilityResult::warning_equal_gitHash_but_different_wersion:
        return false;
        break;
    case PlausibilityResult::warning_equal_version_but_different_gitHash:
        return false;
        break;
    }
    return true;
}



























