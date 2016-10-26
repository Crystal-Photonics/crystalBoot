/*
 * bootloader_config.h
 *
 *  Created on: 25.10.2016
 *      Author: ak
 */



/*
 * deviceIDs.h is a catalog of our device portfolio and is something like this:
 *
 * 	typedef enum{deviceID_none=0,
 *		deviceID_CrystalBoot=1,
 *		..,
 *		..
 *	}deviceID_t;
 *
 *
 * */

#include "../../../_deviceIDs/include/deviceIDs.h"



#ifndef MCU_BOOTLOADERCONFIG_BOARD_BOOTLOADER_CONFIG_H_
#define MCU_BOOTLOADERCONFIG_BOARD_BOOTLOADER_CONFIG_H_

#define BOOTLOADER_BOOT_APP_USING_RESET 1

#define BOORLOADER_WAITTIME_FOR_APP_BOOT_ms 1000

#define BOOTLOADER_ALLOW_PAIN_TEXT_COMMUNICATION 0
#define BOOTLOADER_WITH_DECRYPT_SUPPORT 1


#define BOOTLOADER_DEVICE_ID deviceID_CrystalBoot



#endif /* MCU_BOOTLOADERCONFIG_BOARD_BOOTLOADER_CONFIG_H_ */
