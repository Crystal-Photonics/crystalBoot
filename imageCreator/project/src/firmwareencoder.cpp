#include "firmwareencoder.h"

#include <QDebug>
#include <iostream>
#include <fstream>
#include <QXmlStreamWriter>
#include <QRegularExpression>
#include <QFile>

FirmwareEncoder::FirmwareEncoder(ImageCreatorSettings imageCreatorSettings):imageCreatorSettings(imageCreatorSettings)
{
    fwImage.clear();
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

void FirmwareEncoder::fetchMetaData()
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
        }else{
            QTextStream in(&file);
            while(!in.atEnd()){
                QString line = in.readLine();
                line = line.left(line.indexOf("\\\\"));
                QString value;
                if (parseTheDefine(imageCreatorSettings.keyWord_gitdate,DefineType::Number, line, value)){
                    int v = value.toInt();
                    fwImage.firmware_gitdate = QDateTime::fromTime_t(v);
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
    }
    if (firmware_version_found && firmware_name_found && firmware_githash_found && firmware_gitdate_found){
        qDebug() << "found all fields";
    }
    qDebug() << "firmware_githash" << fwImage.firmware_githash;
    qDebug() << "firmware_gitdate" << fwImage.firmware_gitdate;
    qDebug() << "firmware_version" << fwImage.firmware_version;
    qDebug() << "firmware_name" << fwImage.firmware_name;

    std::ifstream intelHexFile;
    intelHexFile.open(imageCreatorSettings.hexFileName_abs.toStdString(), std::ifstream::in);

    if(!intelHexFile.good())
    {
        qDebug() << "Error: couldn't open " << imageCreatorSettings.hexFileName_abs;
        //usage();
    }
    intelHexFile >> ihex;
    ihex.begin();
    fwImage.firmware_entryPoint = ihex.currentAddress();
    ihex.end();
    fwImage.firmware_size = ihex.currentAddress()-fwImage.firmware_entryPoint;
    qDebug() << "entrypoint: 0x"+QString::number( fwImage.firmware_entryPoint,16);
    qDebug() << "size: "+QString::number( fwImage.firmware_size);
}


void FirmwareEncoder::createImage()
{

    QXmlStreamWriter xml;
    QFile file(imageCreatorSettings.targetFileName_abs);
    file.open(QIODevice::WriteOnly);

    xml.setDevice(&file);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE crystalBoot>");
    xml.writeStartElement("crystalBoot");
    xml.writeAttribute("version", "1.0");

    xml.writeStartElement("meta");
    xml.writeAttribute("githash", "0x"+QString::number(fwImage.firmware_githash,16).toUpper());
    xml.writeAttribute("gitdate", QString::number(fwImage.firmware_gitdate.toTime_t()));
    xml.writeAttribute("firmware_version", fwImage.firmware_version);
    xml.writeAttribute("firmware_name", fwImage.firmware_name);
    xml.writeAttribute("firmware_entrypoint","0x"+QString::number(fwImage.firmware_entryPoint,16).toUpper() );
    xml.writeAttribute("firmware_size",QString::number(fwImage.firmware_size));
    xml.writeEndElement();
    ihex.begin();
    uint8_t hexByte=0;
    QByteArray imageBinary;//(firmware_size,0);
    while (ihex.getData(&hexByte)){
        imageBinary.append(hexByte);
        ++ihex;
    }
    QByteArray imageBinary64 = imageBinary.toBase64();
    xml.writeTextElement("binary",imageBinary64.data() );
    qDebug() << imageBinary.size();
    xml.writeEndElement();
    xml.writeEndDocument();
}
