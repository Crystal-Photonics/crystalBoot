#include "mainwindow.h"
#include <QApplication>

#include <thread>
#include <iostream>
#include <chrono>
#include <QStringList>
#include <set>
#include <QCommandLineParser>
#include <QElapsedTimer>
#include <QFileInfo>
#include "bootloader.h"
#include "vc.h"

enum class CommandlineAction { info, transfer, run, eraseEEPROM };
// enum class CommandlineAction{info, transfer, run };

void call_from_thread(int argc, char *argv[]) {

    qDebug() << "crystalBoot";
    qDebug() << QString("Git Hash: ") + QString::number(GITHASH, 16).toUpper();
    qDebug() << QString("Git Date: ") + GITDATE;

    QStringList argslist;
    std::set<CommandlineAction> cmdAction;

    QCommandLineParser parser;
    QCommandLineOption cmdInfo("info");
    QCommandLineOption cmdTransfer("transfer");
    QCommandLineOption cmdRun("run");
    QCommandLineOption cmdEraseEEPROM("eraseEEPROM");
    parser.addOption(cmdInfo);
    parser.addOption(cmdTransfer);
    parser.addOption(cmdRun);
    parser.addOption(cmdEraseEEPROM);

    QCommandLineOption cmdTimoutOption(QStringList() << "t"
                                                     << "timout",
                                       "timout in seconds", "2");
    QCommandLineOption cmdImageOption(QStringList() << "i"
                                                    << "image",
                                      "path to image for beeing transfered", "image.cfw");
    QCommandLineOption cmdSettingsOption(QStringList() << "s"
                                                       << "settings",
                                         "path to settingsfile", "settings.ini");
    parser.addOption(cmdTimoutOption);
    parser.addOption(cmdImageOption);
    parser.addOption(cmdSettingsOption);

    for (int i = 0; i < argc; i++) {
        argslist.append(QString(argv[i]));
    }

    //  qDebug() <<"arglist"<< argslist;
    parser.process(argslist);

    if (parser.isSet(cmdInfo)) {
        cmdAction.insert(CommandlineAction::info);
    }
    if (parser.isSet(cmdRun)) {
        cmdAction.insert(CommandlineAction::run);
    }
    if (parser.isSet(cmdTransfer)) {
        cmdAction.erase(CommandlineAction::info);
        cmdAction.insert(CommandlineAction::transfer);
    }
    if (parser.isSet(cmdEraseEEPROM)) {
        cmdAction.insert(CommandlineAction::eraseEEPROM);
    }
    // qDebug() << argslist;

    QString timeout_str = parser.value(cmdTimoutOption);
    int timout_s = timeout_str.toInt();
    if (timout_s == 0) {
        timout_s = 2;
    }
    QString image_str = parser.value(cmdImageOption);
    QString settings_str = parser.value(cmdSettingsOption);

    // just info
    // just run

    // info transfer
    // info transfer run
    // timeout

    if (!QFileInfo(settings_str).exists()) {
        qDebug() << "couldnt load settings file" << settings_str;
        QApplication::exit(1);
        return;
    }

    Bootloader bl(settings_str);

    bl.connectComPort();
    QElapsedTimer timeoutTimer;
    timeoutTimer.start();
    while (bl.getConnectionState() != ConnectionState::Connected) {
        bl.tryConnect();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        if (timeoutTimer.hasExpired(timout_s * 1000)) {
            qDebug() << "timout while connect.";
            QApplication::exit(1);
            return;
        }
    }
    if (cmdAction.count(CommandlineAction::info)) {
        FlashResultDocumentation flashResultDocumentation;
        flashResultDocumentation.setNewFirmwareDescriptor(bl.fwImage);
        RPC_RESULT result = bl.getDeviceInfo();
        flashResultDocumentation.addActionResult("GetInfo", result);
        flashResultDocumentation.setRemoteDeviceInfo(bl.remoteDeviceInfo);
        flashResultDocumentation.save(bl.settings);
        flashResultDocumentation.print();
    }

    if (cmdAction.count(CommandlineAction::transfer)) {
        bl.loadFile(image_str);
        if (bl.fileLoaded) {
            bl.sendfirmware();
        } else {
            qDebug() << "couldnt load firmware image" << image_str;
            QApplication::exit(1);
            return;
        }
    }

    if (cmdAction.count(CommandlineAction::eraseEEPROM)) {
        bl.eraseEEPROM();
    }
    if (cmdAction.count(CommandlineAction::run)) {
        bl.runApplication();
    }
    QApplication::exit(0);
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setOrganizationName("CPG");
    a.setApplicationName("crystalBoot");
    int result = 0;
    if (argc > 1) {

        std::thread t1(call_from_thread, argc, argv);
        result = a.exec();
        t1.join();
    } else {
        MainWindow w;
        w.show();
        result = a.exec();
    }
    return result;
}
