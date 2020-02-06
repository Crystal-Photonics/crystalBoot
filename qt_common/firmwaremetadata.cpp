#include "firmwaremetadata.h"
#include "hexfile.h"
#include "intelhexclass.h"
#include "stm32_fimware_descriptor.h"
#include <QDebug>
#include <QDomElement>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <assert.h>
#include <iostream>
#include <sstream>

FirmwareMetaData::FirmwareMetaData() {}

bool FirmwareMetaData::parseTheDefine(QString defineName, DefineType defineType,
                                      QString line, QString &value) {
  if (defineType == DefineType::None) {
    return false;
  }

  line = line.left(line.indexOf("//"));
  QRegularExpression re("([^\\s]*)\\s*([^\\s]*)\\s*(.*)");
  QStringList elements = re.match(line).capturedTexts();

#if 1
  elements.removeFirst(); // remove full match
  if (elements.count() < 3) {
    return false;
  }

  if (elements[0].toLower() != QString("#define")) {
    return false;
  }
  if (elements[1] != defineName) {
    return false;
  }

  QString tmp = elements[2];
  tmp = tmp.left(tmp.indexOf("//"));
  tmp = tmp.trimmed();
  if (defineType == DefineType::String) {
    if ((tmp[0] == '"') && (tmp[tmp.length() - 1] == '"')) {
      value = tmp.mid(1, tmp.length() - 2);
    } else {
      return false;
    }
  } else if (defineType == DefineType::Number) {
    bool ok = false;
    uint val = 0;
    val = tmp.toUInt(&ok, 0);
    if (ok) {
      value = QString::number(val);
    } else {
      return false;
    }
  }
#else
  (void)defineName;
  (void)value;
#endif
  return true;
}

bool FirmwareMetaData::load_meta_data_from_source(
    ImageCreatorSettings &settings) {

  bool firmware_githash_found = false;
  bool firmware_gitdate_found = false;
  bool firmware_version_found = false;
  bool firmware_name_found = false;
  bool exit_loop_due_error = false;
  bool address_firmware_descriptor_in_bootloader_found = false;

  for (int i = 0; i < settings.header_files_absolute.count(); i++) {
    QString filename = settings.header_files_absolute[i];
    // bool withiinComment=false;
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    if (!file.isOpen()) {
      qDebug() << "cant open file " << settings.header_files_absolute[i];
      return false;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
      QString line = in.readLine();
      line = line.left(line.indexOf("\\\\"));
      QString value;
      if (parseTheDefine(settings.key_word_gitdate, DefineType::Number, line,
                         value)) {
        uint v = value.toUInt();
        firmware_gitdate_unix = v;
        firmware_gitdate_dt = QDateTime::fromTime_t(v);
        // qDebug() << "found firmware_gitdate";
        if (firmware_gitdate_found) {
          qDebug() << "gitdate found more than once.";
          exit_loop_due_error = true;
          break;
        }
        firmware_gitdate_found = true;

      } else if (parseTheDefine(settings.key_word_githash, DefineType::Number,
                                line, value)) {
        firmware_githash = value.toUInt();
        // qDebug() << "found firmware_githash";
        if (firmware_githash_found) {
          qDebug() << "githash found more than once.";
          exit_loop_due_error = true;
          break;
        }
        firmware_githash_found = true;
      } else if (parseTheDefine(settings.key_word_name, DefineType::String,
                                line, value)) {
        firmware_name = value;
        // qDebug() << "found firmware_name";
        if (firmware_name_found) {
          qDebug() << "firmware name found more than once.";
          exit_loop_due_error = true;
          break;
        }
        firmware_name_found = true;
      } else if (parseTheDefine(settings.key_word_version, DefineType::String,
                                line, value)) {
        firmware_version = value;
        // qDebug() << "found firmware_version";
        if (firmware_version_found) {
          qDebug() << "firmware version found more than once.";
          exit_loop_due_error = true;
          break;
        }
        firmware_version_found = true;
      }
    }
    if (exit_loop_due_error) {
      break;
    }
  }

  // QString target_file_name_application_image;
  // QString target_file_name_bundle_image;

  QFile sym_file(settings.sym_file_name_bootloader_absolute);
  sym_file.open(QIODevice::ReadOnly);
  if (!sym_file.isOpen()) {
    qDebug() << "cant open file " << settings.sym_file_name_bootloader_absolute;
    return false;
  }

  address_firmware_descriptor_in_bootloader = 0;
  QTextStream symfile_stream(&sym_file);
  while (!symfile_stream.atEnd()) {
    QString line = symfile_stream.readLine();
    if (line.contains("firmwareDescriptorBuffer")) {
      // 08004100 d firmwareDescriptorBuffer
      auto fields =
          line.split(QRegularExpression("[ \\t]"), QString::SkipEmptyParts);
      auto address_firmware_descriptor_in_bootloader_str = "0x" + fields[0];
      bool ok = false;
      address_firmware_descriptor_in_bootloader =
          address_firmware_descriptor_in_bootloader_str.toUInt(&ok, 0);
      address_firmware_descriptor_in_bootloader_found = ok;
      break;
    }
  }

  if (firmware_version_found && firmware_name_found && firmware_githash_found &&
      firmware_gitdate_found &&
      address_firmware_descriptor_in_bootloader_found) {
    qDebug() << "found all fields";
  }
  qDebug() << "firmware_githash" << firmware_githash;
  qDebug() << "firmware_gitdate_unix" << firmware_gitdate_unix;
  qDebug() << "firmware_version" << firmware_version;
  qDebug() << "firmware_name" << firmware_name;
  qDebug() << "address_firmware_descriptor_in_bootloader"
           << "0x" + QString::number(address_firmware_descriptor_in_bootloader,
                                     16);

  return true;
}

