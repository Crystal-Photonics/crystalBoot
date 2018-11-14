#ifndef HEXFILE_H
#define HEXFILE_H

#include <QString>
#include <QByteArray>
#include <stdint.h>

class HexFile {
  public:
    HexFile();
    void clear();
    void append_binary(const QByteArray &indata, uint32_t start_address);
    void overwrite_binary(const QByteArray &indata, uint32_t start_address);
    bool append_hex_file(QString file_name);
    QStringList get_hex_file();
    QByteArray get_binary();
    uint32_t get_start_address();

  private:
    QString get_checksum(QString line);
    QStringList dump_data_line(uint8_t byte_buffer[], uint32_t buffer_length, uint32_t old_address, uint32_t actual_address);
    QString get_extended_linear_address(uint32_t buffer_addr);
    // void append_binary(QByteArray indata, uint32_t start_address);

    QByteArray binary_m;
    uint32_t start_address_m;
    void append_binary_(const QByteArray &indata, uint32_t start_address, bool overwrite);
};

#endif // HEXFILE_H
