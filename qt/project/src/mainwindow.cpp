#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QTime>


channel_codec_instance_t channel_codec_instance[channel_codec_comport_COUNT];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings("crystalBoot.ini", QSettings::IniFormat, parent)
{

    ui->setupUi(this);
    on_btnRefresh_clicked();
    //qRegisterMetaType<rpcKeyStatus_t>("rpcKeyStatus_t");

    QStringList bdl;
    bdl << "75" << "300" << "1200" << "2400" << "4800" << "9600" << "14400" << "19200" << "28800" << "38400" << "57600" << "115200";
    ui->cmbBaud->addItems(bdl);
    ui->cmbBaud->setCurrentIndex(11);

    ui->edtFileName->setText(settings.value("lastFirmwareFile","").toString());
    qDebug() << "gui:" << QThread::currentThreadId();


    serialThread = new SerialThread(this);
    //connect(serialThread, SIGNAL(updateADC(float)), this, SLOT(updateADC(float)));

}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("lastFirmwareFile",ui->edtFileName->text());
    settings.setValue("lastComPort",ui->cmbPort->currentIndex());
    settings.sync();
    event->accept();
}

void MainWindow::on_btnRefresh_clicked()
{
    ui->cmbPort->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->cmbPort->addItem(info.portName());
    }
    ui->cmbPort->setCurrentIndex(settings.value("lastComPort",0).toInt());

}

void MainWindow::on_btnConnect_clicked()
{
    bool shallBeOpened = true;
    QPushButton* button = ui->btnConnect;
    if( button != NULL )
    {
        if (button->property("connected").toBool() == true){
            button->setText("Connect");
            button->setProperty("connected",false);
            shallBeOpened = false;
        }else{
            button->setText("Disconnect");
            button->setProperty("connected",true);
            shallBeOpened = true;
        }

        if (shallBeOpened){
            QString serialPortName = ui->cmbPort->currentText();

            int baudrate = ui->cmbBaud->currentText().toInt();

#if 0

            int dataBits = comData->currentText().toInt();
            serialport->setDataBits((QSerialPort::DataBits)dataBits);


            QSerialPort::StopBits stopBits = stopBitDescriptionToStopBit(comStop->currentText());
            serialport->setStopBits(stopBits);

#endif



            qDebug() << "open" << serialPortName << baudrate;
            serialThread->open(serialPortName,baudrate);

			if (serialThread->isOpen()){
				if (!serialThread->rpcIsCorrectHash()){
					qDebug() << "opened but incorrect hash, closing";
					serialThread->close();
				}
				else{
                    qDebug() << "opened";
				}

            }else{
                qDebug() << "still closed";
            }

        }else{
           // qDebug() << "close" << serialport->portName();
            serialThread->close();

            if (serialThread->isOpen()){
                qDebug() << "still opened";
            }else{
                qDebug() << "closed";
            }
        }

    }
}

void MainWindow::on_btnSend_clicked()
{
#define BLOCKLENGTH 128

    QFile firmwareFile( ui->edtFileName->text());
    if (firmwareFile.open( QIODevice::ReadOnly )){
        RPC_RESULT result = RPC_SUCCESS;
        qint64 fileSize =  firmwareFile.size();
        qint64 byteCounter = 0;
        QTime runtime;
        QTime totalRuntime;
        bool fail = false;
        runtime.start();
        totalRuntime.start();
        qDebug() << "erasing..";
        RPC_SET_timeout(20*1000);
        result = serialThread->rpcEraseFlash();
        if (result != RPC_SUCCESS){
            fail = true;
            qDebug() << "erasing fail";
        }else{
            qDebug() << "erase ok. " << runtime.elapsed()/1000.0<< "seconds needed";
        }

        qDebug() << "resetting write/read pointer..";

        result = serialThread->rpcResetFirmwarePointer();
        if (result != RPC_SUCCESS){
            fail = true;
            qDebug() << "pointer reset fail.";
        }else{
            qDebug() << "pointer reset ok. ";
        }

        runtime.start();
        RPC_SET_timeout(1*1000);

#if 1
        while (!firmwareFile.atEnd() && fail == false){

            char blockData[BLOCKLENGTH];
            char blockDataTest[BLOCKLENGTH];
            memset(blockData,0, BLOCKLENGTH);
            byteCounter += firmwareFile.read(blockData,BLOCKLENGTH);
            result = serialThread->rpcWriteFirmwareBlock((uint8_t*)blockData,BLOCKLENGTH);
            if (result != RPC_SUCCESS){
                break;
            }

            memset(blockDataTest,0, BLOCKLENGTH);
            #if 0
            result = serialThread->rpcReadFirmwareBlock((uint8_t*)blockDataTest,BLOCKLENGTH);
            if (result != RPC_SUCCESS){
                break;
            }

            for(uint8_t i=0;i<BLOCKLENGTH;i++){
                if (blockData[i] != blockDataTest[i]){
                    qDebug() << "verify fail at " << i;
                    fail = true;
                    break;
                }
            }
            if (fail){
                break;
            }
#endif
            static qint64 progress_old = 100;
            qint64 progress = 100*byteCounter;
            progress /=  fileSize ;
            if (progress_old != progress){
                qDebug() << "progress: " << progress << "%";
            }
            progress_old = progress;

        }
#endif
        if ((result != RPC_SUCCESS) || fail){
            QString resultstr;
            switch(result){
            case RPC_SUCCESS:
                resultstr = "RPC_SUCCESS";
                break;
            case RPC_FAILURE:
                resultstr = "RPC_FAILURE";
                break;
            case RPC_COMMAND_UNKNOWN:
                resultstr = "RPC_COMMAND_UNKNOWN";
                break;
            case RPC_COMMAND_INCOMPLETE:
                resultstr = "RPC_COMMAND_INCOMPLETE";
                break;
            }
            qDebug() << resultstr;
        }else{
            qDebug() << "tranfer ok. " << runtime.elapsed()/1000.0<< "seconds needed. In Total: "<< totalRuntime.elapsed()/1000.0 << "seconds needed.";
        }

        (void)fileSize;
        (void)byteCounter;

    }else{
        qDebug() << "cant open file" << ui->edtFileName->text();
    }
}
