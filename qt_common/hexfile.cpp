#include "intelhexclass.h"
#include "hexfile.h"
#include <QStringList>
#include <QDebug>
#include <assert.h>
#include <fstream>

HexFile::HexFile() {
    clear();
}

void HexFile::clear() {
    binary_m.clear();
    start_address_m = 0;
}

bool HexFile::append_hex_file(QString file_name) {
    intelhex ihex;
    std::ifstream intelHexFile;
    QByteArray input_binary;
    uint32_t input_start_address;
    intelHexFile.open(file_name.toStdString(), std::ifstream::in);

    if (!intelHexFile.good()) {
        qDebug() << "Error: couldn't open " << file_name;
        return false;
    }
    intelHexFile >> ihex;
    ihex.begin();
    input_start_address = ihex.currentAddress();
    ihex.begin();
    uint8_t hexByte = 0;
    uint32_t oldAddress = ihex.currentAddress();
    while (ihex.getData(&hexByte)) {
        uint32_t newAddress = ihex.currentAddress(); // fill "blank address spaces" with 0
        uint32_t diffAddress = newAddress - oldAddress;
        for (uint32_t diff = 1; diff < diffAddress; diff++) {
            uint8_t val = 0;
            input_binary.append(val);
        }
        input_binary.append(hexByte);
        oldAddress = newAddress;
        ++ihex;
    }
    int numberWarnings = ihex.getNoWarnings();
    if (numberWarnings) {
        qDebug() << "reading hexfile number of warnings:" << numberWarnings;
        return false;
    }
    int numberErrors = ihex.getNoErrors();
    if (numberErrors) {
        qDebug() << "reading hexfile number of errors:" << numberErrors;
        return false;
    }
    append_binary(input_binary, input_start_address);
    return true;
}

void HexFile::append_binary(const QByteArray &indata, uint32_t start_address) {
    append_binary_(indata, start_address, false);
}

void HexFile::overwrite_binary(const QByteArray &indata, uint32_t start_address) {
    append_binary_(indata, start_address, true);
}

void HexFile::append_binary_(const QByteArray &indata, uint32_t start_address, bool overwrite) {
    if (binary_m.size() == 0) {
        binary_m = indata;
        start_address_m = start_address;
    } else {

        uint32_t actual_start_address = start_address_m;
        uint32_t actual_end_address = start_address_m + binary_m.size() - 1;

        uint32_t new_start_address = start_address;
        uint32_t new_end_address = start_address + indata.size() - 1;

        if (new_start_address > actual_end_address) {
            // append at the end
            for (uint32_t i = actual_end_address; i < new_start_address - 1; i++) {
                binary_m.append(char(0));
            }
            volatile uint32_t new_end_address_ = start_address_m + binary_m.size();
            (void)new_end_address_;
            binary_m.append(indata);
        } else if (new_end_address < actual_start_address) {
            QByteArray indata_padded(indata);
            // prepend
            for (uint32_t i = new_end_address; i < actual_start_address; i++) {
                indata_padded.append(char(0));
            }
            binary_m.prepend(indata_padded);
            start_address_m = start_address;
        } else if ((overwrite) && (new_start_address > actual_start_address) && (new_end_address < actual_end_address)) {

            uint32_t offset = start_address - start_address_m;
            for (uint32_t i = 0; i < (uint32_t)indata.size(); i++) {
                binary_m[i + offset] = indata[i];
            }
        } else {
            assert(0);
        }
    }
}

QString HexFile::get_checksum(QString line) {
    QString result;
    uint8_t sum = 0;
    assert(line[0] == ':');
    for (int i = 1; i < line.size(); i += 2) { // starting by one to skip the ":"
        QString byte_str = line.mid(i, 2);
        uint8_t byte = byte_str.toInt(nullptr, 16);
        sum += byte;
    }
    result = QString::asprintf("%02X", (-sum) & 0xFF);
    return result;
}

#define MAXHEXLINE 16
QStringList HexFile::dump_data_line(uint8_t byte_buffer[MAXHEXLINE], uint32_t buffer_length, uint32_t old_address, uint32_t actual_address) {
    /* it's time to dump the buffer to a line in the file */
    QStringList result;
    const QString TYPE_DATA = "00";

    if ((old_address >> 16) != (actual_address >> 16)) {
        result << get_extended_linear_address(actual_address);
    }

    uint16_t address_lower = actual_address & 0xFFFF;
    QString dataline = QString::asprintf(":%02X%04X", buffer_length, address_lower) + TYPE_DATA;
    for (uint32_t j = 0; j < buffer_length; j++) {
        dataline += QString::asprintf("%02X", byte_buffer[j]);
    }
    dataline += get_checksum(dataline);
    result << dataline;
    return result;
}

QString HexFile::get_extended_linear_address(uint32_t buffer_addr) {
    //:020000040800F2
    QString result;
    const QString TYPE_LINEAR_ADDRESS = "04";
    result = ":020000" + TYPE_LINEAR_ADDRESS + QString::asprintf("%04X", buffer_addr >> 16);
    result += get_checksum(result);
    return result;
}

QStringList HexFile::get_hex_file() {
    uint8_t byte_buffer[MAXHEXLINE];
    QStringList result;
    /* initial condition setup */
    uint32_t buffer_addr = start_address_m;
    uint8_t buffer_pos = 0;
    uint32_t buffer_addr_old = 0;

    for (int i = 0; i < binary_m.size(); i++) {
        uint8_t byte = (uint8_t)binary_m[i];
        byte_buffer[buffer_pos] = byte;
        buffer_pos++;

        if ((buffer_pos >= MAXHEXLINE)) {
            uint32_t buffer_length = buffer_pos;
            buffer_pos = 0;
            result << dump_data_line(byte_buffer, buffer_length, buffer_addr_old, buffer_addr);
            buffer_addr_old = buffer_addr;
            buffer_addr += buffer_length;
        }
    }
    if (buffer_pos > 0) {
        result << dump_data_line(byte_buffer, buffer_pos, buffer_addr_old, buffer_addr);
    }
    result << QString(":00000001FF\n");
    return result;
}

QByteArray HexFile::get_binary() {
    return binary_m;
}

uint32_t HexFile::get_start_address() {
    return start_address_m;
}
