#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include "rpc_transmission/server/app/mcu2qt.h"
#include "rpc_transmission/server/generated_general/RPC_types.h"

struct SerialThread;

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(SerialThread* serialThread, QObject *parent = 0);
    void wrapUpdateADC(float adc1);
    void wrapUpdateKeyState(rpcKeyStatus_t keyState);

public slots:

#if 1
    void openPort(QString name, int baudrate);
    void closePort();
    bool isPortOpened();
    void sendData(QByteArray data);
#endif
    void process();

signals:
    void finished();

    void updateADC(float adc1);
    void updateKeyState(rpcKeyStatus_t keyState);

private slots:
    void on_readyRead();
private:
    QSerialPort* serialport;
    QString linebuffer;

};

struct SerialThread : public QObject
{
    Q_OBJECT
public:
    explicit SerialThread(QObject *parent = 0);
    QThread* thread;
    //QSerialPort* serialport;
    SerialWorker * serialWorker;
    void open(QString name, int baudrate);
    void close();
    bool isOpen();

    void rpcSetTemperature(float temperature);
    void sendByteData(QByteArray data);
    bool rpcIsCorrectHash(void);
signals:

    void openPort(QString name, int baudrate);
    void closePort(void);
    bool isPortOpened();
    void sendData(QByteArray data);

    void updateADC(float adc1);
    void updateKeyState(rpcKeyStatus_t keyState);


public slots:

};

extern SerialWorker *serialWorkerForRPCFunc;

inline QDebug &operator << (QDebug &&qd, RPC_RESULT result){
    switch (result){
    case RPC_SUCCESS:
        qd << "RPC success";
        break;
    case RPC_FAILURE:
        qd << "RPC failure";
        break;
    case RPC_COMMAND_UNKNOWN:
        qd << "RPC command unknown";
        break;
    case RPC_COMMAND_INCOMPLETE:
        qd << "RPC command incomplete";
        break;
    }
    return qd;
}

#endif // SERIALWORKER_H
