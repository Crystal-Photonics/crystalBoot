#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>

bool fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

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


ImageCreatorSettings::ImageCreatorSettings(QWidget *parent)
{
    this->parent = parent;
    isExistingFileName = false;
}

void ImageCreatorSettings::setAbsoluteFilePaths()
{
    QDir dir(fileRootPath);
    encryptKeyFileName_abs = dir.absoluteFilePath(encryptKeyFileName);
    signKeyFileName_abs = dir.absoluteFilePath(signKeyFileName);
    hexFileName_abs = dir.absoluteFilePath(hexFileName);
    targetFileName_abs = dir.absoluteFilePath(targetFileName);
    //qDebug() << "hexfilename: "<< hexFileName_abs;
    headerFiles_abs.clear();
    for (int i = 0; i<headerFiles.count();i++){
        headerFiles_abs.append(dir.absoluteFilePath(headerFiles[i]));
    }
}

void ImageCreatorSettings::load(QString filename)
{
    settingsFileName = filename;
    QFileInfo fi(settingsFileName);
    fileRootPath =fi.absoluteDir().absolutePath();

    isExistingFileName = fileExists(settingsFileName);
    QSettings settings(filename, QSettings::IniFormat, parent);

    encryptKeyFileName = settings.value("encryptKeyFileName","").toString();
    signKeyFileName = settings.value("signKeyFileName","").toString();
    hexFileName = settings.value("hexFileName","").toString();
    targetFileName = settings.value("targetFileName","").toString();

    keyWord_githash = settings.value("keyWord_githash","GITHASH").toString();
    keyWord_gitdate = settings.value("keyWord_gitdate_unix","GITUNIX").toString();
    keyWord_version = settings.value("keyWord_version","VERSION_INFO_VERSION").toString();
    keyWord_name = settings.value("keyWord_name","VERSION_INFO_NAME").toString();
    settings.beginGroup("Version_Info_Header_Files");
    QStringList keys = settings.childKeys();
    headerFiles.clear();
    for (int i = 0; i<keys.count();i++){
        headerFiles.append(settings.value(keys[i],"").toString());
    }
    setAbsoluteFilePaths();
}



void ImageCreatorSettings::save()
{
    QSettings settings(settingsFileName, QSettings::IniFormat, parent);

    settings.setValue("encryptKeyFileName",encryptKeyFileName);
    settings.setValue("signKeyFileName",signKeyFileName);
    settings.setValue("hexFileName",hexFileName);
    settings.setValue("targetFileName",targetFileName);

    settings.setValue("keyWord_githash",keyWord_githash);
    settings.setValue("keyWord_gitdate_unix",keyWord_gitdate);
    settings.setValue("keyWord_version",keyWord_version);
    settings.setValue("keyWord_name",keyWord_name);

    settings.beginGroup("Version_Info_Header_Files");

    QStringList keys = settings.childKeys();
    for (int i = 0; i<keys.count();i++){
        settings.remove(keys[i]);
    }

    for (int i = 0; i<headerFiles.count();i++){
        settings.setValue(QString("file")+QString::number(i),headerFiles[i]);
    }
    settings.sync();
    setAbsoluteFilePaths();
    isExistingFileName = true;
}

QString ImageCreatorSettings::getFileName()
{
    return settingsFileName;
}

QString ImageCreatorSettings::getRootPath()
{
    return fileRootPath;
}

bool ImageCreatorSettings::getIsExistingFileName()
{
    return isExistingFileName;
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


FirmwareImage::FirmwareImage()
{
    firmware_githash = 0;
    firmware_gitdate = QDate();
    firmware_version = "";
    firmware_name = "";
}

QString FirmwareImage::parseADefine(QString key, QString line)
{
    (void)key;
    (void)line;
    return "";

}

bool FirmwareImage::isTheDefine(QString defineName, QString line)
{

    line = line.left(line.indexOf("\\\\"));
    if (line.toLower().contains("#define")){
        return false;
    }
    line = line.right(line.indexOf("#define")+QString("#define").length());
    if (!line[0].isSpace()){
        return false;
    }
    line = line.trimmed();
    if (!line.startsWith(defineName)){
        return false;
    }
    if (!line[defineName.length()+1].isSpace()){
        return false;
    }
    return true;
}

void FirmwareImage::parseHeaderFiles(ImageCreatorSettings &imageCreatorSettings)
{
    firmware_githash = 0;
    firmware_gitdate = QDate();
    firmware_version = "";
    firmware_name = "";


    for (int i = 0;i<imageCreatorSettings.headerFiles_abs.count();i++){
        QString filename = imageCreatorSettings.headerFiles_abs[i];
        //bool withiinComment=false;
        QFile file(filename);
        file.open(QIODevice::ReadOnly);
        if (!file.isOpen()){
            qDebug() << "cant open file " << imageCreatorSettings.headerFiles_abs[i];
        }else{
            QTextStream in(&file);
            while(!in.atEnd()){
                QString line = in.readLine();
                line = line.left(line.indexOf("\\\\"));
                if (line.contains(imageCreatorSettings.keyWord_gitdate)){

                }else if (line.contains(imageCreatorSettings.keyWord_githash)){

                }else if (line.contains(imageCreatorSettings.keyWord_name)){

                }else if (line.contains(imageCreatorSettings.keyWord_version)){
                    firmware_version = parseADefine(imageCreatorSettings.keyWord_version,line);
                }
            }
        }
    }
}



void MainWindow::on_btnPreview_clicked()
{
    FirmwareImage fwImage;
    fwImage.parseHeaderFiles(imageCreatorSettings);
}
