/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: memory.h
 * Basic memory utilities
 */

#include "memory.h"

void mem_set(void *dest, uint8_t value, uint16_t size)
{
    uint8_t *p = (uint8_t *) dest;

    while (size--)
    {
        *(p++) = value;
    }
}

void mem_copy(void *dest, const void *src, uint16_t size)
{
    uint8_t *d = (uint8_t *) dest;
    const uint8_t *s = (const uint8_t *) src;

    while (size--)
    {
        *(d++) = *(s++);
    }
}
