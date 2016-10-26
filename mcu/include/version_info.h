/*
 * version_info.h
 *
 *  Created on: 26.10.2016
 *      Author: ak
 */

#ifndef MCU_INCLUDE_VERSION_INFO_H_
#define MCU_INCLUDE_VERSION_INFO_H_

#include "bootloader_config.h"

//max 8 Characters
#define VERSION_INFO_NAME  "crysBoot"

//max 8 Characters
#define VERSION_INFO_VERSION  "0.1"

//Identifies device. (each device with crystalBoot firmware has this ID). Could be also a kind of hash of VERSION_INFO_NAME
#define VERSION_INFO_ID  BOOTLOADER_DEVICE_ID

#endif /* MCU_INCLUDE_VERSION_INFO_H_ */
