#include "bootloader.h"
#include <QMessageBox>
#include <QDebug>

channel_codec_instance_t channel_codec_instance[channel_codec_comport_COUNT];

void RPC_setTimeout(uint32_t timeout_ms);
uint32_t RPC_getTimeout(void);

Bootloader::Bootloader(QString settingsFileName, QObject *parent) : QObject(parent),  settings(parent)
{
    settings.load(settingsFileName);
    serialThread = new SerialThread(this);
    fileLoaded = false;
}

Bootloader::~Bootloader()
{

}



void Bootloader::tryConnect()
{
    if (serialThread->isOpen()){

        log( "try to connect..");
        uint32_t timeout = RPC_getTimeout();
        RPC_setTimeout(50);
        if (serialThread->rpcEnterProgrammingMode() == RPC_SUCCESS){
            bool corr_hash = serialThread->rpcIsCorrectHash();
            if (corr_hash){
                log("bootloader found.");
                setConnState(ConnectionState::Connected);
                getDeviceInfo();
            }else{
                uint8_t hash[16];
                uint16_t version;
                serialThread->rpcGetRemoteHashVersion(hash, &version);
                log("");
                log("bootloader found but incorrect RPC hash("+ RemoteDeviceInfo::arrayToHexString(hash,16,0) +", V. "+QString::number(version)+"), disconnecting..");
                log("");
                serialThread->close();
                setConnState(ConnectionState::Disconnected);
            }
        }
        RPC_setTimeout(timeout);
    }
}

ConnectionState Bootloader::getConnectionState()
{
    return connState;
}

void Bootloader::connectComPort(QString serialPortName)
{
    settings.COMPortName = serialPortName;
    connectComPort();
}

void Bootloader::connectComPort()
{
    int baudrate = 115200;
    qDebug() << "open" << settings.COMPortName << baudrate;
    serialThread->open(settings.COMPortName,baudrate);

    if (serialThread->isOpen()){
        setConnState(ConnectionState::Connecting);

    }else{
        log("still closed");
    }
}


void Bootloader::disconnectComPort()
{
    serialThread->close();
    if (serialThread->isOpen()){
        log("still opened");

    }else{
        setConnState(ConnectionState::Disconnected);
    }
}

