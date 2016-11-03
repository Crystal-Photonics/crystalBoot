#include "imagecreatorsettings.h"
#include <QDir>
#include <QFileInfo>

bool fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
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

    QString cryptStr=settings.value("encryption","plain").toString();
    if(cryptStr == "plain"){
        crypto = Crypto::Plain;
    }else if(cryptStr == "aes128"){
        crypto = Crypto::AES128;
    }


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

    if (crypto == Crypto::Plain){
        settings.setValue("encryption","plain");
    }else if(crypto == Crypto::AES128){
        settings.setValue("encryption","aes128");
    }else{
        settings.setValue("encryption","");
    }

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


