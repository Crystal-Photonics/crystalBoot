#ifndef FIRMWARECREATESETTINGS_H
#define FIRMWARECREATESETTINGS_H

#include <QSettings>
#include <QString>
#include <QWidget>

class ImageCreatorSettings {

    public:
    enum Crypt{Plain,AES128};
    explicit ImageCreatorSettings(QWidget *parent);

    void load(QString filename);
    void save();
    QString getFileName();

    QStringList headerFiles;
    QString encryptKeyFileName;
    QString signKeyFileName;
    QString hexFileName;
    QString targetFileName;

    QStringList headerFiles_abs;
    QString encryptKeyFileName_abs;
    QString signKeyFileName_abs;
    QString hexFileName_abs;
    QString targetFileName_abs;


    QString keyWord_githash;
    QString keyWord_gitdate;
    QString keyWord_version;
    QString keyWord_name;

    QString getRootPath();
    bool getIsExistingFileName();
    Crypt crypt;
private:
    QWidget *parent;
    QString settingsFileName;
    bool isExistingFileName;
    void setAbsoluteFilePaths();
    QString fileRootPath;

};

#endif // FIRMWARECREATESETTINGS_H