void Bootloader::sendfirmware()
{
#define BLOCKLENGTH 128
    FlashResultDocumentation flashResultDocumentation;
    RPC_RESULT result = RPC_SUCCESS;
    if (fwImage.isValid()){
        if (fwImage.isFileModified()){
            QMessageBox msgBox;
            msgBox.setText("Firmware image has been modified.");
            msgBox.setInformativeText("Do you want to reload the file?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Yes){
                loadFile(fwImage.fileName);
            }
        }
    }
    flashResultDocumentation.setNewFirmwareDescriptor(fwImage);
    if (fwImage.isValid()){
        result = getDeviceInfo();
        flashResultDocumentation.addActionResult("GetInfo",result);
        flashResultDocumentation.setRemoteDeviceInfo(remoteDeviceInfo);
        flashResultDocumentation.checkPlausibility();


        emit onProgress(0);

        qint64 fileSize =  fwImage.binary.size();
        qint64 byteCounter = 0;
        QTime runtime;
        QTime totalRuntime;
        bool fail = false;

        if (!flashResultDocumentation.showWarngingMessage()){
            fail = true;
            log("cancelled due to plausibility issues.");
            flashResultDocumentation.addActionResult("Plausibility",RPC_FAILURE);
            result = RPC_FAILURE;
        }else{
            flashResultDocumentation.addActionResult("Plausibility",RPC_SUCCESS);
            result = RPC_SUCCESS;
        }

        runtime.start();
        totalRuntime.start();

#if 1
        if (!fail){
            log("erasing..");
            RPC_setTimeout(20*1000);
            result = serialThread->rpcEraseFlash();
            flashResultDocumentation.addActionResult("Erase",result);
            if (result != RPC_SUCCESS){
                fail = true;
                log("erasing fail");
            }else{
                log("erase ok. " + QString::number(runtime.elapsed()/1000.0)+" seconds needed");
            }
        }


#endif


        if (!fail){
            log("Initializing transfer..");
            result = serialThread->rpcInitFirmwareTransfer(fwImage);
            flashResultDocumentation.addActionResult("Initialization",result);
            if (result != RPC_SUCCESS){
                fail = true;
                log("Transfer init fail.");
            }else{
                log("Transfer init ok. ");
            }

            runtime.start();
        }
        RPC_setTimeout(1*1000);
#if 1
        {
#if 1
            QBuffer firmwareFile(&fwImage.binary);
            firmwareFile.open(QIODevice::ReadOnly);
            while (!firmwareFile.atEnd() && fail == false){

                char blockData[BLOCKLENGTH];
                char blockDataTest[BLOCKLENGTH];
                memset(blockData,0, BLOCKLENGTH);
                byteCounter += firmwareFile.read(blockData,BLOCKLENGTH);
                result = serialThread->rpcWriteFirmwareBlock((uint8_t*)blockData,BLOCKLENGTH);
                if (result != RPC_SUCCESS){
                    fail = true;
                    break;
                }

                memset(blockDataTest,0, BLOCKLENGTH);
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
                qint64 progress = 100*byteCounter;
                progress /=  fileSize ;
                if (progress_old != progress){
                    log("progress: " + QString::number(progress)+ "%");
                    emit onProgress(progress);
                }
                progress_old = progress;

            }
#endif
        }
#endif
        flashResultDocumentation.addActionResult("Transfer",result);
        if ((result != RPC_SUCCESS) || fail){
            QString resultstr;
            switch(result){
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
           log(resultstr);
        }else{
            log("tranfer ok. " +QString::number( runtime.elapsed()/1000.0)+ " seconds needed. In Total: "+QString::number(totalRuntime.elapsed()/1000.0)+ "seconds needed.");
        }

        if (!fail){
            result = serialThread->rpcVerifyChecksum();
            flashResultDocumentation.addActionResult("Verify",result);
            if (result != RPC_SUCCESS){
                fail = true;
                log("verify fail");
            }else{
                log("verify ok. ");
            }
        }

        (void)fileSize;
        (void)byteCounter;

    }else{
       log("firmware file not valid ");
    }
    flashResultDocumentation.save(settings);
    emit onFinished();
}



RPC_RESULT Bootloader::getDeviceInfo()
{
    mcu_descriptor_t descriptor;
    device_descriptor_v1_t deviceDescriptor;
    firmware_descriptor_t  firmwareDescriptor;
    bool rpcOK = true;

    remoteDeviceInfo.unSet();
    RPC_RESULT result = serialThread->rpcGetMCUDescriptor(&descriptor);
    if (!result == RPC_SUCCESS){
        rpcOK = false;
    }
    result = serialThread->rpcGetDeviceDescriptor(&deviceDescriptor);
    if (!result == RPC_SUCCESS){
        rpcOK = false;
    }
    result = serialThread->rpcGetFirmwareDescriptor(&firmwareDescriptor);
    if (!result == RPC_SUCCESS){
        rpcOK = false;
    }

    if (rpcOK){
        log( "mcu info request ok");
        remoteDeviceInfo.setDeviceDescriptor(&deviceDescriptor);
        remoteDeviceInfo.setMCUDescriptor(&descriptor);
        remoteDeviceInfo.setOldFirmwareDescriptor(&firmwareDescriptor);
        plausibilityCheck.checkPlausibiltity(remoteDeviceInfo,fwImage);
    }else{
       log("mcu info request error ("+QString::number(result)+")");
    }

    emit onMCUGotDeviceInfo();
    return result;
}

void Bootloader::runApplication()
{
    RPC_RESULT result = serialThread->rpcRunApplication();
    log("starting application..");
    if (result == RPC_SUCCESS){
        qDebug() << "application startet";
    }else{
        qDebug() << "application start failed"<<result;
    }
    disconnectComPort();
}

void Bootloader::log(QString str)
{
    qDebug() << str;
    emit onLog(str);
}

void Bootloader::setConnState(ConnectionState connState)
{
    if (connState == ConnectionState::Disconnected){
        remoteDeviceInfo.unSet();
        emit onMCUGotDeviceInfo();
    }
    this->connState = connState;
    emit onConnStateChanged(connState);
}



void Bootloader::loadFile(QString fileName)
{
    if (fwImage.open(fileName)){
        fileLoaded = true;

    }else{
        fileLoaded = false;
        fwImage.clear();
        log("couldnt load file "+fileName);
    }
    plausibilityCheck.checkPlausibiltity(remoteDeviceInfo,fwImage);
    fileNameToSend = fileName;
}

void Bootloader::saveSettings()
{
    settings.save();
}

QString Bootloader::getComPortName()
{
    return settings.COMPortName;
}



