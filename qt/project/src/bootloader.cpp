#include "bootloader.h"
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <assert.h>

channel_codec_instance_t channel_codec_instance[channel_codec_comport_COUNT];

static void crc16_online(uint8_t data, uint16_t *crc16) {
    uint8_t x;

    x = *crc16 >> 8 ^ data;
    x ^= x >> 4;
    *crc16 = (*crc16 << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
}

static void crc16_online_init(uint16_t *crc16) {
    *crc16 = 0xFFFF;
}

void RPC_setTimeout(uint32_t timeout_ms);
uint32_t RPC_getTimeout(void);

Bootloader::Bootloader(QString settingsFileName, QObject *parent) : QObject(parent), settings(parent) {
    settings.load(settingsFileName);
    serialThread = new SerialThread(this);
    fileLoaded = false;
}

Bootloader::~Bootloader() {
    delete serialThread;
}

static QString RPC_RESULT_to_string(RPC_RESULT rpc_result) {
    switch (rpc_result) {
        case RPC_SUCCESS:
            return "RPC_SUCCESS";
        case RPC_FAILURE:
            return "RPC_FAILURE";

        case RPC_COMMAND_UNKNOWN:
            return "RPC_COMMAND_UNKNOWN";

        case RPC_COMMAND_INCOMPLETE:
            return "RPC_COMMAND_INCOMPLETE";
    }
    return "";
}

void Bootloader::tryConnect() {
    if (serialThread->isOpen()) {

        log("try to connect..");
        uint32_t timeout = RPC_getTimeout();
        RPC_setTimeout(50);
        if (serialThread->rpcEnterProgrammingMode() == RPC_SUCCESS) {
            bool corr_hash = serialThread->rpcIsCorrectHash();
            if (corr_hash) {
                log("bootloader found.");
                setConnState(ConnectionState::Connected);
                getDeviceInfo();
            } else {
                uint8_t hash[16];
                uint16_t version;
                serialThread->rpcGetRemoteHashVersion(hash, &version);
                log("");
                log("bootloader found but incorrect RPC hash(" + RemoteDeviceInfo::arrayToHexString(hash, 16, 0) + ", V. " +
                    QString::number(version) + "), disconnecting..");
                log("");
                serialThread->close();
                setConnState(ConnectionState::Disconnected);
            }
        }
        RPC_setTimeout(timeout);
    }
}

ConnectionState Bootloader::getConnectionState() {
    return connState;
}

void Bootloader::connectComPort(QString serialPortName) {
    if (serialPortName != "") {
        settings.COMPortName = serialPortName;
        connectComPort();
    }
}

void Bootloader::connectComPort() {
    int baudrate = 115200;
    qDebug() << "open" << settings.COMPortName << baudrate;
    serialThread->open(settings.COMPortName, baudrate);

    if (serialThread->isOpen()) {
        setConnState(ConnectionState::Connecting);

    } else {
        log("still closed");
    }
}

void Bootloader::disconnectComPort() {
    serialThread->close();
    if (serialThread->isOpen()) {
        log("still opened");

    } else {
        setConnState(ConnectionState::Disconnected);
    }
}

RPC_RESULT Bootloader::eraseFlash() {
    RPC_RESULT result = RPC_SUCCESS;
    QTime runtime;
    runtime.start();
    log("erasing..");
    RPC_setTimeout(20 * 1000);
    result = serialThread->rpcEraseFlash();
    // flashResultDocumentation.addActionResult("Erase", result);
    if (result != RPC_SUCCESS) {
        //   fail = true;
        log("erasing fail");
    } else {
        log("erase ok. " + QString::number(runtime.elapsed() / 1000.0) + " seconds needed");
    }
    return result;
}

void Bootloader::sendfirmware() {
#define BLOCKLENGTH 128
    FlashResultDocumentation flashResultDocumentation;
    RPC_RESULT result = RPC_SUCCESS;
    if (fwImage.isValid()) {
        if (fwImage.isFileModified()) {
            QMessageBox msgBox;
            msgBox.setText("Firmware image has been modified.");
            msgBox.setInformativeText("Do you want to reload the file?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Yes) {
                loadFile(fwImage.fileName);
            }
        }
    }
    flashResultDocumentation.setNewFirmwareDescriptor(fwImage);
    if (fwImage.isValid()) {
        result = getDeviceInfo();
        flashResultDocumentation.addActionResult("GetInfo", result);
        flashResultDocumentation.setRemoteDeviceInfo(remoteDeviceInfo);
        flashResultDocumentation.checkPlausibility();

        emit onProgress(0);

        qint64 fileSize = fwImage.binary_encoded.size();
        qint64 byteCounter = 0;
        QTime runtime;
        QTime totalRuntime;
        bool fail = false;

        if (!flashResultDocumentation.showWarngingMessage()) {
            fail = true;
            log("cancelled due to plausibility issues.");
            flashResultDocumentation.addActionResult("Plausibility", RPC_FAILURE);
            result = RPC_FAILURE;
        } else {
            flashResultDocumentation.addActionResult("Plausibility", RPC_SUCCESS);
            result = RPC_SUCCESS;
        }

        runtime.start();
        totalRuntime.start();

#if 1
        if (!fail) {
            //   log("erasing..");
            //  RPC_setTimeout(20 * 1000);
            result = eraseFlash();
            flashResultDocumentation.addActionResult("Erase", result);
            if (result != RPC_SUCCESS) {
                fail = true;
            }
            //      log("erasing fail");
            //  } else {
            //     log("erase ok. " + QString::number(runtime.elapsed() / 1000.0) + " seconds needed");
            // }
        }

#endif

        if (!fail) {
            log("Initializing transfer..");
            crystalBoolResult_t return_value;
            result = serialThread->rpcInitFirmwareTransfer(&return_value, fwImage);

            flashResultDocumentation.addActionResult("Initialization", result);
            if (result != RPC_SUCCESS) {
                fail = true;
                if (return_value == crystalBool_TryAgainLater) {
                    log("Transfer init needs a timeout break. Please try again later.");
                } else {
                    log("Transfer init fail.");
                }
            } else {
                log("Transfer init ok. ");
            }

            runtime.start();
        }
        RPC_setTimeout(1 * 1000);
#if 1
        {
#if 1
            QBuffer firmwareFile(&fwImage.binary_encoded);
            firmwareFile.open(QIODevice::ReadOnly);
            while (!firmwareFile.atEnd() && fail == false) {

                char blockData[BLOCKLENGTH];
                char blockDataTest[BLOCKLENGTH];
                memset(blockData, 0, BLOCKLENGTH);
                byteCounter += firmwareFile.read(blockData, BLOCKLENGTH);
                result = serialThread->rpcWriteFirmwareBlock((uint8_t *)blockData, BLOCKLENGTH);
                if (result != RPC_SUCCESS) {
                    fail = true;
                    break;
                }

                memset(blockDataTest, 0, BLOCKLENGTH);
#if 0
                result = serialThread->rpcReadFirmwareBlock((uint8_t*)blockDataTest,BLOCKLENGTH);
                if (result != RPC_SUCCESS){
                    break;
                }

                for(uint8_t i=0;i<BLOCKLENGTH;i++){
                    if (blockData[i] != blockDataTest[i]){
                        qDebug() << "verify fail at " << i;
                        fail = true;
                        break;
                    }
                }
                if (fail){
                    break;
                }
#endif
                static qint64 progress_old = 100;
                qint64 progress = 100 * byteCounter;
                progress /= fileSize;
                if (progress_old != progress) {
                    log("progress: " + QString::number(progress) + "%");
                    emit onProgress(progress);
                }
                progress_old = progress;
            }
#endif
        }
#endif
        flashResultDocumentation.addActionResult("Transfer", result);
        if ((result != RPC_SUCCESS) || fail) {
            QString resultstr = RPC_RESULT_to_string(result);
            log(resultstr);
        } else {
            log("tranfer ok. " + QString::number(runtime.elapsed() / 1000.0) + " seconds needed. In Total: " +
                QString::number(totalRuntime.elapsed() / 1000.0) + "seconds needed.");
        }

        if (!fail) {
            result = serialThread->rpcVerifyChecksum();
            flashResultDocumentation.addActionResult("Verify", result);
            if (result != RPC_SUCCESS) {
                fail = true;
                log("verify fail");
            } else {
                log("verify ok. ");
            }
        }

        (void)fileSize;
        (void)byteCounter;

    } else {
        log("firmware file not valid ");
    }
    flashResultDocumentation.save(settings);
    emit onFinished();
}

void Bootloader::readEEPROMToFile(QString file_name) {
    QFile file_bin(file_name);
    if (file_bin.open(QIODevice::WriteOnly)) {
        emit onProgress(0);
#if 0
        RPC_RESULT rpcEEPROMWriteBlock(uint8_t *data, uint8_t size);
        RPC_RESULT rpcEEPROMReadBlock(uint8_t *data, uint8_t size);
        RPC_RESULT rpcEEPROMGetSize(uint16_t *size);
        RPC_RESULT rpcEEPROMInitTransfer();
        RPC_RESULT rpcEEPROMVerify(uint16_t crc16);
#endif
        log("Reading eeprom to file..");
        uint16_t eeprom_size = 0;
        RPC_RESULT result;
        result = serialThread->rpcEEPROMGetSize(&eeprom_size);
        if (result != RPC_SUCCESS) {
            QString resultstr = RPC_RESULT_to_string(result);
            // qDebug() << "readEEPROMToFile, rpcEEPROMGetSize " << resultstr;
            log("error: readEEPROMToFile, rpcEEPROMGetSize: " + resultstr);
            emit onFinished();
            return;
        }
        result = serialThread->rpcEEPROMInitTransfer();
        if (result != RPC_SUCCESS) {
            QString resultstr = RPC_RESULT_to_string(result);
            log("error: readEEPROMToFile, rpcEEPROMInitTransfer: " + resultstr);
            emit onFinished();
            return;
        }
        uint32_t read_position = 0;
        while (read_position < eeprom_size) {
            uint8_t block_buffer[128] = {};
            uint32_t block_size_32 = 128;
            if (read_position + block_size_32 > eeprom_size) {
                block_size_32 = eeprom_size - read_position;
            }
            assert(block_size_32 < 0x100); // RPC function has 8 bits for size
            uint8_t block_size_8 = block_size_32;
            result = serialThread->rpcEEPROMReadBlock(block_buffer, block_size_8);
            if (result != RPC_SUCCESS) {
                QString resultstr = RPC_RESULT_to_string(result);
                log("error: readEEPROMToFile, rpcEEPROMReadBlock: " + resultstr);
                emit onFinished();
                return;
            }
            file_bin.write((char *)block_buffer, block_size_32);
            read_position += block_size_32;
            {
                static qint64 progress_old = 100;
                qint64 progress = 100 * read_position;
                progress /= eeprom_size;
                if (progress_old != progress) {
                    log("progress: " + QString::number(progress) + "%");
                    emit onProgress(progress);
                }
                progress_old = progress;
            }
        }
        log("Reading eeprom done.");
        file_bin.close();
    }
    emit onFinished();
}

void Bootloader::writeEEPROMFromFile(QString file_name) {

    QFile file_bin(file_name);
    if (file_bin.open(QIODevice::ReadOnly)) {
        emit onProgress(0);
        log("Writing eeprom from file..");
        uint16_t eeprom_size = 0;
        RPC_RESULT result;
        result = serialThread->rpcEEPROMGetSize(&eeprom_size);
        if (result != RPC_SUCCESS) {
            QString resultstr = RPC_RESULT_to_string(result);
            log("error: writeEEPROMFromFile, rpcEEPROMGetSize: " + resultstr);
            emit onFinished();
            return;
        }
        result = serialThread->rpcEEPROMInitTransfer();
        if (result != RPC_SUCCESS) {
            QString resultstr = RPC_RESULT_to_string(result);
            log("error: writeEEPROMFromFile, rpcEEPROMInitTransfer: " + resultstr);
            emit onFinished();
            return;
        }
        uint32_t write_position = 0;
        while (write_position < eeprom_size) {
            uint8_t block_buffer[128];
            uint32_t block_size_32 = 128;
            if (write_position + block_size_32 > eeprom_size) {
                block_size_32 = eeprom_size - write_position;
            }
            block_size_32 = file_bin.read((char *)block_buffer, block_size_32);
            if (block_size_32 == 0) {
                break;
            }

            assert(block_size_32 < 0x100); // RPC function has 8 bits for size
            uint8_t block_size_8 = block_size_32;
            result = serialThread->rpcEEPROMWriteBlock(block_buffer, block_size_8);
            if (result != RPC_SUCCESS) {
                QString resultstr = RPC_RESULT_to_string(result);
                log("error: writeEEPROMFromFile, rpcEEPROMWriteBlock: " + resultstr);
                emit onFinished();
                return;
            }
            write_position += block_size_32;
            {
                static qint64 progress_old = 100;
                qint64 progress = 100 * write_position;
                progress /= eeprom_size;
                if (progress_old != progress) {
                    log("progress: " + QString::number(progress) + "%");
                    emit onProgress(progress);
                }
                progress_old = progress;
            }
        }
        log("Writing eeprom done.");
        file_bin.close();
    }
    emit onFinished();
}

bool Bootloader::verifyEEPROMFromFile(QString file_name) {
    QFile file_bin(file_name);
    if (file_bin.open(QIODevice::ReadOnly)) {
        log("Verifing eeprom..");
        uint16_t eeprom_size = 0;
        uint16_t crc16 = 0;
        crc16_online_init(&crc16);
        RPC_RESULT result;
        result = serialThread->rpcEEPROMGetSize(&eeprom_size);

        if (result != RPC_SUCCESS) {
            QString resultstr = RPC_RESULT_to_string(result);
            log("error: verifyEEPROMFromFile, rpcEEPROMGetSize: " + resultstr);
            log("Verify EEPROM failed.");
            return false;
        }

        if (file_bin.size() != eeprom_size) {
            log("verifyEEPROMFromFile, siz emissmatch, file:" + QString::number(file_bin.size()) + ", eeprom:" + QString::number(eeprom_size));
            log("Verify EEPROM failed.");
            return false;
        }
        uint32_t write_position = 0;
        while (write_position < eeprom_size) {
            uint8_t block_buffer[128];
            uint32_t block_size_32 = 128;
            if (write_position + block_size_32 > eeprom_size) {
                block_size_32 = eeprom_size - write_position;
            }
            block_size_32 = file_bin.read((char *)block_buffer, block_size_32);
            if (block_size_32 == 0) {
                break;
            }
            for (uint32_t i = 0; i < block_size_32; i++) {
                crc16_online(block_buffer[i], &crc16);
            }
        }

        result = serialThread->rpcEEPROMVerify(crc16);
        if (result != RPC_SUCCESS) {
            QString resultstr = RPC_RESULT_to_string(result);
            log("error: verifyEEPROMFromFile, rpcEEPROMVerify: " + resultstr);
            log("Verify EEPROM failed.");
            return false;
        }
        file_bin.close();
    }
    log("Verify EEPROM ok.");
    return true;
}

void Bootloader::eraseEEPROM() {
    RPC_RESULT result = RPC_SUCCESS;
    FlashResultDocumentation flashResultDocumentation;
    result = getDeviceInfo();
    flashResultDocumentation.addActionResult("GetInfo", result);
    flashResultDocumentation.setRemoteDeviceInfo(remoteDeviceInfo);

    QTime runtime;

    runtime.start();
#if 1
    log("erasing eeprom..");
    RPC_setTimeout(20 * 1000);
    result = serialThread->rpcEEPROMErase();
    flashResultDocumentation.addActionResult("Erase EEPROM", result);
    if (result != RPC_SUCCESS) {
        log("erasing eeprom fail");
    } else {
        log("erase eeprom ok. " + QString::number(runtime.elapsed() / 1000.0) + " seconds needed");
    }
    flashResultDocumentation.save(settings);
#endif
}

RPC_RESULT Bootloader::getDeviceInfo() {
    mcu_descriptor_t descriptor;
    device_descriptor_v1_t deviceDescriptor;
    firmware_descriptor_t firmwareDescriptor;
    bool rpcOK = true;

    remoteDeviceInfo.unSet();
    RPC_RESULT result = serialThread->rpcGetMCUDescriptor(&descriptor);
    if (!result == RPC_SUCCESS) {
        rpcOK = false;
    }
    result = serialThread->rpcGetDeviceDescriptor(&deviceDescriptor);
    if (!result == RPC_SUCCESS) {
        rpcOK = false;
    }
    result = serialThread->rpcGetFirmwareDescriptor(&firmwareDescriptor);
    if (!result == RPC_SUCCESS) {
        rpcOK = false;
    }

    if (rpcOK) {
        log("mcu info request ok");
        remoteDeviceInfo.setDeviceDescriptor(&deviceDescriptor);
        remoteDeviceInfo.setMCUDescriptor(&descriptor);
        remoteDeviceInfo.setOldFirmwareDescriptor(&firmwareDescriptor);
        plausibilityCheck.checkPlausibiltity(remoteDeviceInfo, fwImage);
    } else {
        log("mcu info request error (" + QString::number(result) + ")");
    }

    emit onMCUGotDeviceInfo();
    return result;
}

void Bootloader::runApplication() {
    RPC_RESULT result = serialThread->rpcRunApplication();
    log("starting application..");
    if (result == RPC_SUCCESS) {
        qDebug() << "application startet";
    } else {
        qDebug() << "application start failed" << result;
    }
    disconnectComPort();
}

void Bootloader::log(QString str) {
    qDebug() << str;
    emit onLog(str);
}

void Bootloader::setConnState(ConnectionState connState) {
    if (connState == ConnectionState::Disconnected) {
        remoteDeviceInfo.unSet();
        emit onMCUGotDeviceInfo();
    }
    this->connState = connState;
    emit onConnStateChanged(connState);
}

void Bootloader::loadFile(QString fileName) {
    if (fwImage.load_compiled_image(fileName)) {
        fileLoaded = true;

    } else {
        fileLoaded = false;
        fwImage.clear();
        log("couldnt load file " + fileName);
    }
    plausibilityCheck.checkPlausibiltity(remoteDeviceInfo, fwImage);
    fileNameToSend = fileName;
}

void Bootloader::saveSettings() {
    settings.save();
}

QString Bootloader::getComPortName() {
    return settings.COMPortName;
}
