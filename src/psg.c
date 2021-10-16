/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: psg.c
 * Control routines for Sega Megadrive/Genesis PSG
 */

#include <stdint.h>
#include "psg.h"

/* PSG port from the m68k side */
#define PSG_PORT ((volatile uint8_t *) 0xC00011)

void psg_init(void)
{
    uint16_t i;

    /* Do silence in all 4 channels */
    for (i = 0; i < 4; ++i)
    {
        /* Set volume (attenuation) to 15 which is silence */
        *PSG_PORT = 0x90 | (i << 5) | 0x0F;

        /* 
         * Set frecuency (pitch) to 0
         * Changing the pitch requires writing two bytes to the PSG port
         */
        *PSG_PORT = 0x80 | (i << 5) | 0x00;
        *PSG_PORT = 0x00;
    }
}