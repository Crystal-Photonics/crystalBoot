#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QSettings>
#include <QCloseEvent>
#include <QDate>
#include "imagecreatorsettings.h"

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
    void loadUIFromSettings();
    void saveUIToSettings();

private slots:
    void on_btnSave_clicked();
    void on_btnLoad_clicked();
    void on_btnSaveAs_clicked();

    void on_btnPreview_clicked();

    void on_btnCreateImage_clicked();

private:
    ImageCreatorSettings imageCreatorSettings;
    QSettings settings;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
