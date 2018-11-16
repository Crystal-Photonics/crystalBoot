/*
 * port_bootloader_app_data_exchange.c
 *
 *  Created on: 16.11.2018
 *      Author: ak
 */

#ifndef CRYSTALBOOT_MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_BOOTLOADER_APP_DATA_EXCHANGE_C_
#define CRYSTALBOOT_MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_BOOTLOADER_APP_DATA_EXCHANGE_C_

typedef struct {
    uint32_t magic_key_which_survives_soft_reset_and_mustn_be_inited;
    uint32_t bootloader_git_hash;
    uint32_t bootloader_git_unixdate;
} bootloader_app_data_exchange_t;

typedef enum { boot_mode_none, boot_mode_app_launch, boot_mode_firmware_update } bootloader_preprogrammed_boot_mode_t;

bootloader_preprogrammed_boot_mode_t port_dataex_get_preprogrammed_bootmode();
// bool port_dataex_is_bootmode_firmware_update_after_reset_set();

void port_dataex_set_bootmode_app_launch_after_reset(void);
void port_dataex_set_bootmode_firmware_update_after_reset(void);
void port_dataex_clear_bootmode(void);

void port_dataex_set_bootloader_git_info(void);
uint32_t port_dataex_get_bootloader_git_hash(void);
uint32_t port_dataex_get_bootloader_git_unixdate(void);

#endif /* CRYSTALBOOT_MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_BOOTLOADER_APP_DATA_EXCHANGE_C_ */
