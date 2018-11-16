/*
 * bootloader_app_data_exchange.c
 *
 *  Created on: 16.11.2018
 *      Author: ak
 */

#include <stdint.h>
#include <stdbool.h>
#include "port_bootloader_app_data_exchange.h"
#include "vc.h"

/*
 * There are changes of the linkerscript which are necessary for the application image to use the same shared data-exchange-memory:
 * Example:
 *
 * MEMORY
 * {
 *   FLASH (rx)      				: ORIGIN = 0x08005000, LENGTH = 488K
 *   RAM (xrw)       				: ORIGIN = 0x20000000, LENGTH = 80K-16
 *   BOOTLOADER_DATA_EXCHANGE (rwx): ORIGIN = 0x20013FF0, LENGTH = 16
 *   MEMORY_B1 (rx)  				: ORIGIN = 0x60000000, LENGTH = 0K
 * }
 * _estack = ORIGIN(RAM) + LENGTH(RAM)-4;
 *
 * and in SECTIONS{:
 *
 *    https://mcuoneclipse.com/2014/04/19/gnu-linker-can-you-not-initialize-my-variable/
 *    https://mcuoneclipse.com/2014/04/19/gnu-linker-can-you-not-initialize-my-variable/
 *
 *  Uninitialized data section
 *  Define memory into the  BOOTLOADER_DATA_EXCHANGE section:
 *  .un_init (NOLOAD):
 *   {
 *     . = ALIGN(4);
 *     KEEP(*(.un_init))
 *    . = ALIGN(4);
 *  } > BOOTLOADER_DATA_EXCHANGE
 *
 *
 *
 * */

static bootloader_app_data_exchange_t bootloader_data_exchange __attribute__((section(".un_init")));

const uint32_t MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE = 0x5868FE4A;
const uint32_t MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_PROGRAMMODE = 0x48B4A946;

bootloader_preprogrammed_boot_mode_t port_dataex_get_preprogrammed_bootmode() {
    bootloader_preprogrammed_boot_mode_t result = boot_mode_none;
    uint32_t val = bootloader_data_exchange.magic_key_which_survives_soft_reset_and_mustn_be_inited;
    if (val == MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE) {
        result = boot_mode_app_launch;
    }

    if (val == MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_PROGRAMMODE) {
        result = boot_mode_firmware_update;
    }
    port_dataex_clear_bootmode();
    return result;
}

void port_dataex_set_bootmode_app_launch_after_reset(void) {
    bootloader_data_exchange.magic_key_which_survives_soft_reset_and_mustn_be_inited = MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE;
}

void port_dataex_set_bootmode_firmware_update_after_reset(void) {
    bootloader_data_exchange.magic_key_which_survives_soft_reset_and_mustn_be_inited = MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_PROGRAMMODE;
}

void port_dataex_clear_bootmode(void) {
    bootloader_data_exchange.magic_key_which_survives_soft_reset_and_mustn_be_inited = 0;
}

void port_dataex_set_bootloader_git_info(void) {
    bootloader_data_exchange.bootloader_git_hash = GITHASH;
    bootloader_data_exchange.bootloader_git_unixdate = GITUNIX;
}

uint32_t port_dataex_get_bootloader_git_hash(void) {
    return bootloader_data_exchange.bootloader_git_hash;
}

uint32_t port_dataex_get_bootloader_git_unixdate(void) {
    return bootloader_data_exchange.bootloader_git_unixdate;
}
