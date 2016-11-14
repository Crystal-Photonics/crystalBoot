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

#include "../../../../mcu/crystalBoot_config/board/bootloader_config.h"

