#ifndef CRYSTALSETTINGS_H
#define CRYSTALSETTINGS_H

#include <QObject>

class CrystalBootSettings
{

public:
    explicit CrystalBootSettings(QObject *parent);
    ~CrystalBootSettings();

    void load(QString filename);
    void save();
    void saveAs(QString filename);

    QString getFileName();


    QString COMPortName;
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
