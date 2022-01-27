/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: sprite.h
 * ??
 *
 * ??
 *
 * More info:
 * 
 */

#ifndef MDDEV_SPRITE_H
#define MDDEV_SPRITE_H

#include<stdint.h>

void spr_init(void);

uint16_t spr_attributes_set(const uint16_t tile_index, const uint16_t palette,
                            const uint16_t h_flip, const uint16_t v_flip,
                            const uint16_t priority);

uint8_t spr_size_set(const uint8_t width, const uint8_t height);

void spr_add(int16_t x, int16_t y, uint16_t attributes, uint8_t size);

void spr_clear(void);

void spr_update(void);

#endif /* MDDEV_SPRITE_H */
