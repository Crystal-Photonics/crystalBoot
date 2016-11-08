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



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    bootloader("bootloader_settings.ini",parent)
{

    ui->setupUi(this);

    ui->progressBar->setVisible(false);

    reconnectTimer = new QTimer(this);

    QSettings lastSettings("crystalBoot.ini", QSettings::IniFormat, parent);
    connect(&bootloader, SIGNAL(onConnStateChanged(ConnectionState)), this, SLOT(onConnStateChanged(ConnectionState)));
    connect(&bootloader, SIGNAL(onProgress(int)), this, SLOT(onProgress(int)));
    connect(&bootloader, SIGNAL(onFinished(void)), this, SLOT(onFinished(void)));
    connect(&bootloader, SIGNAL(onLog(QString)), this, SLOT(onLog(QString)));
    connect(&bootloader, SIGNAL(onMCUGotDeviceInfo()), this, SLOT(onMCUGotDeviceInfo()));
    connect(reconnectTimer, SIGNAL(timeout()), this, SLOT(onReconnectTimer()));




    cmbPort = new QComboBox (this);
    ui->mainToolBar->addAction(ui->actionOpen);
    ui->mainToolBar->addWidget(cmbPort);
    ui->mainToolBar->addAction(ui->actionRefresh);
    ui->mainToolBar->addAction(ui->actionConnect);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionTransfer);
    ui->mainToolBar->addAction(ui->actionRun);
    refreshComPortList();
    //qRegisterMetaType<rpcKeyStatus_t>("rpcKeyStatus_t");

    QStringList bdl;
    bdl << "75" << "300" << "1200" << "2400" << "4800" << "9600" << "14400" << "19200" << "28800" << "38400" << "57600" << "115200";

    qDebug() << "gui:" << QThread::currentThreadId();
    setConnState(ConnectionState::Disconnected);
    loadUIFromSettigns();
    loadFile(lastSettings.value("lastImageFile","").toString());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadUIFromSettigns(){
    cmbPort->setCurrentIndex( cmbPort->findText(bootloader.getComPortName()));
}



void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings lastSettings("crystalBoot.ini", QSettings::IniFormat, this);
    lastSettings.setValue("lastImageFile",bootloader.fileNameToSend);
    bootloader.saveSettings();
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
    ui->lbl_nf_name->setText(bootloader.fwImage.firmware_name);
    ui->lbl_nf_namehash->setText("0x"+QString::number( bootloader.fwImage.getNameCRC16(),16).toUpper());
    ui->lbl_nf_version->setText(bootloader.fwImage.firmware_version);
    if (bootloader.fwImage.crypto == FirmwareImage::Crypto::AES128)
        ui->lbl_nf_crypto->setText("AES");
    else if (bootloader.fwImage.crypto == FirmwareImage::Crypto::Plain){
        ui->lbl_nf_crypto->setText("Plaintext");
    }
    ui->lbl_nf_githash->setText("0x"+QString::number( bootloader.fwImage.firmware_githash,16).toUpper());
    ui->lbl_nf_gitdate->setText(bootloader.fwImage.firmware_gitdate_dt.toString("yyyy.MM.dd HH:mm"));
    ui->lbl_nf_size->setText( QString::number( bootloader.fwImage.firmware_size/1024,10) +"kB ("+  QString::number( bootloader.fwImage.firmware_size,10)+" Bytes)");
    ui->lbl_nf_entrypoint->setText("0x"+QString::number( bootloader.fwImage.firmware_entryPoint,16).toUpper());
    loadFirmwarePathUIFromFile();
    if (bootloader.fwImage.firmwareLoadConsistency){
        ui->lbl_nf_consistency->setText("OK");
    }else{
        ui->lbl_nf_consistency->setText("failed");
    }

}

void MainWindow::loadFirmwarePathUIFromFile()
{
    QPainter painter(ui->lbl_nf_path);
    QFontMetrics fontMetrics = painter.fontMetrics();

    QString elidedPath = fontMetrics.elidedText(bootloader.fileNameToSend, Qt::ElideLeft, ui->lbl_nf_path->width());
    ui->lbl_nf_path->setText(elidedPath);
}

void MainWindow::log(QString str)
{
    ui->plainTextEdit->appendPlainText(str);
    QApplication::processEvents();
}







