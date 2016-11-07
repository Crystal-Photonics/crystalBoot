#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "infodialog.h"
#include "options.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QTime>
#include <QFileDialog>
#include <QPainter>
#include <QBuffer>
#include <QMessageBox>

channel_codec_instance_t channel_codec_instance[channel_codec_comport_COUNT];



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(parent)
{

    ui->setupUi(this);
    fileLoaded = false;
    ui->progressBar->setVisible(false);

    QSettings lastSettings("crystalBoot.ini", QSettings::IniFormat, parent);

    settings.load(lastSettings.value("lastSettingsfile","crytalBootSettings.ini").toString());

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



    qDebug() << "gui:" << QThread::currentThreadId();


    serialThread = new SerialThread(this);
    setConnState(MainWindow::Disconnected);
    loadUIFromSettigns();
    loadFile(settings.imageFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadUIFromSettigns(){
    cmbPort->setCurrentIndex( cmbPort->findText(settings.COMPortName));

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings.imageFile = fileNameToSend;
    settings.COMPortName = cmbPort->currentText();
    settings.save();
    event->accept();
}

void MainWindow::paintEvent(QPaintEvent *event){
    QMainWindow::paintEvent(event);
    loadFirmwarePathUIFromFile();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);
   loadFirmwarePathUIFromFile();
}

void MainWindow::loadUIFromFile(){
    ui->lbl_nf_name->setText(fwImage.firmware_name);
    ui->lbl_nf_namehash->setText("0x"+QString::number( fwImage.getNameCRC16(),16).toUpper());
    ui->lbl_nf_version->setText(fwImage.firmware_version);
    if (fwImage.crypto == FirmwareImage::Crypto::AES128)
        ui->lbl_nf_crypto->setText("AES");
    else if (fwImage.crypto == FirmwareImage::Crypto::Plain){
        ui->lbl_nf_crypto->setText("Plaintext");
    }
    ui->lbl_nf_githash->setText("0x"+QString::number( fwImage.firmware_githash,16).toUpper());
    ui->lbl_nf_gitdate->setText(fwImage.firmware_gitdate_dt.toString("yyyy.MM.dd HH:mm"));
    ui->lbl_nf_size->setText( QString::number( fwImage.firmware_size/1024,10) +"kB ("+  QString::number( fwImage.firmware_size,10)+" Bytes)");
    ui->lbl_nf_entrypoint->setText("0x"+QString::number( fwImage.firmware_entryPoint,16).toUpper());
    loadFirmwarePathUIFromFile();
    if (fwImage.firmwareLoadConsistency){
        ui->lbl_nf_consistency->setText("OK");
    }else{
        ui->lbl_nf_consistency->setText("failed");
    }

}

void MainWindow::loadFirmwarePathUIFromFile()
{
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
                log("bootloader found but incorrect RPC hash("+ RemoteDeviceInfo::arrayToHexString(hash,16,0) +", V. "+QString::number(version)+"), disconnecting..");
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
    FlashResultDocumentation flashResultDocumentation;
    RPC_RESULT result = RPC_SUCCESS;
    if (fwImage.isValid()){
        if (fwImage.isFileModified()){
            QMessageBox msgBox;
            msgBox.setText("Firmware image has been modified.");
            msgBox.setInformativeText("Do you want to reload the file?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Yes){
                loadFile(fwImage.fileName);
            }
        }
    }
    flashResultDocumentation.setNewFirmwareDescriptor(fwImage);
    if (fwImage.isValid()){
        result = getDeviceInfo();
        flashResultDocumentation.addActionResult("GetInfo",result);
        flashResultDocumentation.setRemoteDeviceInfo(remoteDeviceInfo);

        ui->progressBar->setValue(0);
        ui->progressBar->setVisible(true);

        qint64 fileSize =  fwImage.binary.size();
        qint64 byteCounter = 0;
        QTime runtime;
        QTime totalRuntime;
        bool fail = false;
        runtime.start();
        totalRuntime.start();
        log("erasing..");
        RPC_setTimeout(20*1000);
#if 1
        result = serialThread->rpcEraseFlash();
        flashResultDocumentation.addActionResult("Erase",result);
        if (result != RPC_SUCCESS){
            fail = true;
           log("erasing fail");
        }else{
            log("erase ok. " + QString::number(runtime.elapsed()/1000.0)+" seconds needed");
        }
#endif
        log("Initializing transfer..");

        if (!fail){
            result = serialThread->rpcInitFirmwareTransfer(fwImage);
            flashResultDocumentation.addActionResult("Initialization",result);
            if (result != RPC_SUCCESS){
                fail = true;
                log("Transfer init fail.");
            }else{
                log("Transfer init ok. ");
            }

            runtime.start();
        }
        RPC_setTimeout(1*1000);
#if 1
        {
#if 1
            QBuffer firmwareFile(&fwImage.binary);
            firmwareFile.open(QIODevice::ReadOnly);
            while (!firmwareFile.atEnd() && fail == false){

                char blockData[BLOCKLENGTH];
                char blockDataTest[BLOCKLENGTH];
                memset(blockData,0, BLOCKLENGTH);
                byteCounter += firmwareFile.read(blockData,BLOCKLENGTH);
                result = serialThread->rpcWriteFirmwareBlock((uint8_t*)blockData,BLOCKLENGTH);
                if (result != RPC_SUCCESS){
                    fail = true;
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
#endif
        }
#endif
        flashResultDocumentation.addActionResult("Transfer",result);
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

        if (!fail){
            result = serialThread->rpcVerifyChecksum();
            flashResultDocumentation.addActionResult("Verify",result);
            if (result != RPC_SUCCESS){
                fail = true;
                log("verify fail");
            }else{
                log("verify ok. ");
            }
        }

        (void)fileSize;
        (void)byteCounter;

    }else{
       log("firmware file not valid ");
    }
    flashResultDocumentation.save(settings);
    ui->progressBar->setVisible(false);
}

void MainWindow::loadUIDeviceInfo(){
    if (remoteDeviceInfo.isValid()){

        ui->plainTextEdit->clear();
        ui->plainTextEdit->appendPlainText(QString("MCU devID: ")+remoteDeviceInfo.getMCU_DeviceIDString()+" "+remoteDeviceInfo.getDeviceCategorieString());
        ui->plainTextEdit->appendPlainText(QString("MCU revID: ")+remoteDeviceInfo.getMCU_RevString());
        ui->plainTextEdit->appendPlainText(QString("Flashsize: ")+QString::number(remoteDeviceInfo.mcu_descriptor.flashsize/1024)+"kB ("+remoteDeviceInfo.getMCU_FlashsizeString()+")");
        ui->plainTextEdit->appendPlainText(QString("Unique ID: ")+remoteDeviceInfo.getMCU_UniqueIDString(0));
        ui->plainTextEdit->appendPlainText(QString(""));
        ui->plainTextEdit->appendPlainText(QString("Available flashsize: ")+remoteDeviceInfo.getMCU_AvailFlashSizeString());
        ui->plainTextEdit->appendPlainText(QString("Entrypoint: ")+remoteDeviceInfo.getMCU_EntryPointString());
        ui->plainTextEdit->appendPlainText(QString("minimal entrypoint: ")+remoteDeviceInfo.getMCU_MinimalEntryPointString());


        ui->lblMCU_devid->setText(remoteDeviceInfo.getMCU_DeviceIDString()+" "+remoteDeviceInfo.getDeviceCategorieString());
        ui->lblMCU_avSize->setText(QString::number(remoteDeviceInfo.mcu_descriptor.availFlashSize/1024)+"kb");
        ui->lblMCU_entryPoint->setText(remoteDeviceInfo.getMCU_EntryPointString());
        ui->lblMCU_fsize->setText(QString::number(remoteDeviceInfo.mcu_descriptor.flashsize/1024)+"kB");
        ui->lblMCU_guid->setText(remoteDeviceInfo.getMCU_UniqueIDString(4));
        if (remoteDeviceInfo.mcu_descriptor.cryptoRequired){
            ui->lblMCU_cryptoRequered->setText("yes");
        }else{
            ui->lblMCU_cryptoRequered->setText("no");
        }


        ui->lblBL_GitDate->setText( remoteDeviceInfo.getDEV_gitDate_str());
        ui->lblBL_GitHash->setText(remoteDeviceInfo.getDEV_gitHash());
        ui->lblBL_Name->setText(remoteDeviceInfo.getDEV_name());
        ui->lblBL_Version->setText(remoteDeviceInfo.getDEV_version());
        ui->lblBL_ID->setText(remoteDeviceInfo.getDEV_deviceID());


        ui->lbl_rf_githash->setText(remoteDeviceInfo.getFW_gitHash());
        ui->lbl_rf_gitdate->setText(remoteDeviceInfo.getFW_gitDate_str());
        ui->lbl_rf_namehash->setText(remoteDeviceInfo.getFW_nameCRC());
        ui->lbl_rf_name->setText(remoteDeviceInfo.getFW_name());
        ui->lbl_rf_version->setText(remoteDeviceInfo.getFW_version());
    }



}

RPC_RESULT MainWindow::getDeviceInfo()
{
    mcu_descriptor_t descriptor;
    device_descriptor_v1_t deviceDescriptor;
    firmware_descriptor_t  firmwareDescriptor;
    bool rpcOK = true;

    remoteDeviceInfo.unSet();
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
        log( "mcu info request ok");
        remoteDeviceInfo.setDeviceDescriptor(&deviceDescriptor);
        remoteDeviceInfo.setMCUDescriptor(&descriptor);
        remoteDeviceInfo.setOldFirmwareDescriptor(&firmwareDescriptor);
    }else{
       log("mcu info request error ("+QString::number(result)+")");
    }
    loadUIDeviceInfo();
    return result;
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
    connectComPort(false);
}



void MainWindow::refreshComPortList()
{
    cmbPort->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        cmbPort->addItem(info.portName());
    }
    //  cmbPort->setCurrentIndex(settings.value("lastComPort",0).toInt());
    loadUIFromSettigns();

}

void MainWindow::log(QString str)
{
    qDebug() << str;
    ui->plainTextEdit->appendPlainText(str);
    QApplication::processEvents();
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
    dialog.selectFile(fileNameToSend);
    dialog.setDirectory(fileNameToSend);
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

void MainWindow::on_actionOptions_triggered()
{
    OptionsDiag optionDiag(&settings,this);
    optionDiag.exec();

}

void MainWindow::on_actionClose_triggered()
{
    close();
}


