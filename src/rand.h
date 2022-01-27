/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: rand.h
 * Control routines for Pseudo-Random Numbers Generation
 *
 * Implementation of pseudo-random number generation using Xorshift algorithm
 * by George Marsaglia
 *
 * More info:
 * http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html
 */

#ifndef MDDEV_RAND_H
#define MDDEV_RAND_H

#include <stdint.h>

/**
 * @brief initialises the prng.
 * 
 * Starts the pseudo-random number generator with a random seed
 * 
 * @note This function is called from the boot process so maybe you don't need
 * to call it anymore.
 */
void rnd_init(void);

/**
 * @brief Sets a new seed for prng
 * 
 * Setting the seed used to generate pseudo-random numbers lets you repeat the
 * sequence of numbers generated
 * 
 * @param seed  New seed
 */
void rnd_seed_set(const uint16_t seed);

/**
 * @brief Gets the current seed
 * 
 * Returns the current seed used to generate pseudo-random numbers
 * 
 * @return uint16_t The current seed
 */
uint16_t rnd_seed_get(void);

/**
 * @brief Generates a new random number
 * 
 * Returns a new pseudo-random number
 * 
 * @return uint16_t The new number
 */
uint16_t rnd_get(void);

#endif /* MDDEV_RAND_H */