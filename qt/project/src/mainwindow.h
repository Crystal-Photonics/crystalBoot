#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QCloseEvent>
#include <QComboBox>
#include <QTimer>
#include "serialworker.h"
#include "channel_codec/channel_codec_types.h"

#include "rpc_transmission/server/app/mcu2qt.h"
#include "firmwareimage.h"

extern channel_codec_instance_t channel_codec_instance[channel_codec_comport_COUNT];

void RPC_setTimeout(uint32_t timeout_ms);
uint32_t RPC_getTimeout(void);

class TargetInfo{
    mcu_descriptor_t mcu_descriptor;
    device_descriptor_v1_t device_descriptor;
    firmware_descriptor_t remoteFirmwareDescriptor;
};

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum ConnectionState {none,Connecting, Connected, Disconnected};
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

private slots:
    void on_tryConnect_timer();
    void on_actionOpen_triggered();
    void on_actionConnect_triggered();
    void on_actionGet_Info_triggered();
    void on_actionRun_triggered();
    void on_actionTransfer_triggered();
    void on_actionRefresh_triggered();
    void on_actionInfo_triggered();
    void on_actionConnect_2_triggered();
    void on_actionTransfer_2_triggered();
    void on_actionRun_Application_triggered();
    void on_actionGet_Chip_Info_triggered();
    void on_actionOpen_Firmware_Image_triggered();
    void on_actionClose_triggered();

private:

    Ui::MainWindow *ui;

    SerialThread* serialThread;
    QSettings settings;

    QTimer *connectTimer;
    QComboBox *cmbPort;
    QString fileNameToSend;
    bool fileLoaded;
    void sendfirmware();
    void getDeviceInfo();
    void runApplication();
    void connectComPort(bool shallBeOpened);
    void refreshComPortList();
    void log(QString str);
    void loadFile(QString fileName);
    void recalcUIState();
    void setConnState(ConnectionState connState);
    ConnectionState connState;

    FirmwareImage fwImage;


    void loadUIFromFile();
    uint16_t getNameHash(QString name);
};

#endif // MAINWINDOW_H
