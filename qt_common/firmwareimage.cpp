#include "intelhexclass.h"
#include "firmwareimage.h"
#include <QXmlStreamWriter>
#include <QFile>
#include <QDomDocument>
#include <QDomNode>
#include <QDebug>
#include <QFileInfo>
#include <QCryptographicHash>
#include <fstream>

FirmwareImage::FirmwareImage() {
    clear();
}

void FirmwareImage::clear() {
    firmware_meta_data.clear();
    binary_encoded.clear();
    sha256.clear();
    firmwareLoadConsistency = false;
}

bool FirmwareImage::isValid() {
    const uint32_t CHECKSUM_SIZE = 32;
    // signature check

    if (!firmware_meta_data.is_valid()) {
        return false;
    }

    if (!binary_encoded.size()) {
        qDebug() << "binary.size() is zero";
        return false;
    }

    if ((firmware_meta_data.firmware_size != (size_t)binary_encoded.size()) && (firmware_meta_data.crypto == FirmwareMetaData::Crypto::Plain)) {
        qDebug() << "firmware_size doesnt equal binary.size()";
        return false;
    }

    if (sha256.size() != CHECKSUM_SIZE) {
        qDebug() << "sha256 is not 32";
        return false;
    }

    return true;
}

bool FirmwareImage::isFileModified() {
    return lastModified != QFileInfo(fileName).lastModified();
}

const FirmwareMetaData &FirmwareImage::get_firmware_meta_data() {
    return firmware_meta_data;
}

void FirmwareImage::set_crypto(FirmwareMetaData::Crypto crypto) {
    firmware_meta_data.crypto = crypto;
}

void FirmwareImage::createXML_for_checksum(QBuffer &buf) {
    QXmlStreamWriter xml;
    xml.setDevice(&buf);
    firmware_meta_data.write_meta_data_to_xml(xml);
    QByteArray aes128_iv_64 = aes128_iv.toBase64();
    xml.writeTextElement("aes128_iv", aes128_iv_64.data());

    QByteArray checkSumBinary64 = sha256.toBase64();
    xml.writeTextElement("sha256_for_verify", checkSumBinary64.data());

    QByteArray imageBinary64 = binary_encoded.toBase64();
    xml.writeTextElement("binary", imageBinary64.data());

    xml.writeEndDocument();
}

QString FirmwareImage::getBase64CheckSum(QBuffer &bufferForCheckSum) {
    bufferForCheckSum.open(QIODevice::ReadOnly);
    QCryptographicHash hashfunction(QCryptographicHash::Sha256);
    hashfunction.addData(&bufferForCheckSum);
    QByteArray hashValue = hashfunction.result();
    QString hashValue_64 = hashValue.toBase64();
    bufferForCheckSum.close();
    return hashValue_64;
}

bool FirmwareImage::save_compiled_image(QString targetFile) {

    QFile file(targetFile);

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "cant open file" << targetFile;
        return false;
    }
    fileName = targetFile;

    firmware_meta_data.imageCreationDate = QDateTime::currentDateTime();
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
        stream << "<data>" << xmlData << "</data>";
        stream << "<check>" << hashValue_64 << "</check>";
        stream << "</crystalBoot>";
    }

    file.close();
    lastModified = QFileInfo(fileName).lastModified();
    return true;
}

