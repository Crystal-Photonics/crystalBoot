#ifndef CRYSTALSETTINGS_H
#define CRYSTALSETTINGS_H

#include <QObject>

class CrystalSettings
{

public:
    explicit CrystalSettings(QObject *parent);
    ~CrystalSettings();

    void load(QString filename);
    void save();
    QString getFileName();


    QString COMPortName;
    QString imageFile;
    bool callProcessAfterProgrammuing;
    QString callProcessPath;
    QString callProcessArguments;

    QString flashResultDocumentationPath;

private:
    QObject *parent;
    QString settingsFileName;
    bool isExistingFileName;
    QString fileRootPath;

};




#endif // CRYSTALSETTINGS_H
