#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include "imagecreatorsettings.h"
#include "firmwareencoder.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    switch (argc)
    {
    case 1:

        w.show();
       return a.exec();

    case 2:
    {
        QString creationSettings(argv[1]);
        if (QFileInfo(creationSettings).exists()){

            ImageCreatorSettings imageCreatorSettings(NULL);


            imageCreatorSettings.load(creationSettings);

            FirmwareEncoder fwImageEncode(imageCreatorSettings);
            bool result1,result2;
            result1 = fwImageEncode.loadFirmwareData();
            if (!result1){
                qDebug() << "error loading firmware data";
                exit(-1);
            }
            result2 = fwImageEncode.saveImage();
            if (!result1){
                qDebug() << "error saving firmware data";
                exit(-1);
            }
            if (result1 && result2){
                qDebug() << "created successfully";
            }

        }else{
            qDebug() << "cant find file" << creationSettings;
            exit(-1);
        }


    }
        break;
    default:
        qDebug() << "Too many arguments";
        exit(-1);
    }

    return 0;
}
