/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: megadrive.c
 * ??
 */

#include "megadrive.h"

static uint16_t ints_status_flag;

void smd_init(void)
{
    /* Initialises the z80 secondary CPU */
    z80_init();
    /* Initialises gamepad ports */
    pad_init();
    /* Initialises the PSG (Programmable Sound Generator) */
    psg_init();
    /* Initialises the Yamaha YM2612 FM sound chip */
    ym2612_init();
    /* Initialises the graphics  */
    vid_init();
    /* Initialises the pseudo-random number generator */
    rnd_init();
    /* Initialises the DMA system  */
    dma_init();
    /* Initialises the palette system  */
    pal_init();
}

inline void smd_ints_enable(void)
{
    __asm__ volatile ("\tandi.w	#0xF8FF, %sr\n");
    ints_status_flag = 1;
}

inline void smd_ints_disable(void)
{
    __asm__ volatile ("\tori.w	#0x700, %sr\n");
    ints_status_flag = 0;    
}

inline uint16_t smd_ints_status(void)
{
    return ints_status_flag;
}