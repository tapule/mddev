/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: memory.h
 * Basic memory utilities
 * 
 */

#ifndef MDDEV_MEMORY_H
#define MDDEV_MEMORY_H

#include <stdint.h>

/**
 * @brief Fill a memory area with a constant value
 * 
 * Fills an amount of bytes pointed by dest with a constant value 
 * 
 * @param dest Destination memory address
 * @param value Value used to fill the memory area
 * @param size Amount of bytes to fill
 */
void mem_set(void *dest, uint8_t value, uint16_t size);

/**
 * @brief Copy a memory area
 * 
 * Copy an amount of bytes from a source area to a destination area
 * 
 * @param dest Destination memory address
 * @param src Source data
 * @param size Amount of bytes to copy
 */
void mem_copy(void *dest, const void *src, uint16_t size);

#endif /* MDDEV_MEMORY_H */
