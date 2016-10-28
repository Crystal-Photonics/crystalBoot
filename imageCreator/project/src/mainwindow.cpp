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


