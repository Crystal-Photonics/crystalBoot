#ifndef AESKEYFILE_H
#define AESKEYFILE_H
#include <QString>
#include <QDateTime>

class AESKeyFile
{
public:
    QByteArray key;
    QDateTime creationDateTime;


    bool save(QString fileName);
    bool open(QString fileName);
    QString fileName;

    static bool isHex(QString hex_in);
};




#endif // AESKEYFILE_H
