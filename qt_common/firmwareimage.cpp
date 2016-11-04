#include "firmwareimage.h"
#include <QXmlStreamWriter>
#include <QFile>
#include <QDomDocument>
#include <QDomNode>
#include <QDebug>
#include <QFileInfo>



FirmwareImage::FirmwareImage()
{
    clear();
}

void FirmwareImage::clear()
{
    firmware_githash = 0;
    firmware_gitdate = 0;
    firmware_gitdate_dt = QDateTime();
    firmware_version = "";
    firmware_name = "";

    firmware_entryPoint = 0;
    firmware_size = 0;

    crypto = Crypto::Plain;
    binary.clear();
    sha256.clear();
}

bool FirmwareImage::isValid()
{
    const uint32_t CHECKSUM_SIZE = 32;
    //signature check
    if(!firmware_githash){
        qDebug() << "firmware_githash is zero";
        return false;
    }

    if(!firmware_gitdate){
        qDebug() << "firmware_gitdate is zero";
        return false;
    }

    if(!firmware_entryPoint){
        qDebug() << "firmware_entryPoint is zero";
        return false;
    }

    if(!firmware_size){
        qDebug() << "firmware_size is zero";
        return false;
    }

    if(!binary.size()){
        qDebug() << "binary.size() is zero";
        return false;
    }

    if(((size_t)firmware_size != (size_t)binary.size()) && (crypto == Crypto::Plain)){
        qDebug() <<  "firmware_size doesnt equal binary.size()";
        return false;
    }


    if(sha256.size() != CHECKSUM_SIZE){
        qDebug() << "sha256 is not 32";
        return false;
    }

    return true ;
}

bool FirmwareImage::isFileModified()
{
    return lastModified != QFileInfo(fileName).lastModified();
}

QString FirmwareImage::getGitHash_str()
{
    return "0x"+QString::number(firmware_githash,16).toUpper();
}

bool FirmwareImage::save(QString targetFile){
    QXmlStreamWriter xml;
    QFile file(targetFile);

    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << "cant open file"<< targetFile;
        return false;
    }
    fileName = targetFile;
    xml.setDevice(&file);

    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE crystalBoot>");
    xml.writeStartElement("crystalBoot");
    xml.writeAttribute("version", "1.0");

    xml.writeStartElement("meta");
    xml.writeAttribute("githash", "0x"+QString::number(firmware_githash,16).toUpper());
    xml.writeAttribute("gitdate", QString::number(firmware_gitdate));
    xml.writeAttribute("firmware_version", firmware_version);
    xml.writeAttribute("firmware_name", firmware_name);
    xml.writeAttribute("firmware_entrypoint","0x"+QString::number(firmware_entryPoint,16).toUpper() );
    xml.writeAttribute("firmware_size",QString::number(firmware_size));
    xml.writeAttribute("creation_date",QDateTime(QDateTime::currentDateTime()).toString("yyyy.mm.dd HH:MM"));
    if (crypto == Crypto::Plain){
        xml.writeAttribute("crypto","plain");
    }else if(crypto == Crypto::AES128){
        xml.writeAttribute("crypto","aes");
    }
    xml.writeEndElement();



    QByteArray aes128_iv_64 = aes128_iv.toBase64();
    xml.writeTextElement("aes128_iv",aes128_iv_64.data() );

    QByteArray checkSumBinary64 = sha256.toBase64();
    xml.writeTextElement("sha256",checkSumBinary64.data() );
  //  xml.writeEndElement();

    QByteArray imageBinary64 = binary.toBase64();
    xml.writeTextElement("binary",imageBinary64.data() );
  //  xml.writeEndElement();

    xml.writeEndDocument();
    file.close();
    lastModified = QFileInfo(fileName).lastModified();
    return true;
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
     *      crypto = "plain"
     *  />
     *  <binary></binary>
     *</crystalBoot>*/




    QDomDocument doc("mydocument");
    QFile file(fileName);
    this->fileName = "";
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "cant open file as readonly";
        return false;
    }

    this->fileName = fileName;
    lastModified = QFileInfo(fileName).lastModified();

    if (!doc.setContent(&file)) {
        qDebug() << "cant set xml content";
        file.close();
        return false;
    }


    QDomNode crystalBootNode = doc.elementsByTagName("crystalBoot").at(0);
    if (crystalBootNode.isNull()){
       qDebug() << "crystalBoot does not exist";
        return false;
    }


    QDomElement metaNodeElement = crystalBootNode.firstChildElement("meta");
    if (metaNodeElement.isNull()){
        return false;
    }
    bool ok = true;
    firmware_githash = metaNodeElement.attribute("githash","").toInt(&ok,0);
    if (!ok) {
        qDebug() << "githash is not a number";
        return false;
    }
    firmware_gitdate = metaNodeElement.attribute("gitdate","").toInt(&ok,0);;
    firmware_gitdate_dt = QDateTime::fromTime_t(firmware_gitdate);
    if (!ok) {
        qDebug() << "gitdate is not a date/number";
        return false;
    }
    firmware_version = metaNodeElement.attribute("firmware_version","");

    firmware_name = metaNodeElement.attribute("firmware_name","");
    firmware_entryPoint = metaNodeElement.attribute("firmware_entrypoint","").toInt(&ok,0);
    if (!ok) {
        qDebug() << "firmware_entrypoint is not a number";
        return false;
    }
    firmware_size = metaNodeElement.attribute("firmware_size","").toInt(&ok,0);
    if (!ok) {
        qDebug() << "firmware_size is not a number";
        return false;
    }

    QString crpt = metaNodeElement.attribute("crypto","").toLower();
    if (crpt == "plain"){
        crypto = Crypto::Plain;
    }else if (crpt == "aes"){
        crypto = Crypto::AES128;
    }else{
        qDebug() << "no crypto type found";
        return false;
    }

    if (crypto == Crypto::AES128){
        QDomElement aes128_ivNodeElement = crystalBootNode.firstChildElement("aes128_iv");
        if (aes128_ivNodeElement.isNull()){
            qDebug() << "no iv found";
            return false;
        }
        QByteArray aes128_iv_64 = aes128_ivNodeElement.text().toUtf8();
        aes128_iv = QByteArray::fromBase64(aes128_iv_64);
    }else{
        aes128_iv = QByteArray(16,0);
    }

    QDomElement checksumNodeElement = crystalBootNode.firstChildElement("sha256");
    if (checksumNodeElement.isNull()){
        qDebug() << "no checksum found";
        return false;
    }
    QByteArray checksum_base64 = checksumNodeElement.text().toUtf8();
    sha256 = QByteArray::fromBase64(checksum_base64);


    QDomElement binNodeElement = crystalBootNode.firstChildElement("binary");
    if (binNodeElement.isNull()){
            qDebug() << "no binary found";
        return false;
    }
    QByteArray bin_base64 = binNodeElement.text().toUtf8();
    binary =  QByteArray::fromBase64(bin_base64);
#if 0
    QFile testFile("test.bin");
    testFile.open(QIODevice::WriteOnly);
    testFile.write(binary);
    testFile.close();
#endif

   // qDebug() <<"first byte" << binary.at(0);
   // qDebug() <<"size" << binary.size();

    return isValid();
}



QString FirmwareImage::getNameShort(int len)
{
    return firmware_name.left(len);
}

uint16_t FirmwareImage::getNameCRC16()
{
    return qChecksum(firmware_name.toStdString().c_str(),firmware_name.length());
}
