#include "firmwareimage.h"




FirmwareImage::FirmwareImage()
{
    clear();
}

void FirmwareImage::clear()
{
    firmware_githash = 0;
    firmware_gitdate = QDateTime();
    firmware_version = "";
    firmware_name = "";
}


