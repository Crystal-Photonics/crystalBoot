#include "firmwareencoder.h"

#include <QDebug>
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <QRegularExpression>
#include <QFile>
#include <QCryptographicHash>
#include <QBuffer>
#include "tiny-aes128-c_wrapper.h"
#include "aeskeyfile.h"

bool readHexFile(QString fileName, QByteArray &result, uint32_t &startAddress)
{
    intelhex ihex;
    std::ifstream intelHexFile;
    intelHexFile.open(fileName.toStdString(), std::ifstream::in);

    if(!intelHexFile.good())
    {
        qDebug() << "Error: couldn't open " << fileName;
        return false;
    }
    intelHexFile >> ihex;
    ihex.begin();
    startAddress = ihex.currentAddress();
    ihex.begin();
    uint8_t hexByte=0;
    result.clear();
    uint32_t oldAddress = ihex.currentAddress();
    while (ihex.getData(&hexByte)){
        uint32_t newAddress = ihex.currentAddress();    //fill "blank address spaces" with 0
        uint32_t diffAddress = newAddress-oldAddress;
        for(uint32_t diff=1; diff < diffAddress;diff++){
            uint8_t val = 0;
            result.append(val);
        }
        result.append(hexByte);
        oldAddress = newAddress;
        ++ihex;
    }
    int numberWarnings = ihex.getNoWarnings();
    if (numberWarnings){
        qDebug() << "reading hexfile number of warnings:" << numberWarnings;
        return false;
    }
    int numberErrors = ihex.getNoErrors();
    if (numberErrors){
        qDebug() << "reading hexfile number of errors:" << numberErrors;
        return false;
    }
    return true;
}


bool readBinFile(QString fileName, QByteArray &result)
{
    QFile firmwareFile( fileName);
    if (!firmwareFile.open( QIODevice::ReadOnly )){
        return false;
    }
    result = firmwareFile.readAll();
    return true;
}


FirmwareEncoder::FirmwareEncoder(ImageCreatorSettings imageCreatorSettings):imageCreatorSettings(imageCreatorSettings)
{
    fwImage.clear();
    srand(time(NULL));
}



bool FirmwareEncoder::parseTheDefine(QString defineName, DefineType defineType, QString line, QString &value)
{
    if (defineType == DefineType::None){
        return false;
    }

    line = line.left(line.indexOf("//"));
    QRegularExpression re("([^\\s]*)\\s*([^\\s]*)\\s*(.*)");
    QStringList elements = re.match(line).capturedTexts();

#if  1
    elements.removeFirst(); //remove full match
    if (elements.count() < 3){
        return false;
    }

    if (elements[0].toLower() != QString("#define")){
        return false;
    }
    if (elements[1] != defineName){
        return false;
    }

    QString tmp = elements[2];
    tmp = tmp.left(tmp.indexOf("//"));
    tmp = tmp.trimmed();
    if (defineType == DefineType::String){
        if ((tmp[0] == '"') && (tmp[tmp.length()-1] == '"')){
            value = tmp.mid(1,tmp.length()-2);
        }else{
            return false;
        }
    }else if (defineType == DefineType::Number){
        bool ok=false;
        int val = 0;
        val = tmp.toInt(&ok,0);
        if (ok){
            value = QString::number(val);
        }else{
            return false;
        }
    }
#else
    (void)defineName;
    (void)value;
#endif
    return true;
}