bool FirmwareImage::load_compiled_image(QString fileName) {
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
    if (!file.open(QIODevice::ReadOnly)) {
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
    if (topNode.isNull()) {
        qDebug() << "crystalBoot does not exist";
        return false;
    }

    QDomNode crystalBootNode = topNode.firstChildElement("data");
    if (crystalBootNode.isNull()) {
        qDebug() << "crystalBoot/data does not exist";
        return false;
    }

    QDomElement metaNodeElement = crystalBootNode.firstChildElement("meta");

    firmware_meta_data.load_meta_data_from_xml(metaNodeElement);

    if (firmware_meta_data.crypto == FirmwareMetaData::Crypto::AES128) {
        QDomElement aes128_ivNodeElement = crystalBootNode.firstChildElement("aes128_iv");
        if (aes128_ivNodeElement.isNull()) {
            qDebug() << "no iv found";
            return false;
        }
        QByteArray aes128_iv_64 = aes128_ivNodeElement.text().toUtf8();
        aes128_iv = QByteArray::fromBase64(aes128_iv_64);
    } else {
        aes128_iv = QByteArray(16, 0);
    }

    QDomElement checksumNodeElement = crystalBootNode.firstChildElement("sha256_for_verify");
    if (checksumNodeElement.isNull()) {
        qDebug() << "no checksum found";
        return false;
    }
    QByteArray checksum_base64 = checksumNodeElement.text().toUtf8();
    sha256 = QByteArray::fromBase64(checksum_base64);

    QDomElement binNodeElement = crystalBootNode.firstChildElement("binary");
    if (binNodeElement.isNull()) {
        qDebug() << "no binary found";
        return false;
    }
    QByteArray bin_base64 = binNodeElement.text().toUtf8();
    binary_encoded = QByteArray::fromBase64(bin_base64);

    QDomElement checkNode = topNode.firstChildElement("check");
    if (checkNode.isNull()) {
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
    if (firmwareLoadConsistency) {
        qDebug() << "data consistent";
    } else {
        qDebug() << "data not consistent";
    }

    return isValid();
}

bool readHexFile(QString fileName, QByteArray &result, uint32_t &startAddress) {
    intelhex ihex;
    std::ifstream intelHexFile;
    intelHexFile.open(fileName.toStdString(), std::ifstream::in);

    if (!intelHexFile.good()) {
        qDebug() << "Error: couldn't open " << fileName;
        return false;
    }
    intelHexFile >> ihex;
    ihex.begin();
    startAddress = ihex.currentAddress();
    ihex.begin();
    uint8_t hexByte = 0;
    result.clear();
    uint32_t oldAddress = ihex.currentAddress();
    while (ihex.getData(&hexByte)) {
        uint32_t newAddress = ihex.currentAddress(); // fill "blank address spaces" with 0
        uint32_t diffAddress = newAddress - oldAddress;
        for (uint32_t diff = 1; diff < diffAddress; diff++) {
            uint8_t val = 0;
            result.append(val);
        }
        result.append(hexByte);
        oldAddress = newAddress;
        ++ihex;
    }
    int numberWarnings = ihex.getNoWarnings();
    if (numberWarnings) {
        qDebug() << "reading hexfile number of warnings:" << numberWarnings;
        return false;
    }
    int numberErrors = ihex.getNoErrors();
    if (numberErrors) {
        qDebug() << "reading hexfile number of errors:" << numberErrors;
        return false;
    }
    return true;
}

bool readBinFile(QString fileName, QByteArray &result) {
    QFile firmwareFile(fileName);

    if (!firmwareFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    result = firmwareFile.readAll();
    return true;
}

bool FirmwareImage::load_plain_image(ImageCreatorSettings &settings) {
    clear();
    if (firmware_meta_data.load_meta_data_from_source(settings) == false) {
        return false;
    }

    if (!readHexFile(settings.hex_file_name_application_absolute, binary_plain, firmware_meta_data.firmware_entryPoint)) {
        qDebug() << "error loading hex file:" << firmware_meta_data.firmware_name;
        return false;
    }
    firmware_meta_data.firmware_size = binary_plain.size();

    QCryptographicHash sha256_check(QCryptographicHash::Sha256);
    QByteArray data_for_hashing(binary_plain);

    sha256_check.addData(data_for_hashing);
    sha256 = sha256_check.result();

    aes128_iv = QByteArray(16, 0);
    binary_encoded = binary_plain;

    qDebug() << "entrypoint: 0x" + QString::number(firmware_meta_data.firmware_entryPoint, 16);
    qDebug() << "size: " + QString::number(firmware_meta_data.firmware_size);

    qDebug() << "sha256: " << sha256.toHex();
    return true;
}
