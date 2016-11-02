#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <QXmlStreamWriter>

#include "firmwareimage.h"
#include "firmwareencoder.h"
#include <random>
#include <QProcess>
#include <QMessageBox>
#include "aeskeyfile.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    imageCreatorSettings(parent),
    settings("crystalBootImager.ini", QSettings::IniFormat, parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    imageCreatorSettings.load(settings.value("LastSettingsFile","").toString());
    loadUIFromSettings();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveUIToSettings();
    imageCreatorSettings.save();
    settings.setValue("LastSettingsFile",imageCreatorSettings.getFileName());
    settings.sync();
    event->accept();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadUIFromSettings()
{
    ui->lblEntryPoint->setText("");
    ui->lblGitDate->setText("");
    ui->lblGitHash->setText("");
    ui->lblName->setText("");
    ui->lblVersion->setText("");
    ui->lblSize->setText("");

    ui->edt_encryptionKey->setText(imageCreatorSettings.encryptKeyFileName);
    ui->edt_hexFile->setText(imageCreatorSettings.hexFileName);
    ui->edt_signPrivateKey->setText(imageCreatorSettings.signKeyFileName);
    ui->edt_targetFile->setText(imageCreatorSettings.targetFileName);

    ui->edt_githash->setText(imageCreatorSettings.keyWord_githash);
    ui->edt_gitdate->setText(imageCreatorSettings.keyWord_gitdate);
    ui->edt_version->setText(imageCreatorSettings.keyWord_version);
    ui->edt_name->setText(imageCreatorSettings.keyWord_name);

    ui->edt_versionInfoHeader->clear();
    for(int i=0;i< imageCreatorSettings.headerFiles.count();i++){
        ui->edt_versionInfoHeader->appendPlainText(imageCreatorSettings.headerFiles[i]);
    }

    if (imageCreatorSettings.getIsExistingFileName()){
        ui->btnSave->setEnabled(true);
        ui->btnSaveAs->setEnabled(true);
    }else{
        ui->btnSave->setEnabled(false);
        ui->btnSaveAs->setEnabled(true);
    }
    QFileInfo fileInfo(imageCreatorSettings.getFileName());

    ui->edtRelative->setText(imageCreatorSettings.getRootPath());
    ui->lblFileName->setText(fileInfo.fileName());
    if (imageCreatorSettings.crypt == ImageCreatorSettings::Crypt::AES128){
        ui->rbtn_crypt_aes->setChecked(true);
        ui->rbtn_crypt_plain->setChecked(false);
    }else{
        ui->rbtn_crypt_aes->setChecked(false);
        ui->rbtn_crypt_plain->setChecked(true);
    }
}

void MainWindow::saveUIToSettings()
{
    imageCreatorSettings.encryptKeyFileName = ui->edt_encryptionKey->text();
    imageCreatorSettings.hexFileName = ui->edt_hexFile->text();
    imageCreatorSettings.signKeyFileName = ui->edt_signPrivateKey->text();
    imageCreatorSettings.targetFileName = ui->edt_targetFile->text();

    imageCreatorSettings.keyWord_githash = ui->edt_githash->text();
    imageCreatorSettings.keyWord_gitdate = ui->edt_gitdate->text();
    imageCreatorSettings.keyWord_version = ui->edt_version->text();
    imageCreatorSettings.keyWord_name = ui->edt_name->text();
    imageCreatorSettings.headerFiles =  ui->edt_versionInfoHeader->toPlainText().split('\n', QString::SkipEmptyParts);
    if (ui->rbtn_crypt_aes->isChecked()){
        imageCreatorSettings.crypt = ImageCreatorSettings::Crypt::AES128;
    }else if (ui->rbtn_crypt_plain->isChecked()){
        imageCreatorSettings.crypt = ImageCreatorSettings::Crypt::Plain;
    }
}




void MainWindow::on_btnLoad_clicked()
{
    QString fn = imageCreatorSettings.getFileName();
    if (fn == ""){
        fn = "bootloader_image_creator_settings.conf";
    }
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.selectFile(fn);
    dialog.setDirectory(fn);
    dialog.setNameFilter(tr("Setting Files (*.conf)"));
    if (dialog.exec()){
        imageCreatorSettings.load(dialog.selectedFiles()[0]);
        loadUIFromSettings();
    }
}

void MainWindow::on_btnSave_clicked()
{
    if (imageCreatorSettings.getIsExistingFileName()){
        saveUIToSettings();
        imageCreatorSettings.save();
    }else{
        on_btnSaveAs_clicked();
    }
}

void MainWindow::on_btnSaveAs_clicked()
{

    QString fn = imageCreatorSettings.getFileName();
    if (fn == ""){
        fn = "bootloader_image_creator_settings.conf";
    }
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.selectFile(fn);
    dialog.setDirectory(fn);
    dialog.setNameFilter(tr("Setting Files (*.conf)"));
    if (dialog.exec()){
        imageCreatorSettings.load(dialog.selectedFiles()[0]);
        saveUIToSettings();
        imageCreatorSettings.save();
    }
}

void MainWindow::loadUIFromFirmwaredata(FirmwareImage fwImage)
{
    ui->lblGitHash->setText("0x"+QString::number(fwImage.firmware_githash,16).toUpper());
    ui->lblName->setText(fwImage.firmware_name);
    ui->lblVersion->setText(fwImage.firmware_version);
    ui->lblGitDate->setText(fwImage.firmware_gitdate_dt.toString("yyyy.mm.dd HH:MM"));
    ui->lblEntryPoint->setText("0x"+QString::number(fwImage.firmware_entryPoint,16).toUpper());
    ui->lblSize->setText(QString::number(fwImage.firmware_size).toUpper());
}

void MainWindow::on_btnPreview_clicked()
{
    bool result = false;
    FirmwareEncoder fwImageEncode(imageCreatorSettings);
    result = fwImageEncode.loadFirmwareData();
    if (!result){
        ui->statusBar->showMessage("error loading firmware data");
    }
    loadUIFromFirmwaredata(fwImageEncode.fwImage);
    if (result){
        ui->statusBar->showMessage("");
    }
}

void MainWindow::on_btnCreateImage_clicked()
{
    FirmwareEncoder fwImageEncode(imageCreatorSettings);
    bool result1,result2;
    result1 = fwImageEncode.loadFirmwareData();
    if (!result1){
        ui->statusBar->showMessage("error loading firmware data");
    }
    loadUIFromFirmwaredata(fwImageEncode.fwImage);
    result2 = fwImageEncode.saveImage();
    if (!result1){
        ui->statusBar->showMessage("error saving firmware data");
    }
    if (result1 && result2){
        ui->statusBar->showMessage("");
    }
}



void MainWindow::on_actionCreate_AES_128_key_triggered()
{
#if 1
    const uint32_t AES128_KEY_LENGTH = 128/8;
    std::random_device rd;
    QByteArray key;
    std::uniform_int_distribution<int> dist(0, 255);
    double entropy = rd.entropy();
    bool error = false;
    QString ErrorStr;

    if ((entropy == 0.0 )|| (entropy == 32.0 )){
        QString randomPath = QDir::currentPath()+QDir::separator()+"random.exe";

        QMessageBox msgBox;

        msgBox.setText("not enough entropy: entrop="+QString::number(entropy)+"");
        msgBox.setInformativeText("Do you want to call "+randomPath+" for getting a random number? This is quite a security problem. Please be sure what you do.");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Abort);
        msgBox.setDefaultButton(QMessageBox::Yes);

        int ret = msgBox.exec();
        if (ret == QMessageBox::Abort){
            error = true;
            ErrorStr = "Aborted";
        }

        QProcess myProcess(this);
        QString randomOut;

        if (!QFileInfo(randomPath).exists()){
            error = true;
        }
        if (!error){
            qDebug() << "trying to call "+randomPath;
            QString program = randomPath;
            QStringList arguments;
            //arguments << "-style" << "fusion";


            myProcess.start(program, arguments);
            if (!myProcess.waitForStarted()){
                error = true ;
                ErrorStr = "starting random.exe failed";

            }
        }
        if (!error){
            if (!myProcess.waitForFinished()){//3s timeout
                error = true ;
                ErrorStr =  "running random.exe failed";

            }
        }
        if (!error){
            randomOut = myProcess.readAllStandardOutput();
            randomOut = randomOut.toUpper();
            if (!AESKeyFile::isHex(randomOut)){
                error = true ;
                ErrorStr = "not valid random output";

            }
        }

        if (!error){
            key = QByteArray::fromHex(randomOut.toUtf8());
            if (key.length() != 16){
                error = true ;
                ErrorStr = "wrong random length";

            }
        }
        if(error){
            qDebug() << ErrorStr;
        }
    }else{

        for (uint32_t n = 0; n < AES128_KEY_LENGTH; n++) {
            key.append(dist(rd));
        }
    }

    if (!error){
        //QString fn = "";
        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setNameFilter(tr("Firmware Image Encryption Key(*.cfk)"));
        if (dialog.exec()){
            QString fileName = dialog.selectedFiles()[0];
            QString suffix = QFileInfo(fileName).completeSuffix();
            if (suffix != "cfk"){
                fileName.append("cfk");
            }
            AESKeyFile aeskeyFile;
            aeskeyFile.key = key;
            aeskeyFile.creationDateTime = QDateTime::currentDateTime();
            aeskeyFile.save(fileName);
            ui->edt_encryptionKey->setText(aeskeyFile.fileName);
        }
    }else{
        QMessageBox msgBox;

        msgBox.setText("Error happened:");
        msgBox.setInformativeText(ErrorStr);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }

    qDebug() << key.toHex();
#endif
}
