/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: ym2612.c
 * Control routines for Sega Megadrive/Genesis Yamaha YM2612 FM sound chip
 */

#include <stdint.h>
#include "ym2612.h"
#include "z80.h"

/* YM2612 port from the m68k side */
#define YM2612_PORT ((volatile uint8_t *) 0xA04000)

void ym2612_init(void)
{
    uint16_t i;
#if 0
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
#endif
}