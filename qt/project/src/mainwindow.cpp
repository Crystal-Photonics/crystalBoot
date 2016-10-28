#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "infodialog.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QTime>
#include <QFileDialog>
#include <QPainter>
#include <QBuffer>


channel_codec_instance_t channel_codec_instance[channel_codec_comport_COUNT];

QString arrayToHexString(uint8_t *buff, const size_t length, int insertNewLine){
    QString result;
    int insNewLineCounter=insertNewLine;
    for (size_t i=0;i<length;i++){
        result += "0x"+QString::number(buff[i],16).toUpper();
        if (i<length-1){
            result += ", ";
            if (insertNewLine){
                insNewLineCounter--;
                if (insNewLineCounter==0){
                    result += "\n";
                    insNewLineCounter = insertNewLine;
                }
            }
        }
    }
    return result;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings("crystalBoot.ini", QSettings::IniFormat, parent)
{

    ui->setupUi(this);
    fileLoaded = false;
    ui->progressBar->setVisible(false);
    cmbPort = new QComboBox (this);
    ui->mainToolBar->addAction(ui->actionOpen);
    ui->mainToolBar->addWidget(cmbPort);
    ui->mainToolBar->addAction(ui->actionRefresh);
    ui->mainToolBar->addAction(ui->actionConnect);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionTransfer);
    ui->mainToolBar->addAction(ui->actionRun);
    refreshComPortList();
    connectTimer = new QTimer(this);
    connect(connectTimer, SIGNAL(timeout()), this, SLOT(on_tryConnect_timer()));
    //qRegisterMetaType<rpcKeyStatus_t>("rpcKeyStatus_t");

    QStringList bdl;
    bdl << "75" << "300" << "1200" << "2400" << "4800" << "9600" << "14400" << "19200" << "28800" << "38400" << "57600" << "115200";

    cmbPort->setCurrentIndex( settings.value("lastComPort",0).toInt());
        qDebug() << "gui:" << QThread::currentThreadId();


    serialThread = new SerialThread(this);
    setConnState(MainWindow::Disconnected);
    loadFile(settings.value("lastFirmwareFile","").toString());
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("lastFirmwareFile",fileNameToSend);
    settings.setValue("lastComPort",cmbPort->currentIndex());
    settings.sync();
    event->accept();
}

void MainWindow::on_tryConnect_timer()
{
    if (serialThread->isOpen()){

        log( "try to connect..");
        uint32_t timeout = RPC_getTimeout();
        RPC_setTimeout(50);
        if (serialThread->rpcEnterProgrammingMode() == RPC_SUCCESS){
            connectTimer->stop();
            bool corr_hash = serialThread->rpcIsCorrectHash();
            if (corr_hash){
                log("bootloader found.");
                setConnState(MainWindow::Connected);
                getDeviceInfo();
            }else{
                uint8_t hash[16];
                uint16_t version;
                serialThread->rpcGetRemoteHashVersion(hash, &version);
                log("");
                log("bootloader found but incorrect RPC hash("+arrayToHexString(hash,16,0) +", V. "+QString::number(version)+"), disconnecting..");
                log("");
                serialThread->close();
                setConnState(MainWindow::Disconnected);
            }
        }
        RPC_setTimeout(timeout);
    }
}

void MainWindow::connectComPort(bool shallBeOpened)
{
    if (shallBeOpened){
        QString serialPortName = cmbPort->currentText();
        int baudrate = 115200;//ui->cmbBaud->currentText().toInt();
        qDebug() << "open" << serialPortName << baudrate;
        serialThread->open(serialPortName,baudrate);

        if (serialThread->isOpen()){
            setConnState(MainWindow::Connecting);
            connectTimer->start(10);

        }else{
            log("still closed");
        }
    }else{
        serialThread->close();

        if (serialThread->isOpen()){
            log("still opened");

        }else{
            setConnState(MainWindow::Disconnected);
        }
    }
}


