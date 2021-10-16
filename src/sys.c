/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: sys.c
 * ??
 */

#include "sys.h"

static uint16_t ints_status_flag;


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
