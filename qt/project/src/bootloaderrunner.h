#ifndef BOOTLOADERRUNNER_H
#define BOOTLOADERRUNNER_H

#include <QObject>
#include <QTimer>
#include "bootloader.h"

class BootloaderRunnerThread;

class BootloaderRunnerWorker : public QObject
{
    Q_OBJECT
public:
    explicit BootloaderRunnerWorker(QObject *parent = 0);

    Bootloader bootloader;
public slots:
    void process();
signals:
    void finished();

};

class BootloaderRunnerThread : public QObject
{
    Q_OBJECT
public:
    explicit BootloaderRunnerThread(QObject *parent = 0);
    QThread* thread;
    BootloaderRunnerWorker * bootloaderRunnerWorker;


};

#endif // BOOTLOADERRUNNER_H
