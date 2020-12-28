/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Copyright: 2020 Juan Ángel Moreno Fernández (@_tapule)
 * Github: https://github.com/tapule/mddev
 *
 * File: psg.h
 * Control routines for Sega Megadrive/Genesis PSG
 *
 * The PSG (Programmable Sound Generator) is a sound hardware for simple sounds
 * output. It contains four channels, three of them are tone generators (square
 * wave generators) and the last one is a noise genrator. Each channel has its
 * own volume control.
 *
 * More info:
 * https://www.plutiedev.com/psg
 * https://blog.bigevilcorporation.co.uk/2012/09/03/sega-megadrive-10-sound-part-i-the-psg-chip/
 */

#ifndef PSG_H
#define PSG_H

/**
 * @brief Initializes the PSG sound hardware.
 * 
 * Controls the initialization process of the PSG. It reset the channels and
 * put them in silence.
 * 
 * @note This function is called from the boot process so maybe you don't need
 * to call it anymore.
 */
void psg_init(void);

#endif // PSG_H
