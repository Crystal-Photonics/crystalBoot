#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include "serialworker.h"
#include "crystalsettings.h"
#include "flashresultdocumentation.h"
#include "channel_codec/channel_codec_types.h"
#include "rpc_transmission/server/app/mcu2qt.h"

extern channel_codec_instance_t channel_codec_instance[channel_codec_comport_COUNT];

enum class ConnectionState {none,Connecting, Connected, Disconnected};

class Bootloader : public QObject
{

    Q_OBJECT
public:
    explicit Bootloader(QString settingsFileName, QObject *parent = 0);
    ~Bootloader();

    void connectComPort(QString serialPortName);
    void disconnectComPort();
    void sendfirmware();
    RPC_RESULT getDeviceInfo();
    void runApplication();
    void loadFile(QString fileName);
    void saveSettings();
    QString getComPortName();
    QString fileNameToSend;
    FirmwareImage fwImage;
    RemoteDeviceInfo remoteDeviceInfo;
    bool fileLoaded;
    CrystalBootSettings settings;
signals:
    void onConnStateChanged(ConnectionState connState);
    void onProgress(int progress);
    void onFinished();
    void onLog(QString text);
    void onMCUGotDeviceInfo();

public slots:

private slots:
    void on_tryConnect_timer();
private:
    ConnectionState connState;



    SerialThread* serialThread;


    QTimer *connectTimer;



    void log(QString str);
    void setConnState(ConnectionState connState);
};

#endif // BOOTLOADER_H