void MainWindow::loadUIDeviceInfo(){
    if (bootloader.remoteDeviceInfo.isValid()){

        ui->plainTextEdit->clear();
        ui->plainTextEdit->appendPlainText(QString("MCU devID: ")+bootloader.remoteDeviceInfo.getMCU_DeviceIDString()+" "+bootloader.remoteDeviceInfo.getDeviceCategorieString());
        ui->plainTextEdit->appendPlainText(QString("MCU revID: ")+bootloader.remoteDeviceInfo.getMCU_RevString());
        ui->plainTextEdit->appendPlainText(QString("Flashsize: ")+QString::number(bootloader.remoteDeviceInfo.mcu_descriptor.flashsize/1024)+"kB ("+bootloader.remoteDeviceInfo.getMCU_FlashsizeString()+")");
        ui->plainTextEdit->appendPlainText(QString("Unique ID: ")+bootloader.remoteDeviceInfo.getMCU_UniqueIDString(0));
        ui->plainTextEdit->appendPlainText(QString(""));
        ui->plainTextEdit->appendPlainText(QString("Available flashsize: ")+bootloader.remoteDeviceInfo.getMCU_AvailFlashSizeString());
        ui->plainTextEdit->appendPlainText(QString("Entrypoint: ")+bootloader.remoteDeviceInfo.getMCU_EntryPointString());
        ui->plainTextEdit->appendPlainText(QString("minimal entrypoint: ")+bootloader.remoteDeviceInfo.getMCU_MinimalEntryPointString());


        ui->lblMCU_devid->setText(bootloader.remoteDeviceInfo.getMCU_DeviceIDString()+" "+bootloader.remoteDeviceInfo.getDeviceCategorieString());
        ui->lblMCU_avSize->setText(QString::number(bootloader.remoteDeviceInfo.mcu_descriptor.availFlashSize/1024)+"kb");
        ui->lblMCU_entryPoint->setText(bootloader.remoteDeviceInfo.getMCU_EntryPointString());
        ui->lblMCU_fsize->setText(QString::number(bootloader.remoteDeviceInfo.mcu_descriptor.flashsize/1024)+"kB");
        ui->lblMCU_guid->setText(bootloader.remoteDeviceInfo.getMCU_UniqueIDString(4));
        if (bootloader.remoteDeviceInfo.mcu_descriptor.cryptoRequired){
            ui->lblMCU_cryptoRequered->setText("yes");
        }else{
            ui->lblMCU_cryptoRequered->setText("no");
        }


        ui->lblBL_GitDate->setText( bootloader.remoteDeviceInfo.getDEV_gitDate_str());
        ui->lblBL_GitHash->setText(bootloader.remoteDeviceInfo.getDEV_gitHash());
        ui->lblBL_Name->setText(bootloader.remoteDeviceInfo.getDEV_name());
        ui->lblBL_Version->setText(bootloader.remoteDeviceInfo.getDEV_version());
        ui->lblBL_ID->setText(bootloader.remoteDeviceInfo.getDEV_deviceID());


        ui->lbl_rf_githash->setText(bootloader.remoteDeviceInfo.getFW_gitHash());
        ui->lbl_rf_gitdate->setText(bootloader.remoteDeviceInfo.getFW_gitDate_str());
        ui->lbl_rf_namehash->setText(bootloader.remoteDeviceInfo.getFW_nameCRC());
        ui->lbl_rf_name->setText(bootloader.remoteDeviceInfo.getFW_name());
        ui->lbl_rf_version->setText(bootloader.remoteDeviceInfo.getFW_version());
    }else{
        ui->plainTextEdit->clear();


        ui->lblMCU_devid->setText("");
        ui->lblMCU_avSize->setText("");
        ui->lblMCU_entryPoint->setText("");
        ui->lblMCU_fsize->setText("");
        ui->lblMCU_guid->setText("");
        ui->lblMCU_cryptoRequered->setText("");



        ui->lblBL_GitDate->setText("");
        ui->lblBL_GitHash->setText("");
        ui->lblBL_Name->setText("");
        ui->lblBL_Version->setText("");
        ui->lblBL_ID->setText("");


        ui->lbl_rf_githash->setText("");
        ui->lbl_rf_gitdate->setText("");
        ui->lbl_rf_namehash->setText("");
        ui->lbl_rf_name->setText("");
        ui->lbl_rf_version->setText("");
    }



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

void MainWindow::loadFile(QString fileName)
{
    bootloader.loadFile(fileName);
    recalcUIState();
    loadUIFromFile();
}






void MainWindow::setConnState(ConnectionState connState)
{
    this->connState = connState;
    if (connState == ConnectionState::Connecting){
        reconnectTimer->start(20);
    }else{
        reconnectTimer->stop();
    }
    recalcUIState();
}

void MainWindow::recalcUIState()
{
    if (bootloader.fileLoaded){
        ui->actionConnect->setEnabled(true);
        ui->actionConnect_2->setEnabled(true);
        switch (connState){
        case ConnectionState::Disconnected:
            bootloader.remoteDeviceInfo.unSet();
            loadUIDeviceInfo();
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
        bootloader.connectComPort(cmbPort->currentText());
    }else{
        bootloader.disconnectComPort();
    }
}

void MainWindow::on_actionGet_Chip_Info_triggered()
{
    on_actionGet_Info_triggered();
}

void MainWindow::on_actionGet_Info_triggered()
{
    bootloader.getDeviceInfo();
}

void MainWindow::on_actionRun_Application_triggered()
{
    on_actionRun_triggered();
}

void MainWindow::on_actionRun_triggered()
{
    bootloader.runApplication();
}


void MainWindow::on_actionTransfer_2_triggered()
{
    on_actionTransfer_triggered();
}
void MainWindow::on_actionTransfer_triggered()
{
    bootloader.sendfirmware();
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
    dialog.selectFile(bootloader.fileNameToSend);
    dialog.setDirectory(bootloader.fileNameToSend);
    dialog.setNameFilter(tr("Firmware Image (*.cfw)"));
    if (dialog.exec()){
        loadFile(dialog.selectedFiles()[0]);
        //loadUIFromSettings();
    }
}



void MainWindow::on_actionInfo_triggered()
{
    InfoDialog infodiag(this);
    infodiag.exec();
}

void MainWindow::on_actionOptions_triggered()
{
    OptionsDiag optionDiag(&bootloader.settings,this);
    optionDiag.exec();

}

void MainWindow::onProgress(int progress)
{
    ui->progressBar->setValue(progress);
    ui->progressBar->setVisible(true);

}

void MainWindow::onFinished()
{
    ui->progressBar->setVisible(false);
}

void MainWindow::onLog(QString text)
{
    log(text);
}

void MainWindow::onMCUGotDeviceInfo()
{
    loadUIDeviceInfo();
}

void MainWindow::onConnStateChanged(ConnectionState connState)
{
    setConnState(connState);
}

void MainWindow::on_actionClose_triggered()
{
    close();
}

void MainWindow::onReconnectTimer()
{
    bootloader.tryConnect();
}


