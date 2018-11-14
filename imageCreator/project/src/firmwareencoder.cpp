#include "firmwareencoder.h"
#include "hexfile.h"
#include <QDebug>
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <QRegularExpression>
#include <QFile>
#include <QCryptographicHash>
#include <QBuffer>
#include "aes.h"
#include "aeskeyfile.h"

QByteArray AES_CBC_128_encrypt(QByteArray &key, QByteArray &iv, QByteArray &plain) {

    uint8_t aes_key_buffer[16];
    uint8_t *p_aes_key = aes_key_buffer;
    QBuffer aes128_keyStream(&key);
    aes128_keyStream.open(QIODevice::ReadOnly);
    aes128_keyStream.read((char *)aes_key_buffer, sizeof aes_key_buffer);

    uint8_t aes_iv_buffer[16];
    uint8_t *p_aes_iv = aes_iv_buffer;
    QBuffer aes128_ivStream(&iv);
    aes128_ivStream.open(QIODevice::ReadOnly);
    aes128_ivStream.read((char *)aes_iv_buffer, sizeof aes_iv_buffer);

    QBuffer inStream(&plain);
    QByteArray cipheredArray;

    uint8_t inbuffer[32];
    uint8_t outbuffer[32];
    inStream.open(QIODevice::ReadOnly);
    while (!inStream.atEnd()) {
        inStream.read((char *)inbuffer, sizeof inbuffer);
        AES128_CBC_encrypt_buffer(outbuffer, inbuffer, sizeof inbuffer, p_aes_key, p_aes_iv);
        cipheredArray.append((char *)outbuffer, sizeof outbuffer);
        p_aes_key = 0;
        p_aes_iv = 0;
    }
    return cipheredArray;
}

QByteArray AES_CBC_128_decrypt(QByteArray key, QByteArray iv, QByteArray &cipher) {
    uint8_t aes_key_buffer[16];
    uint8_t *p_aes_key = aes_key_buffer;

    QBuffer aes128_keyStream(&key);
    aes128_keyStream.open(QIODevice::ReadOnly);
    aes128_keyStream.read((char *)aes_key_buffer, sizeof aes_key_buffer);

    uint8_t aes_iv_buffer[16];
    uint8_t *p_aes_iv = aes_iv_buffer;
    QBuffer aes128_ivStream(&iv);
    aes128_ivStream.open(QIODevice::ReadOnly);
    aes128_ivStream.read((char *)aes_iv_buffer, sizeof aes_iv_buffer);

    QBuffer inStream(&cipher);
    QByteArray plainArray;

    uint8_t inbuffer[32];
    uint8_t outbuffer[32];
    inStream.open(QIODevice::ReadOnly);
    while (!inStream.atEnd()) {
        inStream.read((char *)inbuffer, sizeof inbuffer);
        AES128_CBC_decrypt_buffer(outbuffer, inbuffer, sizeof inbuffer, p_aes_key, p_aes_iv);
        plainArray.append((char *)outbuffer, sizeof outbuffer);
        p_aes_key = 0;
        p_aes_iv = 0;
    }
    return plainArray;
}

FirmwareImageContainer::FirmwareImageContainer(ImageCreatorSettings &imageCreatorSettings) : imageCreatorSettings(imageCreatorSettings) {
    fwImage.clear();
    srand(time(NULL));
}

bool FirmwareImageContainer::load_plain_image() {
    fwImage.load_plain_image(imageCreatorSettings);

    if (imageCreatorSettings.crypto == ImageCreatorSettings::Crypto::AES128) {
        AESKeyFile aeskeyfile;
        if (!aeskeyfile.open(imageCreatorSettings.encryption_key_file_name_absolute)) {
            qDebug() << "error loading key file:" << imageCreatorSettings.encryption_key_file_name_absolute;
            return false;
        }

        if (!aeskeyfile.isValid()) {
            qDebug() << "aes key file not valid";
            return false;
        }

        fwImage.set_crypto(FirmwareMetaData::Crypto::AES128);
        fwImage.aes128_iv.clear();
        for (size_t i = 0; i < 16; i++) {
            uint8_t tmp = rand() & 0xFF;
            fwImage.aes128_iv.append(tmp);
        }

        fwImage.binary_encoded = AES_CBC_128_encrypt(aeskeyfile.key, fwImage.aes128_iv, fwImage.binary_plain);
    }
    const FirmwareMetaData &firmware_meta_data = fwImage.get_firmware_meta_data();
    qDebug() << "entrypoint: 0x" + QString::number(firmware_meta_data.firmware_entryPoint, 16);
    qDebug() << "size: " + QString::number(firmware_meta_data.firmware_size);

    qDebug() << "sha256: " << fwImage.sha256.toHex();
    return true;
}

bool FirmwareImageContainer::saveImage() {
    bool result = fwImage.save_compiled_image(imageCreatorSettings.target_file_name_application_image_absolute);
    const auto &firmware_meta_data = fwImage.get_firmware_meta_data();

    HexFile hexfile;
    hexfile.append_hex_file(imageCreatorSettings.hex_file_name_bootloader_absolute);
    QByteArray descriptor_binary = firmware_meta_data.compile_firmware_descriptor_binary(fwImage.sha256);
    hexfile.overwrite_binary(descriptor_binary, firmware_meta_data.address_firmware_descriptor_in_bootloader);
    hexfile.append_hex_file(imageCreatorSettings.hex_file_name_application_absolute);
    QStringList bundled_hex_file = hexfile.get_hex_file();

    QFile file_hex("out.hex");
    if (file_hex.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file_hex);
        for (const auto &s : bundled_hex_file) {
            out << s << "\n";
        }
        file_hex.close();
    }
    QFile file_bin("out.bin");
    if (file_bin.open(QIODevice::WriteOnly)) {
        file_bin.write(hexfile.get_binary());
        file_bin.close();
    }

    return result;
}
