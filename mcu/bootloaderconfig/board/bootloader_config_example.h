/*
 * bootloader_config.h
 *
 *  Created on: 25.10.2016
 *      Author: ak
 */

/*
 * deviceIDs.h is a catalog of our device portfolio and is something like this:
 *
 *
 *  typedef struct{
 * 		uint32_t githash;
 * 		uint32_t gitDate_unix;
 *
 *		uint32_t serialnumber;
 *		uint16_t deviceID;
 *		uint8_t guid[12];
 *		uint8_t boardRevision;
 *
 *		char name[12];
 *		char version[8];
 *  } device_descriptor_v1_t;
 *
 * 	typedef enum{deviceID_none=0,
 *		deviceID_CrystalBoot=1,
 *		..,
 *		..
 *	}deviceID_t;
 *
 *
 * */

#include "device_id_mapper.h"

#ifndef MCU_BOOTLOADERCONFIG_BOARD_BOOTLOADER_CONFIG_H_
#define MCU_BOOTLOADERCONFIG_BOARD_BOOTLOADER_CONFIG_H_

#define BOOTLOADER_DEVICE_ID deviceID_CrystalBoot

#define BOOTLOADER_BOOT_APP_USING_RESET 1

#define BOOTLOADER_WAITTIME_FOR_APP_BOOT_ms 1000
#define BOOTLOADER_WAITTIME_FOR_REBOOT_AFTER_BOOTLOADER_MODE_IDLE_ms 20 * 1000

#define BOOTLOADER_APP_STARTS_AFTER_POR_WITHOUT_WAITING 1

#define BOOTLOADER_ALLOW_PAIN_TEXT_COMMUNICATION 1
#define BOOTLOADER_WITH_DECRYPT_SUPPORT 0

#define BOOTLOADER_AES_REINITIALISATION_WAITTIME_s 1

#endif /* MCU_BOOTLOADERCONFIG_BOARD_BOOTLOADER_CONFIG_H_ */
