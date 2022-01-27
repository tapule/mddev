/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: sys.h
 * System core routines to manage the Sega Megadrive/Genesis hardware
 *
 * More info:
 * https://www.eeeguide.com/register-architecture-of-68000-microprocessor/
 */

#ifndef MDDEV_SYS_H
#define MDDEV_SYS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Enable system interrupts
 * 
 * The m68k provides 7 levels of interrupts were 7 has the highest priority and
 * 0 the lowest. For the Sega Megadrive/Genesis the most important interrupts
 * are:
 *  Level 6: Vertical retrace interrupt
 *  Level 4: Horizontal retrace interrupt
 *  Level 2: External interrupt
 * This function enables all of them
 */
void smd_ints_enable(void);

/**
 * @brief Disable system interrupts
 */
void smd_ints_disable(void);

/**
 * @brief Get interrupts status
 * 
 * @return True if interrupts are enabled, false otherwise
 */
bool smd_ints_status(void);

/**
 * @brief Checks if the system is using video mode PAL or NTSC
 * 
 * The Sega Megadrive/Genesis can run in PAL mode with a clock of 7.60 MHz or in
 * NTSC with a clock of 7.67 MHz.
 * 
 * @return True if the system is in PAL mode, false if it is running in NTSC
 */
bool smd_is_pal(void);

/**
 * @brief Checks if the system is running in Japanese (Domestic) mode
 * 
 * The Sega Megadrive/Genesis can run in Domestic mode (Japanese) or in Overseas
 * mode (US or European).
 * 
 * @return True if the system is running in Japasese mode or false if it is
 *         running in US/European mode
 */
bool smd_is_japanese(void);

/*
    * TODO:

*/

#endif /* MDDEV_SYS_H */
