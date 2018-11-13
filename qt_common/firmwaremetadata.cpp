#include "firmwaremetadata.h"
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QDomElement>

FirmwareMetaData::FirmwareMetaData() {
}

bool FirmwareMetaData::parseTheDefine(QString defineName, DefineType defineType, QString line, QString &value) {
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
        int val = 0;
        val = tmp.toInt(&ok, 0);
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

bool FirmwareMetaData::load_meta_data_from_source(ImageCreatorSettings &settings) {

    QString sym_file_name_bootloader;
    QString target_file_name_application_image;
    QString target_file_name_bundle_image;

    bool firmware_githash_found = false;
    bool firmware_gitdate_found = false;
    bool firmware_version_found = false;
    bool firmware_name_found = false;
    bool exit_loop_due_error = false;
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
            if (parseTheDefine(settings.key_word_gitdate, DefineType::Number, line, value)) {
                int v = value.toInt();
                firmware_gitdate_unix = v;
                firmware_gitdate_dt = QDateTime::fromTime_t(v);
                // qDebug() << "found firmware_gitdate";
                if (firmware_gitdate_found) {
                    qDebug() << "gitdate found more than once.";
                    exit_loop_due_error = true;
                    break;
                }
                firmware_gitdate_found = true;

            } else if (parseTheDefine(settings.key_word_githash, DefineType::Number, line, value)) {
                firmware_githash = value.toInt();
                // qDebug() << "found firmware_githash";
                if (firmware_githash_found) {
                    qDebug() << "githash found more than once.";
                    exit_loop_due_error = true;
                    break;
                }
                firmware_githash_found = true;
            } else if (parseTheDefine(settings.key_word_name, DefineType::String, line, value)) {
                firmware_name = value;
                // qDebug() << "found firmware_name";
                if (firmware_name_found) {
                    qDebug() << "firmware name found more than once.";
                    exit_loop_due_error = true;
                    break;
                }
                firmware_name_found = true;
            } else if (parseTheDefine(settings.key_word_version, DefineType::String, line, value)) {
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
    if (firmware_version_found && firmware_name_found && firmware_githash_found && firmware_gitdate_found) {
        qDebug() << "found all fields";
    }
    qDebug() << "firmware_githash" << firmware_githash;
    qDebug() << "firmware_gitdate_unix" << firmware_gitdate_unix;
    qDebug() << "firmware_version" << firmware_version;
    qDebug() << "firmware_name" << firmware_name;

    return true;
}

bool FirmwareMetaData::load_meta_data_from_xml(QDomElement &metaNodeElement) {
    if (metaNodeElement.isNull()) {
        return false;
    }

    bool ok = true;
    firmware_githash = metaNodeElement.attribute("githash", "").toInt(&ok, 0);
    if (!ok) {
        qDebug() << "githash is not a number";
        return false;
    }
    firmware_gitdate_unix = metaNodeElement.attribute("gitdate", "").toInt(&ok, 0);

    firmware_gitdate_dt = QDateTime::fromTime_t(firmware_gitdate_unix);
    if (!ok) {
        qDebug() << "gitdate is not a date/number";
        return false;
    }
    firmware_version = metaNodeElement.attribute("firmware_version", "");

    firmware_name = metaNodeElement.attribute("firmware_name", "");
    firmware_entryPoint = metaNodeElement.attribute("firmware_entrypoint", "").toInt(&ok, 0);
    if (!ok) {
        qDebug() << "firmware_entrypoint is not a number";
        return false;
    }
    firmware_size = metaNodeElement.attribute("firmware_size", "").toInt(&ok, 0);
    if (!ok) {
        qDebug() << "firmware_size is not a number";
        return false;
    }

    QString imageCreationDate_str = metaNodeElement.attribute("creation_date", "");
    imageCreationDate = QDateTime::fromString(imageCreationDate_str, "yyyy.MM.dd HH:mm");
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
    xml.writeAttribute("githash", "0x" + QString::number(firmware_githash, 16).toUpper());
    xml.writeAttribute("gitdate", QString::number(firmware_gitdate_unix));
    xml.writeAttribute("firmware_version", firmware_version);
    xml.writeAttribute("firmware_name", firmware_name);
    xml.writeAttribute("firmware_entrypoint", "0x" + QString::number(firmware_entryPoint, 16).toUpper());
    xml.writeAttribute("firmware_size", QString::number(firmware_size));
    xml.writeAttribute("creation_date", imageCreationDate.toString("yyyy.MM.dd HH:mm"));
    if (crypto == Crypto::Plain) {
        xml.writeAttribute("crypto", "plain");
    } else if (crypto == Crypto::AES128) {
        xml.writeAttribute("crypto", "aes");
    }
    xml.writeEndElement();
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
