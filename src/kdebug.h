/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: kdebug.h
 * Gens KMod Debugging routines 
 *
 * Gens KMod is a modified version developed by Kaneda of Gens emulator by 
 * Stephane Dallongeville. Among other improvements KMod adds some debugging
 * features. These features are also supported by Blastem emulator.
 *
 * More info:
 * https://documentation.help/Gens-KMod/documentation.pdf
 */

#ifndef MDDEV_KDEBUG_H
#define MDDEV_KDEBUG_H

/* Debugging disabled so do not evaluate kdebug functions. */
#ifdef NDEBUG

#define kdebug_halt() ((void)0)
#define kdebug_alert(x) ((void)0)
#define kdebug_timer_start() ((void)0)
#define kdebug_timer_stop() ((void)0)
#define kdebug_timer_output() ((void)0)

#else

#define kdebug_halt() __kdebug_halt()
#define kdebug_alert(x) __kdebug_alert(x)
#define kdebug_timer_start() __kdebug_timer_start()
#define kdebug_timer_stop() __kdebug_timer_stop()
#define kdebug_timer_output() __kdebug_timer_output()

/**
 * @brief Pauses rom emulation
 */
void __kdebug_halt(void);

/**
 * @brief Outputs a message string
 * 
 * Shows a text message on the emulator's Message window.
 * 
 * @param str   Text string to output
 */
void __kdebug_alert(const char *str);

/**
 * @brief Starts the emulator timer
 * 
 * It starts an internal emulator timer counter based on m68k cycles.
 */
void __kdebug_timer_start(void);

/**
 * @brief Stops the emulator timer
 * 
 * It stops the internal emulator timer counter and output its value in the
 * emulator's Message window
 */
void __kdebug_timer_stop(void);

/**
 * @brief Outputs current emulator timer value
 */
void __kdebug_timer_output(void);

#endif

#endif /* MDDEV_KDEBUG_H */
