#include <qdebug.h>
#include <thread>
#include <chrono>
#include "serialworker.h"
#include "channel_codec/channel_codec.h"
#include "errorlogger/generic_eeprom_errorlogger.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_qt2mcu.h"

SerialThread* globSerialThread;

SerialWorker *serialWorkerForRPCFunc = NULL;


extern "C" RPC_TRANSMISSION_RESULT phyPushDataBuffer(const char *buffer, size_t length){
    QByteArray data = QByteArray(buffer,length);
    globSerialThread->sendByteData(data);
    return RPC_TRANSMISSION_SUCCESS;
}

extern "C" void ChannelCodec_errorHandler(channelCodecErrorNum_t ErrNum){
    (void)ErrNum;
}

SerialThread::SerialThread(QObject *parent) :
    QObject(parent)
{
    channel_init();
    thread = new QThread();

    globSerialThread = this;
    serialWorker = new SerialWorker();
    serialWorkerForRPCFunc = serialWorker;
    serialWorker->moveToThread(thread);

    connect(thread, SIGNAL(started()), serialWorker, SLOT(process()));
    connect(serialWorker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(serialWorker, SIGNAL(finished()), serialWorker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));


    connect(serialWorker,SIGNAL(updateADC(float)),this,SIGNAL(updateADC(float)));
    connect(serialWorker,SIGNAL(updateKeyState(rpcKeyStatus_t)),this,SIGNAL(updateKeyState(rpcKeyStatus_t)));

    connect(this, SIGNAL(openPort(QString, int)), serialWorker, SLOT(openPort(QString, int)));
    connect(this, SIGNAL(closePort()), serialWorker, SLOT(closePort()));
    connect(this, SIGNAL(isPortOpened()), serialWorker, SLOT(isPortOpened()));
    connect(this, SIGNAL(sendData(QByteArray)), serialWorker, SLOT(sendData(QByteArray)));



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

void SerialThread::rpcSetTemperature(float temperature)
{
    uint16_t temp_returnvalue;
    RPC_TRANSMISSION_RESULT result;
    rpcLEDStatus_t ledStatus = rpcLEDStatus_none;
    if (temperature > 10){
        ledStatus = rpcLEDStatus_on;
    }else{
        ledStatus = rpcLEDStatus_off;
    }

    result = mcuSetLEDStatus(&temp_returnvalue, ledStatus);
    QString resultstr;
    switch(result){
    case RPC_TRANSMISSION_SUCCESS:
        resultstr = "RPC_TRANSMISSION_SUCCESS";
        break;
    case RPC_TRANSMISSION_FAILURE:
        resultstr = "RPC_TRANSMISSION_FAILURE";
        break;
    case RPC_TRANSMISSION_COMMAND_UNKNOWN:
        resultstr = "RPC_TRANSMISSION_COMMAND_UNKNOWN";
        break;
    case RPC_TRANSMISSION_COMMAND_INCOMPLETE:
        resultstr = "RPC_TRANSMISSION_COMMAND_INCOMPLETE";
        break;
    }

    qDebug() << "sending data return: " << temp_returnvalue << " with : "<< resultstr;
    //qDebug()<<" rpcSetTemperature threadid "<<QThread::currentThreadId();
}

void SerialThread::sendByteData(QByteArray data)
{
    emit sendData(data);
}



SerialWorker::SerialWorker( QObject *parent):
    QObject(parent)
{
    serialport = new QSerialPort(this);
    connect(serialport,SIGNAL(readyRead()),this,SLOT(on_readyRead()));
}

void SerialWorker::wrapUpdateADC(float adc1)
{
    emit updateADC(adc1);
}

void SerialWorker::wrapUpdateKeyState(rpcKeyStatus_t keyState)
{
    emit updateKeyState(keyState);
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

    if (inbuffer.count() == 512){
        qDebug() << "Rechner langsam";
    }
    for (int i=0;i<inbuffer.count();i++){
        channel_push_byte_to_RPC(inbuffer[i]);
        if (inbuffer[i] == '\n'){
           // qDebug() << linebuffer;

            linebuffer = "";
        }else{
            linebuffer += inbuffer[i];
        }
    }

}