void MainWindow::sendfirmware()
{
#define BLOCKLENGTH 128

    if (fwImage.isValid()){

        ui->progressBar->setValue(0);
        ui->progressBar->setVisible(true);
        RPC_RESULT result = RPC_SUCCESS;
        qint64 fileSize =  fwImage.binary.size();
        qint64 byteCounter = 0;
        QTime runtime;
        QTime totalRuntime;
        bool fail = false;
        runtime.start();
        totalRuntime.start();
        log("erasing..");
        RPC_setTimeout(20*1000);
        result = serialThread->rpcEraseFlash();
        if (result != RPC_SUCCESS){
            fail = true;
           log("erasing fail");
        }else{
            log("erase ok. " + QString::number(runtime.elapsed()/1000.0)+" seconds needed");
        }

        log("Initializing transfer..");

        result = serialThread->rpcInitFirmwareTransfer(fwImage);
        if (result != RPC_SUCCESS){
            fail = true;
             log("Transfer init fail.");
        }else{
             log("Transfer init ok. ");
        }

        runtime.start();
        RPC_setTimeout(1*1000);

#if 1
        {
            QBuffer firmwareFile(&fwImage.binary);
            firmwareFile.open(QIODevice::ReadOnly);
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
                    log("progress: " + QString::number(progress)+ "%");
                    ui->progressBar->setValue(progress);
                }
                progress_old = progress;

            }
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
           log(resultstr);
        }else{
            log("tranfer ok. " +QString::number( runtime.elapsed()/1000.0)+ " seconds needed. In Total: "+QString::number(totalRuntime.elapsed()/1000.0)+ "seconds needed.");
        }

        (void)fileSize;
        (void)byteCounter;

    }else{
       log("firmware file not valid ");
    }
    ui->progressBar->setVisible(false);
}

void MainWindow::getDeviceInfo()
{
    mcu_descriptor_t descriptor;
    device_descriptor_v1_t deviceDescriptor;
    firmware_descriptor_t  firmwareDescriptor;
    bool rpcOK = true;

    RPC_RESULT result = serialThread->rpcGetMCUDescriptor(&descriptor);
    if (!result == RPC_SUCCESS){
        rpcOK = false;
    }
    result = serialThread->rpcGetDeviceDescriptor(&deviceDescriptor);
    if (!result == RPC_SUCCESS){
        rpcOK = false;
    }
    result = serialThread->rpcGetFirmwareDescriptor(&firmwareDescriptor);
    if (!result == RPC_SUCCESS){
        rpcOK = false;
    }

    if (rpcOK){
        log( "mcu info requested");

        QString cat;
        if (descriptor.devID == 0x416){
           cat = "Cat.1 device";
        }else if(descriptor.devID == 0x429){
            cat = "Cat.2 device";
        }else if(descriptor.devID == 0x427){
            cat = "Cat.3 device";
        }else if(descriptor.devID == 0x436){
            cat = "Cat.4 device or Cat.3 device";
        }else if(descriptor.devID == 0x437){
            cat = "Cat.5 device or Cat.6 device";
        }
        ui->plainTextEdit->clear();
        ui->plainTextEdit->appendPlainText(QString("MCU devID: ")+"0x"+QString::number(descriptor.devID,16).toUpper()+" "+cat);
        ui->plainTextEdit->appendPlainText(QString("MCU revID: ")+"0x"+QString::number(descriptor.revision,16).toUpper());
        ui->plainTextEdit->appendPlainText(QString("Flashsize: ")+QString::number(descriptor.flashsize/1024)+"kB (0x"+QString::number(descriptor.flashsize,16).toUpper()+")");
        ui->plainTextEdit->appendPlainText(QString("Unique ID: ")+arrayToHexString(descriptor.guid,12,0));
        ui->plainTextEdit->appendPlainText(QString(""));
        ui->plainTextEdit->appendPlainText(QString("Available flashsize: ")+"0x"+QString::number(descriptor.availFlashSize,16).toUpper());
        ui->plainTextEdit->appendPlainText(QString("Entrypoint: ")+"0x"+QString::number(descriptor.firmwareEntryPoint,16).toUpper());
        ui->plainTextEdit->appendPlainText(QString("minimal entrypoint: ")+"0x"+QString::number(descriptor.minimalFirmwareEntryPoint,16).toUpper());


        ui->lblMCU_devid->setText("0x"+QString::number(descriptor.devID,16).toUpper()+" "+cat);
        ui->lblMCU_avSize->setText(QString::number(descriptor.availFlashSize/1024)+"kb");
        ui->lblMCU_entryPoint->setText("0x"+QString::number(descriptor.firmwareEntryPoint,16).toUpper());
        ui->lblMCU_fsize->setText(QString::number(descriptor.flashsize/1024)+"kB");
        ui->lblMCU_guid->setText(arrayToHexString(descriptor.guid,12,4));
        char blName[12];
        char blVersion[9];
        blName[12] = 0;
        blVersion[8] = 0;
        memcpy(blName,deviceDescriptor.name,sizeof(blName)-1 );
        memcpy(blVersion,deviceDescriptor.version,sizeof(blVersion)-1 );
        ui->lblBL_GitDate->setText( QDateTime::fromTime_t(deviceDescriptor.gitDate_unix).toString("yyyy.MM.dd"));
        ui->lblBL_GitHash->setText("0x"+QString::number(deviceDescriptor.githash,16).toUpper());
        ui->lblBL_Name->setText(blName);
        ui->lblBL_Version->setText(blVersion);
        ui->lblBL_ID->setText(QString::number(deviceDescriptor.deviceID));


        char rfName[12];
        char rfVersion[9];
        rfName[12] = 0;
        rfVersion[8] = 0;
        memcpy(rfName,firmwareDescriptor.name,sizeof(rfName)-1 );
        memcpy(rfVersion,firmwareDescriptor.version,sizeof(rfVersion)-1 );

        ui->lbl_rf_githash->setText("0x"+QString::number(firmwareDescriptor.githash,16).toUpper());
        ui->lbl_rf_gitdate->setText(QDateTime::fromTime_t(firmwareDescriptor.gitDate_unix).toString("yyyy.MM.dd HH:mm"));
        ui->lbl_rf_namehash->setText("0x"+QString::number(firmwareDescriptor.nameCRC16,16).toUpper());
        ui->lbl_rf_name->setText(rfName);
        ui->lbl_rf_version->setText(rfVersion);




    }else{
       log("mcu info request error ("+QString::number(result)+")");
    }

}

