/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: mddev.h
 * MDdev common entry point
 *
 * This header includes all the needed resources to start building your Sega
 * Megadrive/Genesis homebrew.
 * You must include it in your proyects.
 */

#ifndef MDDEV_H
#define MDDEV_H

#define MDDEV_VERSION   0.1

#include "assert.h"
#include "config.h"

#include "memory.h"
#include "sys.h"
#include "z80.h"
#include "pad.h"
#include "psg.h"
#include "ym2612.h"
#include "sound.h"
#include "video.h"
#include "rand.h"
#include "dma.h"
#include "pal.h"
#include "tiles.h"
#include "sprite.h"
#include "plane.h"
#include "text.h"
#include "kdebug.h"

/**
 * @brief initialises the Sega Megadrive/Genesis hardware
 * 
 * Controls the initialisation process of the main hardware present in the
 * console. 
 * 
 * @note This function is called from the boot process so maybe you don't need
 * to call it anymore.
 */
void smd_init(void);

#endif /* MDDEV_H */
