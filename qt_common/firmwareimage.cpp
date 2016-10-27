#include "firmwareimage.h"
#include <QXmlStreamWriter>
#include <QFile>
#include <QDomDocument>
#include <QDomNode>
#include <QDebug>



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

bool FirmwareImage::open(QString fileName)
{
    /*<crystalBoot version="1.0">
     *  <meta
     *      githash="0x997E583"
     *      gitdate="1477487255"
     *      firmware_version="0.5"
     *      firmware_name="test fm"
     *      firmware_entrypoint="0x8006000"
     *      firmware_size="229911"
     *      creation_date="2016.28.27 18:10"
     *  />
     *  <binary></binary>
     *</crystalBoot>*/

    QXmlStreamReader xml;

    QDomDocument doc("mydocument");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }


    QDomNode crystalBootNode = doc.elementsByTagName("crystalBoot").at(0);
    if (crystalBootNode.isNull()){
        return false;
    }


    QDomElement metaNodeElement = crystalBootNode.firstChildElement("meta");
    if (metaNodeElement.isNull()){
        return false;
    }
    bool ok = false;
    firmware_githash = metaNodeElement.attribute("githash","").toInt(&ok,0);
    if (!ok) {return false;}
    uint32_t unixdate = metaNodeElement.attribute("gitdate","").toInt(&ok,0);;
    firmware_gitdate.fromTime_t(unixdate);
    if (!ok) {return false;}
    firmware_version = metaNodeElement.attribute("firmware_version","");

    firmware_name = metaNodeElement.attribute("firmware_name","");
    firmware_entryPoint = metaNodeElement.attribute("firmware_entrypoint","").toInt(&ok,0);
    if (!ok) {return false;}
    firmware_size = metaNodeElement.attribute("firmware_size","").toInt(&ok,0);
    if (!ok) {return false;}

    QDomElement binNodeElement = crystalBootNode.firstChildElement("binary");
    QString bin64_str = binNodeElement.text();
    QByteArray bin_base64 = bin64_str.toUtf8();
    //bin_base64 = bin64_str.c_str();
    binary =  QByteArray::fromBase64(bin_base64);

    QFile testFile("test.bin");
    testFile.open(QIODevice::WriteOnly);
    testFile.write(binary);
    testFile.close();

    qDebug() <<"first byte" << binary.at(0);
    qDebug() <<"size" << binary.size();
    return true;
}
