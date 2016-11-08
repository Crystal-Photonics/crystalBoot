#include "bootloaderrunner.h"

BootloaderRunnerThread::BootloaderRunnerThread(QObject *parent) : QObject(parent)
{
    thread = new QThread(this);

    bootloaderRunnerWorker = new BootloaderRunnerWorker(this);
    bootloaderRunnerWorker->moveToThread(thread);

    connect(thread, SIGNAL(started()), bootloaderRunnerWorker, SLOT(process()));
    connect(bootloaderRunnerWorker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(bootloaderRunnerWorker, SIGNAL(finished()), bootloaderRunnerWorker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}




BootloaderRunnerWorker::BootloaderRunnerWorker(QObject *parent) :  QObject(parent), bootloader("bootloader_settings.ini",parent)
{

}

void BootloaderRunnerWorker::process()
{
    qDebug()<<"process BootloaderRunnerWorker "<<QThread::currentThreadId();
    bootloader.loadFile("C:/Users/ark/entwicklung/eclipse_workspace/gammafindereval/mcu/buildarm_eval/gammafindereval_bootloader.cfw");
    bootloader.connectComPort();
}

