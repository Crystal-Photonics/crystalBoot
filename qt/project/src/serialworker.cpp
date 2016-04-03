#include <qdebug.h>
#include <thread>
#include <chrono>
#include "serialworker.h"
#include "channel_codec/channel_codec.h"
#include "errorlogger/generic_eeprom_errorlogger.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_qt2mcu.h"

QSerialPort* globSerialPort;

SerialWorker *serialWorkerForRPCFunc = NULL;


extern "C" RPC_TRANSMISSION_RESULT phyPushDataBuffer(const char *buffer, size_t length){
    globSerialPort->write(buffer,length);
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
    serialport = new QSerialPort();
    globSerialPort = serialport;
    serialWorker = new SerialWorker(serialport);
    serialWorkerForRPCFunc = serialWorker;
    serialWorker->moveToThread(thread);



    connect(thread, SIGNAL(started()), serialWorker, SLOT(process()));
    connect(serialWorker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(serialWorker, SIGNAL(finished()), serialWorker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    connect(serialport,SIGNAL(readyRead()),serialWorker,SLOT(on_readyRead()));
    connect(serialWorker,SIGNAL(updateTemperature(float)),this,SIGNAL(updateTemperature(float)));
#if 0

    connect(this, SIGNAL(openPort(QString, int)), serialWorker, SLOT(openPort(QString, int)));
    connect(this, SIGNAL(closePort()), serialWorker, SLOT(closePort()));
    connect(this, SIGNAL(isPortOpened()), serialWorker, SLOT(isPortOpened()));
#endif


    thread->start();

}

void SerialThread::open(QString name, int baudrate)
{
    serialport->setPortName(name);
    serialport->setBaudRate(baudrate);
    serialport->setFlowControl(QSerialPort::NoFlowControl);
    serialport->open(QIODevice::ReadWrite);
    //emit openPort(name,baudrate);
}

void SerialThread::close()
{
    serialport->close();
    //emit closePort();
}

bool SerialThread::isOpen()
{
    bool result;
    result = serialport->isOpen();
    //    result = emit isPortOpened();
    return result;
}

void SerialThread::rpcSetTemperature(float temperature)
{
    u_int16_t temp_returnvalue;
    RPC_TRANSMISSION_RESULT result;
    result = mcuSetMCUTargetTemperature(&temp_returnvalue, temperature);
    qDebug() << "sending data return: " << temp_returnvalue << " with success: "<< result;
    qDebug()<<"rpcSetTemperature threadid "<<QThread::currentThreadId();
}



SerialWorker::SerialWorker(QSerialPort *serialport, QObject *parent):
    QObject(parent)
{
    this->serialport = serialport;
}

void SerialWorker::wrapUpdateTemperature(float temperature)
{
    emit updateTemperature(temperature);
}

#if 0
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
#endif

void SerialWorker::process()
{
    qDebug()<<"process "<<QThread::currentThreadId();
}

void SerialWorker::on_readyRead()
{

    //qDebug() << "on read ID:" << thread()->currentThreadId();
    qDebug() << "on read:" << QThread::currentThreadId();
    QByteArray inbuffer = serialport->readAll();

    if (inbuffer.count() == 512){
        qDebug() << "Rechner langsam";
    }
    for (int i=0;i<inbuffer.count();i++){
        channel_push_byte_to_RPC(inbuffer[i]);
        if (inbuffer[i] == '\n'){
            qDebug() << linebuffer;

            linebuffer = "";
        }else{
            linebuffer += inbuffer[i];
        }
    }

}
