#include "crystalsettings.h"
#include <QFileInfo>
#include <QSettings>
#include <QDir>

CrystalBootSettings::~CrystalBootSettings()
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

CrystalBootSettings::CrystalBootSettings(QObject *parent)
{
    this->parent = parent;
    isExistingFileName = false;
}

QString CrystalBootSettings::getFileName()
{
    return settingsFileName;
}


void CrystalBootSettings::load(QString filename)
{
    settingsFileName = filename;
    QFileInfo fi(settingsFileName);
    fileRootPath =fi.absoluteDir().absolutePath();

    isExistingFileName = fileExists(settingsFileName);
    QSettings settings(filename, QSettings::IniFormat, parent);




    COMPortName = settings.value("COMPortName","").toString();
    //imageFile = settings.value("imageFile","").toString();
    callProcessAfterProgrammuing = settings.value("callProcessAfterProgrammuing",false).toBool();
    callProcessArguments = settings.value("callProcessArguments","%PATH_TO_XML_REPORT%").toString();
    callProcessPath = settings.value("callProcessPath","").toString();

    flashResultDocumentationPath = settings.value("flashResultDocumentationPath","").toString();

}



void CrystalBootSettings::saveAs(QString filename)
{
    QSettings settings(filename, QSettings::IniFormat, parent);

    settings.setValue("COMPortName",COMPortName);
   // settings.setValue("imageFile",imageFile);
    settings.setValue("callProcessAfterProgrammuing",callProcessAfterProgrammuing);
    settings.setValue("callProcessArguments",callProcessArguments);
    settings.setValue("callProcessPath",callProcessPath);
    settings.setValue("flashResultDocumentationPath",flashResultDocumentationPath);
    settings.sync();


}

void CrystalBootSettings::save()
{
    saveAs(settingsFileName);
    isExistingFileName = true;
}


