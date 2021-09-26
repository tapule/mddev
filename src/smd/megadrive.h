/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: megadrive.h
 * Sega Megadrive/Genesis main control unit
 *
 * System core routines to manage the Sega Megadrive/Genesis hardware
 *
 */

#ifndef MEGADRIVE_H
#define MEGADRIVE_H

#include "config.h"

#include "z80.h"
#include "pad.h"
#include "psg.h"
#include "video.h"
#include "rand.h"
#include "dma.h"
#include "pal.h"
#include "tiles.h"
#include "plane.h"
#include "text.h"
#include "kdebug.h"


/**
 * @brief 
 * 
 */
void smd_init(void);

/**
 * @brief Habilita las interrupciones
 * 
 */
void smd_ints_enable(void);

/**
 * @brief deshabilita las interrupciones
 * 
 */
void smd_ints_disable(void);

/**
 * @brief Obtiene el estado de las interrupciones
 * 
 */
uint16_t smd_ints_status(void);



/*
    * TODO:

    smd_ints_mask_set
        Set interrupts mask
    smd_ints_mask_get
        Get interrupts mask
*/


#endif /* MEGADRIVE_H */
