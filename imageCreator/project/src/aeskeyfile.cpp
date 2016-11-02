#include "aeskeyfile.h"
#include <QSettings>
#include <QFileInfo>
#include <QBuffer>

bool AESKeyFile::isHex(QString hex_in){
    bool result = true;
    hex_in = hex_in.toUpper();
    for (int i=0;i<hex_in.length();i++){
        if (('0'  <= hex_in[i] && hex_in[i] <= '9') || ('A'  <= hex_in[i] && hex_in[i] <= 'F')){
        }else{
            result = false;
            break;
        }
    }
    return result;
}


bool AESKeyFile::save(QString fileName)
{
    QSettings keyfile(fileName, QSettings::IniFormat);


    QString key_str = key.toHex();
    keyfile.setValue("key",key_str);
    keyfile.setValue("creationDate",creationDateTime.toString("dd.MM.yyyy hh:mm"));

    QString c_key_str = "uint8_t aes_128_key[16] = {";
    for (int i=0;i<key.size();i++){
        c_key_str += "0x"+QString::number((uint8_t)key[i],16);
        if (i+1 < key.size()){
            c_key_str += ",";
        }
    }
    c_key_str += "};";
    keyfile.setValue("c_key",c_key_str);

    this->fileName = fileName;
    return true;
}

bool AESKeyFile::open(QString fileName)
{
    if (!QFileInfo(fileName).exists()){
        return false;
    }

    QSettings keyfile(fileName, QSettings::IniFormat);

    QString key_str = keyfile.value("key","").toString();
    if (isHex(key_str)){
        key = QByteArray::fromHex(key_str.toUtf8());
    }else{
        return false;
    }
    if (key.length() != 16){
        return false;
    }
    QString creation_str = keyfile.value("creationDate","").toString();
    if (creation_str == ""){
        return false;
    }
    creationDateTime = QDateTime::fromString(creation_str, "dd.MM.yyyy hh:mm");
    this->fileName = fileName;
    return true;
}

bool AESKeyFile::isValid()
{
    if (key.length() != 16){
        return false;
    }
    if (!creationDateTime.isValid()){
        return false;
    }
    return true;
}

void AESKeyFile::fillBuffer(uint8_t outbuffer[16])
{
    QBuffer aes128_keyStream(&key);
    aes128_keyStream.open(QIODevice::ReadOnly);

    aes128_keyStream.read((char*)outbuffer,sizeof outbuffer );

}
