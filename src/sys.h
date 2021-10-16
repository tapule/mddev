/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: sys.h
 * ??
 *
 * System core routines to manage the Sega Megadrive/Genesis hardware
 *
 * More info:
 * 
 */

#ifndef SYS_H
#define SYS_H

#include <stdint.h>

//void sys_interrupts_enable(void);
//void sys_interrupts_disable(void);

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


#endif /* SYS_H */
