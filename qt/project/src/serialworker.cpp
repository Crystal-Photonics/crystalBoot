#include <qdebug.h>
#include <thread>
#include <chrono>
#include <QBuffer>
#include <QApplication>
#include "serialworker.h"
#include "mainwindow.h"

#include "errorlogger/generic_eeprom_errorlogger.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_qt2mcu.h"
#include "channel_codec/channel_codec.h"

SerialWorker *serialWorkerForRPCFunc = NULL;

#define CHANNEL_CODEC_TX_BUFFER_SIZE 256
#define CHANNEL_CODEC_RX_BUFFER_SIZE 256

char channel_codec_rxbuffer[channel_codec_comport_COUNT][CHANNEL_CODEC_RX_BUFFER_SIZE];
char channel_codec_txbuffer[channel_codec_comport_COUNT][CHANNEL_CODEC_TX_BUFFER_SIZE];

extern "C" RPC_RESULT phyPushDataBuffer(channel_codec_instance_t *instance, const char *buffer, size_t length) {

    if (instance->aux.portindex == channel_codec_comport_transmission) {
        QByteArray data = QByteArray(buffer, length);
        SerialThread *serialthread = instance->aux.serialthread;
        assert(serialthread);
        if (serialthread) {
            serialthread->sendByteData(data);
        }
    }
    return RPC_SUCCESS;
}

extern "C" void ChannelCodec_errorHandler(channel_codec_instance_t *instance, channelCodecErrorNum_t ErrNum) {
    (void)ErrNum;
    (void)instance;
}

