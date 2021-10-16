/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: tiles.c
 * Functions to manage tiles in VRAM
 */

#include "tiles.h"
#include "dma.h"

inline void tiles_load(const void *restrict src, const uint16_t tile_index,
                       const uint16_t length)
{
    /*
       A real tile offset in VRAM is its index * 32 and the amount of words 
       to transfer would be length (in tiles) * 16 
    */
    dma_vram_transfer(src, tile_index << 5, length << 4, 2);
}

inline void tiles_load_fast(const void *restrict src, const uint16_t tile_index,
                            const uint16_t length)
{
   dma_vram_transfer_fast(src, tile_index << 5, length << 4, 2);
}