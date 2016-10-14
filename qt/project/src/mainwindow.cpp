#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QDebug>


channel_codec_instance_t channel_codec_instance[channel_codec_comport_COUNT];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_btnRefresh_clicked();
    qRegisterMetaType<rpcKeyStatus_t>("rpcKeyStatus_t");

    QStringList bdl;
    bdl << "75" << "300" << "1200" << "2400" << "4800" << "9600" << "14400" << "19200" << "28800" << "38400" << "57600" << "115200";
    ui->cmbBaud->addItems(bdl);
    ui->cmbBaud->setCurrentIndex(11);

    qDebug() << "gui:" << QThread::currentThreadId();


    serialThread = new SerialThread(this);
    connect(serialThread, SIGNAL(updateADC(float)), this, SLOT(updateADC(float)));
    connect(serialThread, SIGNAL(updateKeyState(rpcKeyStatus_t)), this, SLOT(updateKeyState(rpcKeyStatus_t)));
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::updateADC(float adcValue)
{
    ui->plainTextEdit->appendPlainText(QString::number(adcValue));
}

void MainWindow::updateKeyState(rpcKeyStatus_t keyState)
{
    QString statusStr;
    switch (keyState){
    case rpcKeyStatus_none:
        statusStr = "none";
        break;
    case rpcKeyStatus_pressed:
        statusStr = "pressed";
        break;
    case rpcKeyStatus_pressedLong:
        statusStr = "pressed long";
        break;
    case rpcKeyStatus_released:
        statusStr = "released";
        break;
    }
    ui->lblKey->setText("key: "+statusStr);
}

void MainWindow::on_btnRefresh_clicked()
{
    ui->cmbPort->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->cmbPort->addItem(info.portName());
    }
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
    serialThread->rpcSetTemperature(ui->spinBox->value());
}
