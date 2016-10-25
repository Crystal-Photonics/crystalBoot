#include <qdebug.h>
#include <thread>
#include <chrono>
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


extern "C" RPC_RESULT phyPushDataBuffer(channel_codec_instance_t *instance, const char *buffer, size_t length){

    if (instance->aux.portindex == channel_codec_comport_transmission){
        QByteArray data = QByteArray(buffer,length);
        SerialThread *serialthread = instance->aux.serialthread;
        assert(serialthread);
        if (serialthread){
            serialthread->sendByteData(data);
        }

    }
    return RPC_SUCCESS;
}

extern "C" void ChannelCodec_errorHandler(channel_codec_instance_t *instance, channelCodecErrorNum_t ErrNum){
    (void)ErrNum;
    (void)instance;
}

SerialThread::SerialThread(QObject *parent) :
    QObject(parent)
{

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

void SerialThread::open(QString name, int baudrate)
{

    emit openPort(name,baudrate);

}

void SerialThread::close()
{

    emit closePort();

}

bool SerialThread::isOpen()
{
    bool result;
    result = emit isPortOpened();
    return result;
}


RPC_RESULT SerialThread::rpcWriteFirmwareBlock(uint8_t *data, size_t size){

    RPC_RESULT result;
    assert(size == 128);

    crystalBoolResult_t return_value = crystalBool_OK;

    result = mcuWriteFirmwareBlock(&return_value,data);
    if (return_value == crystalBool_Fail){
        result = RPC_FAILURE;
    }
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

  //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcReadFirmwareBlock(uint8_t *data, size_t size){
    RPC_RESULT result;
    assert(size == 128);

    crystalBoolResult_t return_value = crystalBool_OK;

    result = mcuReadFirmwareBlock(&return_value,data);
    if (return_value == crystalBool_Fail){
        result = RPC_FAILURE;
    }
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

  //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcEraseFlash()
{
    RPC_RESULT result;

    crystalBoolResult_t return_value = crystalBool_OK;

    result = mcuEraseFlash(&return_value);
    if (return_value == crystalBool_Fail){
        result = RPC_FAILURE;
    }

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

  //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcResetFirmwarePointer(){
    QString resultstr;
    RPC_RESULT result = mcuResetReadWritePointerToApplicationAddress();
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

  //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcRunApplication()
{
    QString resultstr;
    RPC_RESULT result = mcuRunApplication();
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

  //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcEnterProgrammingMode()
{
    QString resultstr;
    RPC_RESULT result = mcuEnterProgrammerMode();
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

  //  qDebug() << "sending data return: "  << " with : "<< resultstr;
    return result;
}

RPC_RESULT SerialThread::rpcGetMCUDescriptor(mcu_descriptor_t *descriptor)
{

    QString resultstr;
    RPC_RESULT result = mcuGetMCUDescriptor(descriptor);
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

//  qDebug() << "sending data return: "  << " with : "<< resultstr;
return result;
}



void SerialThread::sendByteData(QByteArray data)
{
    emit sendData(data);
}



bool SerialThread::rpcIsCorrectHash(void)
{
	unsigned char hash[16];
	unsigned char start_command_id;
	uint16_t version;
	auto result = RPC_TRANSMISSION_get_hash(hash, &start_command_id, &version);
	if (result != RPC_SUCCESS){
		return false;
	}
    return memcmp(hash, RPC_TRANSMISSION_HASH, RPC_TRANSMISSION_HASH_SIZE) == 0;
}



SerialWorker::SerialWorker(SerialThread *serialThread, QObject *parent):
    QObject(parent)
{
    serialport = new QSerialPort(this);
    connect(serialport,SIGNAL(readyRead()),this,SLOT(on_readyRead()));


    channel_codec_instance[channel_codec_comport_transmission].aux.portindex = channel_codec_comport_transmission;
    channel_codec_instance[channel_codec_comport_transmission].aux.serialthread = serialThread;
    channel_init_instance(&channel_codec_instance[channel_codec_comport_transmission],
                                     channel_codec_rxbuffer[channel_codec_comport_transmission],CHANNEL_CODEC_RX_BUFFER_SIZE,
                                     channel_codec_txbuffer[channel_codec_comport_transmission],CHANNEL_CODEC_TX_BUFFER_SIZE);

}





void SerialWorker::openPort(QString name, int baudrate)
{
    serialport->setPortName(name);
    serialport->setBaudRate(baudrate);
    serialport->open(QIODevice::ReadWrite);
}

void SerialWorker::closePort()
{
    serialport->close();
}

bool SerialWorker::isPortOpened()
{
    bool result = serialport->isOpen();
    return result;
}

void SerialWorker::sendData(QByteArray data)
{
    serialport->write(data);
    //qDebug() << ">>>" << data;
}


void SerialWorker::process()
{
    qDebug()<<"process "<<QThread::currentThreadId();
}

void SerialWorker::on_readyRead()
{

    //qDebug() << "on read ID:" << thread()->currentThreadId();
    //qDebug() << "on read:" << QThread::currentThreadId();
    QByteArray inbuffer = serialport->readAll();

    if (!inbuffer.isEmpty()){
        qDebug() << "<<<" << inbuffer;
	}
    if (inbuffer.count() == 512){
        qDebug() << "Rechner langsam";
    }
    for (int i=0;i<inbuffer.count();i++){
        channel_push_byte_to_RPC(&channel_codec_instance[channel_codec_comport_transmission], inbuffer[i]);
        if (inbuffer[i] == '\n'){
           // qDebug() << linebuffer;

            linebuffer = "";
        }else{
            linebuffer += inbuffer[i];
        }
    }

}
