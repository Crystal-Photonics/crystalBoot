#ifndef FIRMWARECREATESETTINGS_H
#define FIRMWARECREATESETTINGS_H

#include <QSettings>
#include <QString>
#include <QWidget>

class ImageCreatorSettings {

  public:
    enum Crypto { Plain, AES128 };
    explicit ImageCreatorSettings(QWidget *parent);

    void load(QString filename);
    void save();
    QString get_settings_file_name();

    QStringList header_files;
    QString encryption_key_file_name;
    QString sign_key_file_name;
    QString hex_file_name_application;
    QString hex_file_name_bootloader;
    QString sym_file_name_bootloader;
    QString target_file_name_application_image;
    QString target_file_name_bundle_image;

    QStringList header_files_absolute;
    QString encryption_key_file_name_absolute;
    QString sign_key_file_name_absolute;
    QString hex_file_name_application_absolute;
    QString target_file_name_application_image_absolute;

    QString hex_file_name_bootloader_absolute;
    QString sym_file_name_bootloader_absolute;
    QString target_file_name_bundle_image_absolute;

    QString key_word_githash;
    QString key_word_gitdate;
    QString key_word_version;
    QString key_word_name;
    bool create_bundle_image_wanted;

    QString get_root_path();
    bool is_existing_file_name();
    Crypto crypto;

  private:
    QWidget *parent;
    QString settings_file_name_m;
    bool is_existing_file_name_m;
    void set_absolute_file_paths();
    QString file_root_path_m;
};

#endif // FIRMWARECREATESETTINGS_H
