/*
 * port_board_preinit_example.c
 *
 *  Created on: 23.11.2018
 *      Author: ak
 */

void PreInit_board(void) {
    // called by startup.s of bootloader as first instruction. This way it is possible to initialize important GPIOs very quickly after bootup
}
