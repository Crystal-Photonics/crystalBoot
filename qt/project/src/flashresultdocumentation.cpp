#include "flashresultdocumentation.h"
#include "namedplacemarker.h"
#include <QXmlStreamWriter>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QProcess>

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