bool FirmwareEncoder::loadFirmwareData()
{
    fwImage.clear();

    bool firmware_githash_found = false;
    bool firmware_gitdate_found = false;
    bool firmware_version_found = false;
    bool firmware_name_found = false;
    bool exit_loop_due_error = false;
    for (int i = 0;i<imageCreatorSettings.headerFiles_abs.count();i++){
        QString filename = imageCreatorSettings.headerFiles_abs[i];
        //bool withiinComment=false;
        QFile file(filename);
        file.open(QIODevice::ReadOnly);
        if (!file.isOpen()){
            qDebug() << "cant open file " << imageCreatorSettings.headerFiles_abs[i];
            return false;
        }
        QTextStream in(&file);
        while(!in.atEnd()){
            QString line = in.readLine();
            line = line.left(line.indexOf("\\\\"));
            QString value;
            if (parseTheDefine(imageCreatorSettings.keyWord_gitdate,DefineType::Number, line, value)){
                int v = value.toInt();
                fwImage.firmware_gitdate = v;
                fwImage.firmware_gitdate_dt = QDateTime::fromTime_t(v);
                qDebug() << "found firmware_gitdate";
                if (firmware_gitdate_found){
                    qDebug() << "gitdate found more than once.";
                    exit_loop_due_error = true;
                    break;
                }
                firmware_gitdate_found = true;

            }else if (parseTheDefine(imageCreatorSettings.keyWord_githash, DefineType::Number, line, value)){
                fwImage.firmware_githash = value.toInt();
                qDebug() << "found firmware_githash";
                if (firmware_githash_found){
                    qDebug() << "githash found more than once.";
                    exit_loop_due_error = true;
                    break;
                }
                firmware_githash_found = true;
            }else if (parseTheDefine(imageCreatorSettings.keyWord_name,DefineType::String, line, value)){
                fwImage.firmware_name = value;
                qDebug() << "found firmware_name";
                if (firmware_name_found){
                    qDebug() << "firmware name found more than once.";
                    exit_loop_due_error = true;
                    break;
                }
                firmware_name_found = true;
            }else if (parseTheDefine(imageCreatorSettings.keyWord_version,DefineType::String,line, value) ){
                fwImage.firmware_version = value;
                qDebug() << "found firmware_version";
                if (firmware_version_found){
                    qDebug() << "firmware version found more than once.";
                    exit_loop_due_error = true;
                    break;
                }
                firmware_version_found = true;
            }
        }
        if (exit_loop_due_error){
            break;
        }
    }
    if (exit_loop_due_error){
        return false;
    }
    if (firmware_version_found && firmware_name_found && firmware_githash_found && firmware_gitdate_found){
        qDebug() << "found all fields";
    }
    qDebug() << "firmware_githash" << fwImage.firmware_githash;
    qDebug() << "firmware_gitdate" << fwImage.firmware_gitdate;
    qDebug() << "firmware_version" << fwImage.firmware_version;
    qDebug() << "firmware_name" << fwImage.firmware_name;

    if (!readHexFile(imageCreatorSettings.hexFileName_abs, fwImage.binary, fwImage.firmware_entryPoint)){
        qDebug() << "error loading hex file:" << fwImage.firmware_name;
        return false;
    }
    fwImage.firmware_size = fwImage.binary.size();

    QCryptographicHash sha256_check(QCryptographicHash::Sha256);


    QByteArray toBeChecked(fwImage.binary);

    sha256_check.addData(toBeChecked);
    fwImage.sha256 = sha256_check.result();

    fwImage.aes128_iv.clear();
    uint8_t aes_iv_buffer[16];
    for (size_t i = 0; i < sizeof aes_iv_buffer;i++){
        aes_iv_buffer[i] = rand() & 0xFF;
        fwImage.aes128_iv.append(aes_iv_buffer[i]);
    }
    uint8_t *p_aes_iv=aes_iv_buffer;

    AESKeyFile aeskeyfile;
    if (!aeskeyfile.open(imageCreatorSettings.encryptKeyFileName_abs)){
        qDebug() << "error loading key file:" << imageCreatorSettings.encryptKeyFileName_abs;
        return false;
    }

    if (!aeskeyfile.isValid()){
        qDebug() << "aes key file not valid";
        return false;
    }

    uint8_t aes_key_buffer[16];
    aeskeyfile.fillBuffer(aes_key_buffer);
    uint8_t *p_aes_key=aes_key_buffer;


    if (imageCreatorSettings.crypt == ImageCreatorSettings::Crypt::AES128){
        QBuffer inStream(&fwImage.binary);
        QByteArray outArray;
        uint8_t inbuffer[128];
        uint8_t outbuffer[128];
        inStream.open(QIODevice::ReadOnly);
        while (!inStream.atEnd()){
            inStream.read((char*)inbuffer,sizeof inbuffer);
            AES128_CBC_encrypt_buffer(outbuffer,inbuffer,sizeof inbuffer,p_aes_key,p_aes_iv);
            outArray.append((char*)outbuffer,sizeof outbuffer);
            p_aes_key=0;
            p_aes_iv=0;
        }
        fwImage.binary = outArray;
    }
    qDebug() << "entrypoint: 0x"+QString::number( fwImage.firmware_entryPoint,16);
    qDebug() << "size: "+QString::number( fwImage.firmware_size);

    qDebug() << "sha256: " << fwImage.sha256.toHex();
    return true;
}

bool FirmwareEncoder::saveImage()
{
    return fwImage.save(imageCreatorSettings.targetFileName_abs);
}

