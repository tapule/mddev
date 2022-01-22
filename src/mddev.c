/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: mddev.c
 * MDdev common entry point
 */

#include "mddev.h"

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
    /* Initialises the XGM based sound engine */
    sound_init();
    /* Initialises the graphics  */
    vid_init();
    /* Initialises the pseudo-random number generator */
    rnd_init();
    /* Initialises the DMA system  */
    dma_init();
    /* Initialises the palette system  */
    pal_init();
    /* Initialises the palette system  */
    spr_init();
}
