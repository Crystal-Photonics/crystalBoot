#ifndef AESKEYFILE_H
#define AESKEYFILE_H
#include <QDateTime>
#include <QString>

class AESKeyFile {
  public:
    QByteArray key;
    QDateTime creationDateTime;
    QString fileName;

    bool save(QString fileName);
    bool open(QString fileName);
    bool isValid();

    // void fillBuffer(uint8_t outbuffer[16]);

    static bool isHex(QString hex_in);
};

#endif // AESKEYFILE_H