void MainWindow::runApplication()
{
    RPC_RESULT result = serialThread->rpcRunApplication();
    log("starting application..");
    if (result == RPC_SUCCESS){
        qDebug() << "application startet";
    }else{
        qDebug() << "application start failed"<<result;
    }
    setConnState(MainWindow::Disconnected);
}



void MainWindow::refreshComPortList()
{
    cmbPort->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        cmbPort->addItem(info.portName());
    }
    cmbPort->setCurrentIndex(settings.value("lastComPort",0).toInt());

}

void MainWindow::log(QString str)
{
    qDebug() << str;
    ui->plainTextEdit->appendPlainText(str);
    QApplication::processEvents();
}



void MainWindow::loadUIFromFile(){
    ui->lbl_nf_name->setText(fwImage.firmware_name);
    ui->lbl_nf_namehash->setText("0x"+QString::number( fwImage.getNameCRC16(),16).toUpper());
    ui->lbl_nf_version->setText(fwImage.firmware_version);
    ui->lbl_nf_githash->setText("0x"+QString::number( fwImage.firmware_githash,16).toUpper());
    ui->lbl_nf_gitdate->setText(fwImage.firmware_gitdate_dt.toString("yyyy.MM.dd HH:mm"));
    ui->lbl_nf_size->setText( QString::number( fwImage.firmware_size/1024,10) +"kB ("+  QString::number( fwImage.firmware_size,10)+" Bytes)");
    ui->lbl_nf_entrypoint->setText("0x"+QString::number( fwImage.firmware_entryPoint,16).toUpper());
    QPainter painter(ui->lbl_nf_path);
    QFontMetrics fontMetrics = painter.fontMetrics();

    QString elidedPath = fontMetrics.elidedText(fileNameToSend, Qt::ElideLeft, ui->lbl_nf_path->width());
    ui->lbl_nf_path->setText(elidedPath);
}

