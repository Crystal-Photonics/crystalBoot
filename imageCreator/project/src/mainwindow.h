#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QCloseEvent>
#include <QDate>

class ImageCreatorSettings {
    public:

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
private:
    QWidget *parent;
    QString settingsFileName;
    bool isExistingFileName;
    void setAbsoluteFilePaths();
    QString fileRootPath;

};

namespace Ui {
class MainWindow;
}

class FirmwareImage{
public:
    explicit FirmwareImage();
    void parseHeaderFiles(ImageCreatorSettings &imageCreatorSettings);

    uint32_t firmware_githash;
    QDate firmware_gitdate;
    QString firmware_version;
    QString firmware_name;
private:
    QString parseADefine(QString key,QString line);
    bool isTheDefine(QString defineName,QString line);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
    void loadUIFromSettings();
    void saveUIToSettings();


    FirmwareImage firmwareImage;

private slots:
    void on_btnSave_clicked();
    void on_btnLoad_clicked();
    void on_btnSaveAs_clicked();

    void on_btnPreview_clicked();

private:
    ImageCreatorSettings imageCreatorSettings;
    QSettings settings;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
