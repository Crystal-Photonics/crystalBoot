#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
//#include "rpc_transmission/server/app/mcu2qt.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_qt2mcu.h"
#include "rpc_transmission/server/generated_general/RPC_types.h"
#include "firmwareimage.h"

struct SerialThread;

class SerialWorker : public QObject {
    Q_OBJECT
  public:
    explicit SerialWorker(SerialThread *serialThread, QObject *parent = 0);
    ~SerialWorker();

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

  private slots:
    void on_readyRead();

  private:
    QSerialPort *serialport;
    QString linebuffer;
};

struct SerialThread : public QObject {
    Q_OBJECT
  public:
    explicit SerialThread(QObject *parent = 0);
    ~SerialThread();
    QThread *thread;
    // QSerialPort* serialport;
    SerialWorker *serialWorker;
    void open(QString name, int baudrate);
    void close();
    bool isOpen();

    void sendByteData(QByteArray data);
    bool rpcIsCorrectHash(void);
    bool rpcGetRemoteHashVersion(unsigned char hash[16], uint16_t *version);

    RPC_RESULT rpcEraseFlash();
    RPC_RESULT rpcWriteFirmwareBlock(uint8_t *data, size_t size);
    RPC_RESULT rpcReadFirmwareBlock(uint8_t *data, size_t size);
    RPC_RESULT rpcVerifyChecksum();

    RPC_RESULT rpcInitFirmwareTransfer(crystalBoolResult_t *return_value, FirmwareImage &fwImage);
    RPC_RESULT rpcRunApplication(void);
    RPC_RESULT rpcEnterProgrammingMode(void);
    RPC_RESULT rpcGetMCUDescriptor(mcu_descriptor_t *descriptor);
    RPC_RESULT rpcGetDeviceDescriptor(device_descriptor_v1_t *descriptor);
    RPC_RESULT rpcGetFirmwareDescriptor(firmware_descriptor_t *descriptor);

    RPC_RESULT rpcEEPROMErase();
    RPC_RESULT rpcEEPROMWriteBlock(uint8_t *data, uint8_t size);
    RPC_RESULT rpcEEPROMReadBlock(uint8_t *data, uint8_t size);
    RPC_RESULT rpcEEPROMGetSize(uint16_t *size);
    RPC_RESULT rpcEEPROMInitTransfer();
    RPC_RESULT rpcEEPROMVerify(uint16_t crc16);
  signals:

    void openPort(QString name, int baudrate);
    void closePort(void);
    bool isPortOpened();
    void sendData(QByteArray data);

  public slots:
};

extern SerialWorker *serialWorkerForRPCFunc;

inline QDebug &operator<<(QDebug &&qd, RPC_RESULT result) {
    switch (result) {
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
