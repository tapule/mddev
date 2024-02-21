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

typedef enum
{
    SPR_SIZE_1X1 = 0x00,
    SPR_SIZE_1X2 = 0x01,
    SPR_SIZE_1X3 = 0x02,
    SPR_SIZE_1X4 = 0x03,
    SPR_SIZE_2X1 = 0x04,
    SPR_SIZE_2X2 = 0x05,
    SPR_SIZE_2X3 = 0x06,
    SPR_SIZE_2X4 = 0x07,
    SPR_SIZE_3X1 = 0x08,
    SPR_SIZE_3X2 = 0x09,
    SPR_SIZE_3X3 = 0x0A,
    SPR_SIZE_3X4 = 0x0B,
    SPR_SIZE_4X1 = 0x0C,
    SPR_SIZE_4X2 = 0x0D,
    SPR_SIZE_4X3 = 0x0E,
    SPR_SIZE_4X4 = 0x0F
} spr_size_t;


void spr_init(void);

uint16_t spr_attributes_encode(const uint16_t priority, const uint16_t palette,
                               const uint16_t v_flip, const uint16_t h_flip,
                               const uint16_t tile_index);
// spr_attributes_priority_set
// spr_attributes_palette_set
// // void spr_attributes_vflip_set(uint16_t* attributes);
void spr_attributes_vflip(uint16_t* attributes);
uint16_t spr_attributes_vflipb(uint16_t attributes);

// // void spr_attributes_hflip_set(uint16_t* attributes);
void spr_attributes_hflip(uint16_t* attributes);

// spr_attributes_index_set


uint8_t spr_size_encode(const uint8_t width, const uint8_t height);

uint8_t spr_size_to_tiles(const spr_size_t size);



void spr_add(int16_t x, int16_t y, uint16_t attributes, uint8_t size);

void spr_clear(void);

void spr_update(void);

#endif /* MDDEV_SPRITE_H */
