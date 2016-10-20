#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QCloseEvent>
#include "serialworker.h"
#include "channel_codec/channel_codec_types.h"

#include "rpc_transmission/server/app/mcu2qt.h"
extern channel_codec_instance_t channel_codec_instance[channel_codec_comport_COUNT];
void RPC_SET_timeout(uint32_t timeout_ms);

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

private slots:
    void on_btnRefresh_clicked();
    void on_btnConnect_clicked();


    void on_btnSend_clicked();

    void on_btnRunApplication_clicked();

private:
    Ui::MainWindow *ui;

    SerialThread* serialThread;
    QSettings settings;


};

#endif // MAINWINDOW_H