bool FirmwareMetaData::load_meta_data_from_xml(QDomElement &metaNodeElement) {
  if (metaNodeElement.isNull()) {
    return false;
  }

  bool ok = true;
  firmware_githash = metaNodeElement.attribute("githash", "").toUInt(&ok, 0);
  if (!ok) {
    qDebug() << "githash is not a number";
    return false;
  }
  firmware_gitdate_unix =
      metaNodeElement.attribute("gitdate", "").toUInt(&ok, 0);

  firmware_gitdate_dt = QDateTime::fromTime_t(firmware_gitdate_unix);
  if (!ok) {
    qDebug() << "gitdate is not a date/number";
    return false;
  }
  firmware_version = metaNodeElement.attribute("firmware_version", "");

  firmware_name = metaNodeElement.attribute("firmware_name", "");
  firmware_entryPoint =
      metaNodeElement.attribute("firmware_entrypoint", "").toUInt(&ok, 0);
  if (!ok) {
    qDebug() << "firmware_entrypoint is not a number";
    return false;
  }
  firmware_size = metaNodeElement.attribute("firmware_size", "").toUInt(&ok, 0);
  if (!ok) {
    qDebug() << "firmware_size is not a number";
    return false;
  }

  QString imageCreationDate_str =
      metaNodeElement.attribute("creation_date", "");
  imageCreationDate =
      QDateTime::fromString(imageCreationDate_str, "yyyy.MM.dd HH:mm");
  if (!imageCreationDate.isValid()) {
    qDebug() << "imageCreationDate is not a datetime";
    return false;
  }

  QString crpt = metaNodeElement.attribute("crypto", "").toLower();
  if (crpt == "plain") {
    crypto = Crypto::Plain;
  } else if (crpt == "aes") {
    crypto = Crypto::AES128;
  } else {
    qDebug() << "no crypto type found";
    return false;
  }
  return true;
}

void FirmwareMetaData::write_meta_data_to_xml(QXmlStreamWriter &xml) {

  xml.writeStartElement("meta");
  xml.writeAttribute("githash",
                     "0x" + QString::number(firmware_githash, 16).toUpper());
  xml.writeAttribute("gitdate", QString::number(firmware_gitdate_unix));
  xml.writeAttribute("firmware_version", firmware_version);
  xml.writeAttribute("firmware_name", firmware_name);
  xml.writeAttribute("firmware_entrypoint",
                     "0x" + QString::number(firmware_entryPoint, 16).toUpper());
  xml.writeAttribute("firmware_size", QString::number(firmware_size));
  xml.writeAttribute("creation_date",
                     imageCreationDate.toString("yyyy.MM.dd HH:mm"));
  if (crypto == Crypto::Plain) {
    xml.writeAttribute("crypto", "plain");
  } else if (crypto == Crypto::AES128) {
    xml.writeAttribute("crypto", "aes");
  }
  xml.writeEndElement();
}

#if 0
QStringList FirmwareMetaData::compile_firmware_descriptor_hex(const QByteArray &sha256) const {
    QByteArray firmware_descriptor_bin = compile_firmware_descriptor_binary(sha256);
    HexFile hex_file;
    hex_file.append_binary(firmware_descriptor_bin, address_firmware_descriptor_in_bootloader);
    return hex_file.get_hex_file();
}
#endif

#if 0
static void append_uint16_stm32(QByteArray &buffer, uint16_t value) {
    buffer.append((value >> 0) & 0xFF);
    buffer.append((value >> 8) & 0xFF);
}