void MainWindow::loadFile(QString fileName)
{
    if (fwImage.open(fileName)){

        loadUIFromFile();
        fileLoaded = true;
    }else{
        fileLoaded = false;
        log("couldnt load file "+fileName);
    }
    fileNameToSend = fileName;
    recalcUIState();
}

void MainWindow::setConnState(MainWindow::ConnectionState connState)
{
    this->connState = connState;
    recalcUIState();
}

void MainWindow::recalcUIState()
{
    if (fileLoaded){
        ui->actionConnect->setEnabled(true);
        ui->actionConnect_2->setEnabled(true);
        switch (connState){
        case ConnectionState::Disconnected:
            statusBar()->showMessage("disconnected");
            log("disconnected");
            ui->actionConnect->setText("connect");
            ui->actionRun->setEnabled(false);
            ui->actionRun_Application->setEnabled(false);
            ui->actionGet_Chip_Info->setEnabled(false);
            ui->actionTransfer->setEnabled(false);
            ui->actionTransfer_2->setEnabled(false);
            break;
        case ConnectionState::Connected:
            statusBar()->showMessage("connected");
            log("connected");
            ui->actionConnect->setText("disconnect");
            ui->actionRun->setEnabled(true);
            ui->actionRun_Application->setEnabled(true);
            ui->actionGet_Chip_Info->setEnabled(true);
            ui->actionTransfer->setEnabled(true);
            ui->actionTransfer_2->setEnabled(true);
            break;
        case ConnectionState::Connecting:
            statusBar()->showMessage("connecting..");
            log("connecting");
            ui->actionConnect->setText("stop");
            ui->actionRun->setEnabled(false);
            ui->actionRun_Application->setEnabled(false);
            ui->actionGet_Chip_Info->setEnabled(false);
            ui->actionTransfer->setEnabled(false);
            ui->actionTransfer_2->setEnabled(false);
            break;
        case ConnectionState::none:
            break;
        }
    }else{
        ui->actionRun->setEnabled(false);
        ui->actionRun_Application->setEnabled(false);

        ui->actionConnect_2->setEnabled(false);
        ui->actionConnect->setEnabled(false);
        ui->actionGet_Chip_Info->setEnabled(false);

        ui->actionTransfer->setEnabled(false);
        ui->actionTransfer_2->setEnabled(false);
    }
}

void MainWindow::on_actionConnect_2_triggered()
{
    on_actionConnect_triggered();
}

void MainWindow::on_actionConnect_triggered()
{
    if(connState == ConnectionState::Disconnected){
        connectComPort(true);
    }else{
        connectComPort(false);
    }
}

void MainWindow::on_actionGet_Chip_Info_triggered()
{
    on_actionGet_Info_triggered();
}

void MainWindow::on_actionGet_Info_triggered()
{
    getDeviceInfo();
}

void MainWindow::on_actionRun_Application_triggered()
{
    on_actionRun_triggered();
}

void MainWindow::on_actionRun_triggered()
{
    runApplication();
}


void MainWindow::on_actionTransfer_2_triggered()
{
    on_actionTransfer_triggered();
}
void MainWindow::on_actionTransfer_triggered()
{
    sendfirmware();
}

void MainWindow::on_actionRefresh_triggered()
{
    refreshComPortList();
}

void MainWindow::on_actionOpen_Firmware_Image_triggered()
{
    on_actionOpen_triggered();
}


void MainWindow::on_actionOpen_triggered()
{
    //QString fn = "";
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    //dialog.selectFile(fn);
    //dialog.setDirectory(fn);
    dialog.setNameFilter(tr("Firmware Image (*.cfw)"));
    if (dialog.exec()){
        loadFile(dialog.selectedFiles()[0]);
       // loadUIFromSettings();
    }
}



void MainWindow::on_actionInfo_triggered()
{
    InfoDialog infodiag(this);
    infodiag.exec();
}


void MainWindow::on_actionClose_triggered()
{
    close();
}
