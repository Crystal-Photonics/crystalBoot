#include "firmwareimage.h"
#include <QXmlStreamWriter>
#include <QFile>
#include <QDomDocument>
#include <QDomNode>
#include <QDebug>
#include <QFileInfo>
#include <QCryptographicHash>



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
    firmwareLoadConsistency = false;
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

void FirmwareImage::createXML_for_checksum(QBuffer &buf)
{
    QXmlStreamWriter xml;
    xml.setDevice(&buf);

    xml.writeStartElement("meta");
    xml.writeAttribute("githash", "0x"+QString::number(firmware_githash,16).toUpper());
    xml.writeAttribute("gitdate", QString::number(firmware_gitdate));
    xml.writeAttribute("firmware_version", firmware_version);
    xml.writeAttribute("firmware_name", firmware_name);
    xml.writeAttribute("firmware_entrypoint","0x"+QString::number(firmware_entryPoint,16).toUpper() );
    xml.writeAttribute("firmware_size",QString::number(firmware_size));
    xml.writeAttribute("creation_date",imageCreationDate.toString("yyyy.MM.dd HH:mm"));
    if (crypto == Crypto::Plain){
        xml.writeAttribute("crypto","plain");
    }else if(crypto == Crypto::AES128){
        xml.writeAttribute("crypto","aes");
    }
    xml.writeEndElement();

    QByteArray aes128_iv_64 = aes128_iv.toBase64();
    xml.writeTextElement("aes128_iv",aes128_iv_64.data() );

    QByteArray checkSumBinary64 = sha256.toBase64();
    xml.writeTextElement("sha256_for_verify",checkSumBinary64.data() );

    QByteArray imageBinary64 = binary.toBase64();
    xml.writeTextElement("binary",imageBinary64.data() );

    xml.writeEndDocument();
}

QString FirmwareImage::getBase64CheckSum(QBuffer &bufferForCheckSum){
    bufferForCheckSum.open(QIODevice::ReadOnly);
    QCryptographicHash hashfunction(QCryptographicHash::Sha256);
    hashfunction.addData(&bufferForCheckSum);
    QByteArray hashValue = hashfunction.result();
    QString hashValue_64 = hashValue.toBase64();
    bufferForCheckSum.close();
    return hashValue_64;
}

bool FirmwareImage::save(QString targetFile){

    QFile file(targetFile);

    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << "cant open file"<< targetFile;
        return false;
    }
    fileName = targetFile;

    imageCreationDate = QDateTime::currentDateTime();
    QBuffer bufferForCheckSum;
    bufferForCheckSum.open(QIODevice::WriteOnly);
    createXML_for_checksum(bufferForCheckSum);
    bufferForCheckSum.close();
#if 0
    {
        bufferForCheckSum.open(QIODevice::ReadOnly);
        QFile testData("test_enc.xml");
        testData.open(QIODevice::WriteOnly);
        testData.write(bufferForCheckSum.readAll());
        testData.close();
        bufferForCheckSum.close();
    }
#endif
    QString hashValue_64 = getBase64CheckSum(bufferForCheckSum);

    bufferForCheckSum.open(QIODevice::ReadOnly);
    QString xmlData = bufferForCheckSum.readAll();
    bufferForCheckSum.close();

    {
        QXmlStreamWriter xml;

        xml.setDevice(&file);
        xml.writeStartDocument();
        xml.writeDTD("<!DOCTYPE crystalBoot>");
    }
    {
        QTextStream stream(&file);
        stream << "<crystalBoot version=\"1.0\">";
        stream << "<data>" << xmlData <<"</data>";
        stream << "<check>" << hashValue_64 << "</check>";
        stream << "</crystalBoot>";

    }

    file.close();
    lastModified = QFileInfo(fileName).lastModified();
    return true;
}

bool FirmwareImage::open(QString fileName)
{
    /*
     * <?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE crystalBoot>
<crystalBoot version="1.0">
    <data>
        <meta
            githash="0x56D91F"
            gitdate="1478267676"
            firmware_version="1.0"
            firmware_name="eval"
            firmware_entrypoint="0x8006000"
            firmware_size="231884"
            creation_date="2016.05.07 15:11"
            crypto="aes"/>
        <aes128_iv> 64 based value</aes128_iv>
        <sha256_for_verify> 64 based value </sha256_for_verify>
        <binary></binary>
    </data>
    <check> 64 based value</check>
</crystalBoot>
*/

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



    QDomNode topNode = doc.elementsByTagName("crystalBoot").at(0);
    if (topNode.isNull()){
       qDebug() << "crystalBoot does not exist";
        return false;
    }

    QDomNode crystalBootNode = topNode.firstChildElement("data");
    if (crystalBootNode.isNull()){
       qDebug() << "crystalBoot/data does not exist";
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

    QString imageCreationDate_str = metaNodeElement.attribute("creation_date","");
    imageCreationDate = QDateTime::fromString(imageCreationDate_str,"yyyy.MM.dd HH:mm");
    if (!imageCreationDate.isValid()) {
        qDebug() << "imageCreationDate is not a datetime";
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

    QDomElement checksumNodeElement = crystalBootNode.firstChildElement("sha256_for_verify");
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


    QDomElement checkNode = topNode.firstChildElement("check");
    if (checkNode.isNull()){
       qDebug() << "crystalBoot/check does not exist";
        return false;
    }

    QString check_read = checkNode.text();

    QBuffer bufferForCheckSum;
    bufferForCheckSum.open(QIODevice::WriteOnly);
    createXML_for_checksum(bufferForCheckSum);
    bufferForCheckSum.close();

#if 0
    {
        bufferForCheckSum.open(QIODevice::ReadOnly);
        QFile testData("test_dec.xml");
        testData.open(QIODevice::WriteOnly);
        testData.write(bufferForCheckSum.readAll());
        testData.close();
        bufferForCheckSum.close();
    }
#endif
    QString hashValue_64_calced = getBase64CheckSum(bufferForCheckSum);
    firmwareLoadConsistency = hashValue_64_calced == check_read;
    if (firmwareLoadConsistency){
        qDebug() << "data consistent";
    }else{
        qDebug() << "data not consistent";
    }


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


