#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialworker.h"
#include "channel_codec/channel_codec_types.h"

#include "rpc_transmission/server/app/mcu2qt.h"
extern channel_codec_instance_t channel_codec_instance[channel_codec_comport_COUNT];


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateADC(float adcValue);
    void updateKeyState(rpcKeyStatus_t keyState);

private slots:
    void on_btnRefresh_clicked();
    void on_btnConnect_clicked();


    void on_btnSend_clicked();

private:
    Ui::MainWindow *ui;

    SerialThread* serialThread;


};

#endif // MAINWINDOW_H
