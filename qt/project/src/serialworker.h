#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/QSerialPort>



class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QSerialPort *serialport, QObject *parent = 0);
    void wrapUpdateTemperature(float temperature);


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
    void updateTemperature(float temperature);

private slots:
    void on_readyRead();
private:
    QSerialPort* serialport;
    QString linebuffer;
};

class SerialThread : public QObject
{
    Q_OBJECT
public:
    explicit SerialThread(QObject *parent = 0);
    QThread* thread;
    QSerialPort* serialport;
    SerialWorker * serialWorker;
    void open(QString name, int baudrate);
    void close();
    bool isOpen();

    void rpcSetTemperature(float temperature);
    void sendByteData(QByteArray data);
signals:

    void openPort(QString name, int baudrate);
    void closePort(void);
    bool isPortOpened();
    void updateTemperature(float temperature);
    void sendData(QByteArray data);

public slots:

};

extern SerialWorker *serialWorkerForRPCFunc;

#endif // SERIALWORKER_H
