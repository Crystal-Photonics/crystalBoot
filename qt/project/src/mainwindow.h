#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QComboBox>
#include <QTimer>
#include <QLabel>
#include "serialworker.h"
#include "crystalsettings.h"

#include "firmwareimage.h"
#include "flashresultdocumentation.h"
#include "bootloader.h"




#if 0
class TargetInfo{
    mcu_descriptor_t mcu_descriptor;
    device_descriptor_v1_t device_descriptor;
    firmware_descriptor_t remoteFirmwareDescriptor;
};
#endif
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent *event);
    void loadUIFromSettigns();


    void loadUIDeviceInfo();
    void loadUIPlausibility();
private slots:
    void on_actionOpen_triggered();
    void on_actionConnect_triggered();
    //void on_actionGet_Info_triggered();
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

    void onReconnectTimer();

    void on_actionOptions_triggered();

    void onConnStateChanged(ConnectionState connState);
    void onProgress(int progress);
    void onFinished();
    void onLog(QString text);
    void onMCUGotDeviceInfo();

    void onCmbCurrentIndexChanged(const QString &text);

    void on_actionSave_settings_as_triggered();

    void on_actionErase_EEPROM_triggered();

private:

    Ui::MainWindow *ui;


    QComboBox *cmbPort;
    QTimer *reconnectTimer;

    Bootloader bootloader;

    ConnectionState connState;
    void sendfirmware();
    RPC_RESULT getDeviceInfo();
    void runApplication();
    void connectComPort(bool shallBeOpened);
    void refreshComPortList();
    void loadFile(QString fileName);
    void recalcUIState();
    void setConnState(ConnectionState connState);



    void loadUIFromFile();
    void loadFirmwarePathUIFromFile();
    uint16_t getNameHash(QString name);
    void log(QString str);


};

#endif // MAINWINDOW_H
