#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialworker.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateTemperature(float temperature);

private slots:
    void on_btnRefresh_clicked();
    void on_btnConnect_clicked();


    void on_btnSend_clicked();

private:
    Ui::MainWindow *ui;

    SerialThread* serialThread;

};

#endif // MAINWINDOW_H