SerialThread::SerialThread(QObject *parent) : QObject(parent) {

    thread = new QThread(this);
    serialWorker = new SerialWorker(this);

    serialWorkerForRPCFunc = serialWorker;
    serialWorker->moveToThread(thread);

    connect(thread, SIGNAL(started()), serialWorker, SLOT(process()));
    connect(serialWorker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(serialWorker, SIGNAL(finished()), serialWorker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    connect(this, SIGNAL(openPort(QString, int)), serialWorker, SLOT(openPort(QString, int)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(closePort()), serialWorker, SLOT(closePort()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(isPortOpened()), serialWorker, SLOT(isPortOpened()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(sendData(QByteArray)), serialWorker, SLOT(sendData(QByteArray)), Qt::QueuedConnection);

    thread->start();
}

SerialThread::~SerialThread() {
    thread->quit();
    while (!thread->wait(16)) {
        QApplication::processEvents();
    }
}

void SerialThread::open(QString name, int baudrate) {

    emit openPort(name, baudrate);
}

void SerialThread::close() {

    emit closePort();
}

bool SerialThread::isOpen() {
    bool result;
    result = emit isPortOpened();
    return result;
}

RPC_RESULT SerialThread::rpcWriteFirmwareBlock(uint8_t *data, size_t size) {
    RPC_RESULT result;
    assert(size == 128);
    crystalBoolResult_t return_value = crystalBool_OK;
    result = mcuWriteFirmwareBlock(&return_value, data);
    if (return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }
    QString resultstr;
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcReadFirmwareBlock(uint8_t *data, size_t size) {
    RPC_RESULT result;
    assert(size == 128);
    crystalBoolResult_t return_value = crystalBool_OK;
    result = mcuReadFirmwareBlock(&return_value, data);
    if (return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }
    QString resultstr;
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcVerifyChecksum() {
    RPC_RESULT result;

    crystalBoolResult_t return_value = crystalBool_OK;

    result = mcuVerifyFirmware(&return_value);
    if (return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }

    QString resultstr;
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcEEPROMInitTransfer() {
    RPC_RESULT result;
    crystalBoolResult_t return_value = crystalBool_OK;
    result = mcuEEPROMInitTransfer(&return_value);
    if (return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }
    QString resultstr;
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }
    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcEEPROMGetSize(uint16_t *size) {
    RPC_RESULT result;
    crystalBoolResult_t return_value = crystalBool_OK;
    result = mcuEEPROMGetSize(&return_value, size);
    if (return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }
    QString resultstr;
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }
    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcEEPROMWriteBlock(uint8_t *data, uint8_t size) {
    RPC_RESULT result;
    assert(size <= 128);
    crystalBoolResult_t return_value = crystalBool_OK;
    result = mcuEEPROMWrite(&return_value, data, size);
    if (return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }
    QString resultstr;
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }
    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcEEPROMReadBlock(uint8_t *data, uint8_t size) {
    RPC_RESULT result;
    assert(size <= 128);
    crystalBoolResult_t return_value = crystalBool_OK;
    result = mcuEEPROMRead(&return_value, data, size);
    if (return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }
    QString resultstr;
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }
    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcEEPROMVerify(uint16_t crc16) {
    RPC_RESULT result;
    crystalBoolResult_t return_value = crystalBool_OK;
    result = mcuEEPROMVerify(&return_value, crc16);
    if (return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }
    QString resultstr;
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }
    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcEEPROMErase() {
    RPC_RESULT result;

    crystalBoolResult_t return_value = crystalBool_OK;

    result = mcuEEPROMErase(&return_value);
    if (return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }

    QString resultstr;
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcEraseFlash() {
    RPC_RESULT result;

    crystalBoolResult_t return_value = crystalBool_OK;

    result = mcuEraseFlash(&return_value);
    if (return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }

    QString resultstr;
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcInitFirmwareTransfer(crystalBoolResult_t *return_value, FirmwareImage &fwImage) {
    QString resultstr;
    RPC_RESULT result;

    *return_value = crystalBool_OK;

    firmware_descriptor_t firmwareDescriptor;

    crystalBoolCrypto_t crypto = crystalBoolCrypto_Plain;

    memset(&firmwareDescriptor, 0, sizeof(firmware_descriptor_t));

    const FirmwareMetaData &fw_meta_data = fwImage.get_firmware_meta_data();
    switch (fw_meta_data.crypto) {
        case FirmwareMetaData::Crypto::Plain:
            crypto = crystalBoolCrypto_Plain;
            break;
        case FirmwareMetaData::Crypto::AES128:
            crypto = crystalBoolCrypto_AES;
            break;
    }
    firmwareDescriptor.entryPoint = fw_meta_data.firmware_entryPoint;
    firmwareDescriptor.gitDate_unix = fw_meta_data.firmware_gitdate_unix;
    firmwareDescriptor.githash = fw_meta_data.firmware_githash;
    QString nameShort = fw_meta_data.getNameShort(sizeof(firmwareDescriptor.name));
    int len = sizeof(firmwareDescriptor.name);
    if (nameShort.length() < len) {
        len = nameShort.length();
    }

    memcpy(firmwareDescriptor.name, nameShort.toStdString().c_str(), len);

    len = sizeof(firmwareDescriptor.version);

    if (fw_meta_data.firmware_version.length() < len) {
        len = fw_meta_data.firmware_version.length();
    }
    memcpy(firmwareDescriptor.version, fw_meta_data.firmware_version.toStdString().c_str(), len);
    firmwareDescriptor.nameCRC16 = fw_meta_data.getNameCRC16();
    firmwareDescriptor.size = fw_meta_data.firmware_size;

    uint8_t aes128_iv[16];
    memset(aes128_iv, 0, sizeof aes128_iv);
    if (crypto == crystalBoolCrypto_AES) {
        if (fwImage.aes128_iv.size() != sizeof aes128_iv) {
            qDebug() << "aes128_iv wrong size";
            return RPC_FAILURE;
        }

        QBuffer aes128_ivStream(&fwImage.aes128_iv);
        aes128_ivStream.open(QIODevice::ReadOnly);
        if (aes128_ivStream.read((char *)aes128_iv, sizeof aes128_iv) != sizeof aes128_iv) {
            qDebug() << "aes128_iv copy wrong size";
            return RPC_FAILURE;
        }
    }

    uint8_t sha256[32];
    if (fwImage.sha256.size() != sizeof sha256) {
        qDebug() << "checksum wrong size";
        return RPC_FAILURE;
    }
    QBuffer shaStream(&fwImage.sha256);
    shaStream.open(QIODevice::ReadOnly);

    if (shaStream.read((char *)sha256, sizeof sha256) != sizeof sha256) {
        qDebug() << "checksum copy wrong size";
        return RPC_FAILURE;
    }
    result = mcuInitFirmwareTransfer(return_value, &firmwareDescriptor, sha256, aes128_iv, crypto);

    if (*return_value == crystalBool_Fail) {
        result = RPC_FAILURE;
    }

    if (*return_value == crystalBool_TryAgainLater) {
        result = RPC_FAILURE;
    }

    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcRunApplication() {
    QString resultstr;
    RPC_RESULT result = mcuRunApplication();
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcEnterProgrammingMode() {
    QString resultstr;
    RPC_RESULT result = mcuEnterProgrammerMode();
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcGetMCUDescriptor(mcu_descriptor_t *descriptor) {

    QString resultstr;
    RPC_RESULT result = mcuGetMCUDescriptor(descriptor);
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcGetFirmwareDescriptor(firmware_descriptor_t *descriptor) {
    QString resultstr;
    RPC_RESULT result = mcuGetFirmwareDescriptor(descriptor);
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcGetDeviceDescriptor(device_descriptor_v1_t *descriptor) {
    QString resultstr;
    RPC_RESULT result = mcuGetDeviceDescriptor(descriptor);
    switch (result) {
        case RPC_SUCCESS:
            resultstr = "RPC_SUCCESS";
            break;
        case RPC_FAILURE:
            resultstr = "RPC_FAILURE";
            break;
        case RPC_COMMAND_UNKNOWN:
            resultstr = "RPC_COMMAND_UNKNOWN";
            break;
        case RPC_COMMAND_INCOMPLETE:
            resultstr = "RPC_COMMAND_INCOMPLETE";
            break;
    }

    //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

void SerialThread::sendByteData(QByteArray data) {
    emit sendData(data);
}

bool SerialThread::rpcIsCorrectHash(void) {
    unsigned char hash[16];
    unsigned char start_command_id;
    uint16_t version;
    auto result = RPC_TRANSMISSION_get_hash(hash, &start_command_id, &version);
    if (result != RPC_SUCCESS) {
        return false;
    }
    return memcmp(hash, RPC_TRANSMISSION_HASH, RPC_TRANSMISSION_HASH_SIZE) == 0;
}

bool SerialThread::rpcGetRemoteHashVersion(unsigned char hash[16], uint16_t *version) {
    unsigned char start_command_id;
    auto result = RPC_TRANSMISSION_get_hash(hash, &start_command_id, version);
    if (result != RPC_SUCCESS) {
        return false;
    }
    return true;
}

SerialWorker::SerialWorker(SerialThread *serialThread, QObject *parent) : QObject(parent) {
    serialport = new QSerialPort(this);
    connect(serialport, SIGNAL(readyRead()), this, SLOT(on_readyRead()));

    channel_codec_instance[channel_codec_comport_transmission].aux.portindex = channel_codec_comport_transmission;
    channel_codec_instance[channel_codec_comport_transmission].aux.serialthread = serialThread;
    channel_init_instance(&channel_codec_instance[channel_codec_comport_transmission], channel_codec_rxbuffer[channel_codec_comport_transmission],
                          CHANNEL_CODEC_RX_BUFFER_SIZE, channel_codec_txbuffer[channel_codec_comport_transmission], CHANNEL_CODEC_TX_BUFFER_SIZE);
}

SerialWorker::~SerialWorker() {
    emit finished();
}

void SerialWorker::openPort(QString name, int baudrate) {
    serialport->setPortName(name);
    serialport->setBaudRate(baudrate);
    serialport->open(QIODevice::ReadWrite);
}

void SerialWorker::closePort() {
    serialport->close();
}

bool SerialWorker::isPortOpened() {
    bool result = serialport->isOpen();
    return result;
}

void SerialWorker::sendData(QByteArray data) {
    serialport->write(data);
    // qDebug() << ">>>" << data;
}

void SerialWorker::process() {
    // qDebug()<<"process "<<QThread::currentThreadId();
}

void SerialWorker::on_readyRead() {

    // qDebug() << "on read ID:" << thread()->currentThreadId();
    // qDebug() << "on read:" << QThread::currentThreadId();
    QByteArray inbuffer = serialport->readAll();

    if (!inbuffer.isEmpty()) {
        // qDebug() << "<<<" << inbuffer;
    }
    if (inbuffer.count() == 512) {
        qDebug() << "Rechner langsam";
    }
    for (int i = 0; i < inbuffer.count(); i++) {
        channel_push_byte_to_RPC(&channel_codec_instance[channel_codec_comport_transmission], inbuffer[i]);
        if (inbuffer[i] == '\n') {
            // qDebug() << linebuffer;

            linebuffer = "";
        } else {
            linebuffer += inbuffer[i];
        }
    }
}
