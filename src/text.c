/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: text.c
 * Text rendering functions
 */

#include "text.h"

static uint16_t text_base_tile;
static uint16_t text_tileset_index;

inline void text_font_set(const uint16_t tileset_index)
{
    text_tileset_index = tileset_index;
}

inline void text_pal_set(const uint16_t palette)
{
    /* Sets to 0 the palete bits */
    text_tileset_index &= ~(0x03 << 13);
    /* Move new palette bits to its positions */
    text_tileset_index |= (0x03 & palette) << 13;
}

void text_priority_set(const uint16_t priority)
{
    /* Sets to 0 the priority bit */
    text_tileset_index &= ~(0x01 << 15);
    /* Move new priority bit to its position */
    text_tileset_index |= (0x01 & priority) << 15;
}

uint16_t text_render(const char *str, uint16_t *dest)
{
    uint16_t count;

    count = 0;
    while (*str)
    {
        *dest = text_base_tile + (text_tileset_index + *str - 32);
        ++dest;
        ++str;
        ++count;
    }

    return count;
}

uint16_t text_nrender(const char *str, uint16_t *dest, const uint16_t size)
{
    uint16_t i;

    i = size;
    while (*str && i)
    {
        *dest = text_base_tile + (text_tileset_index + *str - 32);
        ++dest;
        ++str;
        --i;
    }

    return size - i;
}
