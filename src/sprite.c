/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021
 * Github: https://github.com/tapule/mddev
 *
 * File: sprite.c
 * ??
 */

#include "sprite.h"
#include "config.h"
#include "dma.h"
#include "kdebug.h"

#define SPR_MAX 80

/*
 * VDP Sprite attribute table entry
 * In the Sega Megadrive/Genesis VDP all the sprite data is stored in a table in
 * VRAM. Each entry in this table (8 bytes) describes a sprite with this format:
 *      Index + 0  :   ------yy yyyyyyyy
 *      Index + 2  :   ----hhvv
 *      Index + 3  :   -lllllll
 *      Index + 4  :   pccvhttt tttttttt
 *      Index + 6  :   ------xx xxxxxxxx
 *
 *      y = Vertical coordinate
 *      h = Horizontal size in tiles (00b = 1 tile, 11b = 4 tiles)
 *      v = Vertical size in tiles (00b = 1 tile, 11b = 4 tiles)
 *      l = Link, next sprite index to be drawn.
 *      p = Priority bit
 *      c = Color palette to drawn this sprite
 *      v = Vertical flip flag
 *      h = Horizontal flip flag
 *      t = Sprite tile data start index in VRAM
 *      x = Vertical coordinate
 */
typedef struct spr_entry
{
	int16_t y;
	uint8_t size;
	uint8_t link;
	uint16_t attributes;
	int16_t x;
} spr_entry_t;

/* Sprite table buffer and sprites counter */
static spr_entry_t spr_table[SPR_MAX];
static spr_entry_t* spr_next;
static uint8_t spr_count;


inline void spr_init(void)
{
    spr_clear();
}

inline uint16_t spr_attributes_set(const uint16_t tile_index,
                                const uint16_t palette, const uint16_t h_flip,
                                const uint16_t v_flip, const uint16_t priority)
{
    return (priority << 15) | (palette << 13) | (v_flip << 12) |
           (h_flip << 11) | tile_index;
}

inline uint8_t spr_size_set(const uint8_t width, const uint8_t height)
{
    return (((height - 1) & 0x03) | (((width - 1) & 0x03) << 2));
}

/* Temporal function, only for test purpouse */
/*
void spr_set_vflag(uint16_t* attributes, const uint16_t v_flip)
{
    if (v_flip)
    {
        *attributes |= (1<< 12);
    }
    else
    {
        *attributes &= ~(1<<12);
    }
}*/

void spr_add(int16_t x, int16_t y, uint16_t attributes, uint8_t size)
{
    /* Check sprite limit */
    if (spr_count == SPR_MAX)
    {
        return;
    }
    /* Ignore off-screen sprites */
    if (x <= -32 || x >= 320)
    {
        return;
    }
    if (y <= -32 || y >= 240)
    {
        return;
    }

    spr_next->y = y + 128;
    spr_next->size = size;
    spr_next->link = spr_count + 1;
    spr_next->attributes = attributes;
    spr_next->x = x + 128;
    ++spr_next;
    ++spr_count;
}

inline void spr_clear(void)
{
    spr_count = 0;
    spr_next = &spr_table[0];
    spr_next->x = 0;
    spr_next->link = 0;
}

void spr_update(void)
{
    if (spr_count > 0)
    {
        spr_table[spr_count - 1].link = 0;
    }
    else
    {
        spr_count = 1;
    }

    /* (spr_count * sizeof(spr_entry_t)) / 2  =  spr_count << 2 because spr_entry_t is 8 bytes */
    dma_vram_transfer_fast(spr_table, VID_SPRITE_TABLE_ADDR, spr_count << 2, 2);

    spr_clear();
}