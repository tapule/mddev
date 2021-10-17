/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: sys.c
 * System core routines to manage the Sega Megadrive/Genesis hardware
 */

#include "sys.h"

/*
 * The version port is a read only port that gives information about the Sega
 * Megadrive/Genesis hardware and version:
 *              |MOD|VMOD|DISK|RSV|VER3|VER2|VER1|VER0|
 *  MOD:     0 = Domestic (Japanese) model.
 *           1 = Overseas (US/European) model.
 *  VMOD:    0 = NTSC mode (7.67MHz CPU clock)
 *           1 = PAL mode (7.60MHz CPU clock)
 *  DISK:    0 = Expansion unit connected (MegaCD).
 *           1 = Expansion unit not connected
 *  RSV:     Reserved. Not used
 *  VER3..0: Megadrive/Genesis version
 */
#define VERSION_PORT     ((volatile uint8_t *) 0xA10001)

static bool ints_status_flag;


inline void smd_ints_enable(void)
{
    __asm__ volatile ("\tandi.w	#0xF8FF, %sr\n");
    ints_status_flag = true;
}

inline void smd_ints_disable(void)
{
    __asm__ volatile ("\tori.w	#0x700, %sr\n");
    ints_status_flag = false;    
}

inline bool smd_ints_status(void)
{
    return ints_status_flag;
}

inline bool smd_is_pal(void)
{
    return *VERSION_PORT & 0x40;
}

inline bool smd_is_japanese(void)
{
    return !(*VERSION_PORT & 0x80);
}