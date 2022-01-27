/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: ym2612.h
 * Control routines for Sega Megadrive/Genesis Yamaha YM2612 FM sound chip
 *
 * The Yamaha YM2612 FM sound chip is a sound synthesizer integrated circuit
 * that can generate six simultaneous tones, each with their own configuration
 * of FM operators. Its capabilities include:
 *    6 channels of FM sound
 *    An 8-bit Digitized Audio channel (as replacement for one of FM channels)
 *    Stereo output capability
 *    One LFO (low frequency oscillator) to distort the FM sounds
 *    2 timers, for use by software 
 *
 * More info:
 * https://www.smspower.org/maxim/Documents/YM2612
 * https://www.chibiakumas.com/68000/platform3.php
 * https://www.plutiedev.com/ym2612-operations
 */

#ifndef MDDEV_YM2612_H
#define MDDEV_YM2612_H

/**
 * @brief initialises the YM2612 sound hardware.
 * 
 * Controls the initialisation process of the YM2612.
 * 
 * @note This function is called from the boot process so maybe you don't need
 * to call it anymore.
 */
void ym2612_init(void);

#endif /* MDDEV_YM2612_H */
