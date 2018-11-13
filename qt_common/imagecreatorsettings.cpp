#include "imagecreatorsettings.h"
#include <QDir>
#include <QFileInfo>

bool fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

ImageCreatorSettings::ImageCreatorSettings(QWidget *parent) {
    this->parent = parent;
    is_existing_file_name_m = false;
}

void ImageCreatorSettings::set_absolute_file_paths() {
    QDir dir(file_root_path_m);
    encryption_key_file_name_absolute = dir.absoluteFilePath(encryption_key_file_name);
    sign_key_file_name_absolute = dir.absoluteFilePath(sign_key_file_name);
    hex_file_name_application_absolute = dir.absoluteFilePath(hex_file_name_application);
    target_file_name_application_image_absolute = dir.absoluteFilePath(target_file_name_application_image);

    target_file_name_bundle_image_absolute = dir.absoluteFilePath(target_file_name_bundle_image);
    hex_file_name_bootloader_absolute = dir.absoluteFilePath(hex_file_name_bootloader);
    sym_file_name_bootloader_absolute = dir.absoluteFilePath(sym_file_name_bootloader);

    // qDebug() << "hexfilename: "<< hexFileName_abs;
    header_files_absolute.clear();
    for (int i = 0; i < header_files.count(); i++) {
        header_files_absolute.append(dir.absoluteFilePath(header_files[i]));
    }
}

void ImageCreatorSettings::load(QString filename) {
    settings_file_name_m = filename;
    QFileInfo fi(settings_file_name_m);
    file_root_path_m = fi.absoluteDir().absolutePath();

    is_existing_file_name_m = fileExists(settings_file_name_m);
    QSettings settings(filename, QSettings::IniFormat, parent);

    encryption_key_file_name = settings.value("encryptKeyFileName", "").toString();
    sign_key_file_name = settings.value("signKeyFileName", "").toString();
    hex_file_name_application = settings.value("hexFileName", "").toString();
    target_file_name_application_image = settings.value("targetFileName", "").toString();

    key_word_githash = settings.value("keyWord_githash", "GITHASH").toString();
    key_word_gitdate = settings.value("keyWord_gitdate_unix", "GITUNIX").toString();
    key_word_version = settings.value("keyWord_version", "VERSION_INFO_VERSION").toString();
    key_word_name = settings.value("keyWord_name", "VERSION_INFO_NAME").toString();

    target_file_name_bundle_image = settings.value("target_file_name_bundle_image", "").toString();
    hex_file_name_bootloader = settings.value("hex_file_name_bootloader", "").toString();
    sym_file_name_bootloader = settings.value("sym_file_name_bootloader", "").toString();
    create_bundle_image_wanted = settings.value("create_bundle_image_wanted", false).toBool();

    QString cryptStr = settings.value("encryption", "plain").toString();
    if (cryptStr == "plain") {
        crypto = Crypto::Plain;
    } else if (cryptStr == "aes128") {
        crypto = Crypto::AES128;
    }

    settings.beginGroup("Version_Info_Header_Files");
    QStringList keys = settings.childKeys();
    header_files.clear();
    for (int i = 0; i < keys.count(); i++) {
        header_files.append(settings.value(keys[i], "").toString());
    }
    set_absolute_file_paths();
}

void ImageCreatorSettings::save() {
    QSettings settings(settings_file_name_m, QSettings::IniFormat, parent);

    settings.setValue("encryptKeyFileName", encryption_key_file_name);
    settings.setValue("signKeyFileName", sign_key_file_name);
    settings.setValue("hexFileName", hex_file_name_application);
    settings.setValue("targetFileName", target_file_name_application_image);

    settings.setValue("keyWord_githash", key_word_githash);
    settings.setValue("keyWord_gitdate_unix", key_word_gitdate);
    settings.setValue("keyWord_version", key_word_version);
    settings.setValue("keyWord_name", key_word_name);

    settings.setValue("target_file_name_bundle_image", target_file_name_bundle_image);
    settings.setValue("hex_file_name_bootloader", hex_file_name_bootloader);
    settings.setValue("sym_file_name_bootloader", sym_file_name_bootloader);
    settings.setValue("create_bundle_image_wanted", create_bundle_image_wanted);

    if (crypto == Crypto::Plain) {
        settings.setValue("encryption", "plain");
    } else if (crypto == Crypto::AES128) {
        settings.setValue("encryption", "aes128");
    } else {
        settings.setValue("encryption", "");
    }

    settings.beginGroup("Version_Info_Header_Files");

    QStringList keys = settings.childKeys();
    for (int i = 0; i < keys.count(); i++) {
        settings.remove(keys[i]);
    }

    for (int i = 0; i < header_files.count(); i++) {
        settings.setValue(QString("file") + QString::number(i), header_files[i]);
    }
    settings.sync();
    set_absolute_file_paths();
    is_existing_file_name_m = true;
}

QString ImageCreatorSettings::get_settings_file_name() {
    return settings_file_name_m;
}

QString ImageCreatorSettings::get_root_path() {
    return file_root_path_m;
}

bool ImageCreatorSettings::is_existing_file_name() {
    return is_existing_file_name_m;
}
