#include "firmwareimage.h"
#include <QXmlStreamWriter>
#include <QFile>


FirmwareImage::FirmwareImage()
{
    clear();
}

void FirmwareImage::clear()
{
    firmware_githash = 0;
    firmware_gitdate = QDateTime();
    firmware_version = "";
    firmware_name = "";
}


void FirmwareImage::save(QString targetFile){
    QXmlStreamWriter xml;
    QFile file(targetFile);
    file.open(QIODevice::WriteOnly);

    xml.setDevice(&file);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE crystalBoot>");
    xml.writeStartElement("crystalBoot");
    xml.writeAttribute("version", "1.0");

    xml.writeStartElement("meta");
    xml.writeAttribute("githash", "0x"+QString::number(firmware_githash,16).toUpper());
    xml.writeAttribute("gitdate", QString::number(firmware_gitdate.toTime_t()));
    xml.writeAttribute("firmware_version", firmware_version);
    xml.writeAttribute("firmware_name", firmware_name);
    xml.writeAttribute("firmware_entrypoint","0x"+QString::number(firmware_entryPoint,16).toUpper() );
    xml.writeAttribute("firmware_size",QString::number(firmware_size));
    xml.writeAttribute("creation_date",QDateTime(QDateTime::currentDateTime()).toString("yyyy.mm.dd HH:MM"));
    xml.writeEndElement();

    QByteArray imageBinary64 = binary.toBase64();
    xml.writeTextElement("binary",imageBinary64.data() );
    xml.writeEndElement();
    xml.writeEndDocument();
}
