#include "crystalsettings.h"
#include <QFileInfo>
#include <QSettings>
#include <QDir>

CrystalSettings::~CrystalSettings()
{

}


bool fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

CrystalSettings::CrystalSettings(QObject *parent)
{
    this->parent = parent;
    isExistingFileName = false;
}

QString CrystalSettings::getFileName()
{
    return settingsFileName;
}


void CrystalSettings::load(QString filename)
{
    settingsFileName = filename;
    QFileInfo fi(settingsFileName);
    fileRootPath =fi.absoluteDir().absolutePath();

    isExistingFileName = fileExists(settingsFileName);
    QSettings settings(filename, QSettings::IniFormat, parent);




    COMPortName = settings.value("COMPortName","").toString();
    imageFile = settings.value("imageFile","").toString();
    callProcessAfterProgrammuing = settings.value("callProcessAfterProgrammuing",false).toBool();
    callProcessArguments = settings.value("callProcessArguments","%PATH_TO_JSON_REPORT%").toString();
    callProcessPath = settings.value("callProcessPath","").toString();

    flashResultDocumentationPath = settings.value("flashResultDocumentationPath","").toString();

}



void CrystalSettings::save()
{
    QSettings settings(settingsFileName, QSettings::IniFormat, parent);

    settings.setValue("COMPortName",COMPortName);
    settings.setValue("imageFile",imageFile);
    settings.setValue("callProcessAfterProgrammuing",callProcessAfterProgrammuing);
    settings.setValue("callProcessArguments",callProcessArguments);
    settings.setValue("callProcessPath",callProcessPath);
    settings.setValue("flashResultDocumentationPath",flashResultDocumentationPath);
    settings.sync();

    isExistingFileName = true;
}