static void append_uint32_stm32(QByteArray &buffer, uint32_t value) {
    buffer.append((value >> 0) & 0xFF);
    buffer.append((value >> 8) & 0xFF);
    buffer.append((value >> 16) & 0xFF);
    buffer.append((value >> 24) & 0xFF);
}

static void append_bool_stm32(QByteArray &buffer, bool value) {
    if (value) {
        buffer.append(1);
    } else {
        buffer.append(0);
    }
}

static void append_Crypto_stm32(QByteArray &buffer, crystalBoolCrypto_t value) {
    buffer.append(value);
}

#endif

static uint16_t crc16_buffer(char *data_p, uint16_t length) {
  uint8_t x;
  uint16_t crc = 0xFFFF;

  while (length--) {
    x = crc >> 8 ^ *data_p++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^
          ((uint16_t)x);
  }
  return crc;
}

QByteArray FirmwareMetaData::compile_firmware_descriptor_binary(
    const QByteArray &sha256) const {
  QByteArray result;

  assert(sha256.size() == CHECKSUM_SIZE);
  stm32_firmware_meta_t firmware_meta = {};

  firmware_meta.d.checksumVerified = 0;
  firmware_meta.d.checksumVerifiedByCrypto = 0;
  if (crypto == Crypto::Plain) {
    firmware_meta.d.usedCrypto = crystalBoolCrypto_Plain;
  } else if (crypto == Crypto::AES128) {
    firmware_meta.d.usedCrypto = crystalBoolCrypto_AES;
  }
  for (int i = 0; i < CHECKSUM_SIZE; i++) {
    firmware_meta.d.sha256[i] = sha256[i];
  }
  firmware_meta.d.firmwareDescriptor.githash = firmware_githash;
  firmware_meta.d.firmwareDescriptor.gitDate_unix = firmware_gitdate_unix;
  firmware_meta.d.firmwareDescriptor.nameCRC16 = getNameCRC16();
  firmware_meta.d.firmwareDescriptor.size = firmware_size;
  firmware_meta.d.firmwareDescriptor.entryPoint = firmware_entryPoint;

  int buf_size = sizeof(firmware_meta.d.firmwareDescriptor.name) - 1;
  if (firmware_name.size() < buf_size) {
    buf_size = firmware_name.size();
  }
  for (int i = 0; i < buf_size; i++) {
    firmware_meta.d.firmwareDescriptor.name[i] = firmware_name[i].toLatin1();
  }

  buf_size = sizeof(firmware_meta.d.firmwareDescriptor.version) - 1;
  if (firmware_version.size() < buf_size) {
    buf_size = firmware_version.size();
  }
  for (int i = 0; i < buf_size; i++) {
    firmware_meta.d.firmwareDescriptor.version[i] =
        firmware_version[i].toLatin1();
  }
  result.append((char *)&firmware_meta, sizeof(firmware_meta));

  // result[0x2c] = 1;
  result = result.leftJustified(0x50, 0);
  uint16_t crc16 =
      crc16_buffer(result.data() + 4, sizeof(stm32_firmware_meta_data_t));
  result[0] = crc16 & 0xFF;
  result[1] = (crc16 >> 8) & 0xFF;
  result = result.leftJustified(FIRMWARE_DESCRIPTION_BUFFER_SIZE, 0);

  return result;
}

QString FirmwareMetaData::getNameShort(int len) const {
  return firmware_name.left(len);
}

void FirmwareMetaData::clear() {
  firmware_githash = 0;
  firmware_gitdate_unix = 0;
  firmware_gitdate_dt = QDateTime();
  firmware_version = "";
  firmware_name = "";
  firmware_entryPoint = 0;
  firmware_size = 0;
  crypto = Crypto::Plain;
}

bool FirmwareMetaData::is_valid() {
  if (!firmware_githash) {
    qDebug() << "firmware_githash is zero";
    return false;
  }

  if (!firmware_gitdate_unix) {
    qDebug() << "firmware_gitdate_unix is zero";
    return false;
  }

  if (!firmware_entryPoint) {
    qDebug() << "firmware_entryPoint is zero";
    return false;
  }
  if (!firmware_size) {
    qDebug() << "firmware_size is zero";
    return false;
  }
  return true;
}

uint16_t FirmwareMetaData::getNameCRC16() const {
  return qChecksum(firmware_name.toStdString().c_str(), firmware_name.length());
}

QString FirmwareMetaData::getGitHash_str() const {
  return "0x" + QString::number(firmware_githash, 16).toUpper();
}
