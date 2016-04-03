#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_btnRefresh_clicked();

    QStringList bdl;
    bdl << "75" << "300" << "1200" << "2400" << "4800" << "9600" << "14400" << "19200" << "28800" << "38400" << "57600" << "115200";
    ui->cmbBaud->addItems(bdl);
    ui->cmbBaud->setCurrentIndex(7);

    qDebug() << "gui:" << QThread::currentThreadId();


    serialThread = new SerialThread(this);
    connect(serialThread, SIGNAL(updateTemperature(float)), this, SLOT(updateTemperature(float)));
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::updateTemperature(float temperature)
{
    ui->plainTextEdit->appendPlainText(QString::number(temperature));
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
                qDebug() << "opened";
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
