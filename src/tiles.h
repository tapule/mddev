/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: tiles.h
 * Functions to manage tiles in VRAM
 *
 * The Sega Megadrive/Genesis VDP uses tiles as the basic unit. A tile is a
 * square 8x8 pixel image that can have up to 16 colors (one of them
 * transparency). It is used as a base to draw everything: Backgrounds,
 * Foregrounds, Sprites, etc.
 * A tile uses 32 bytes of memory where each pixel is represented by 4 bits (1
 * hexadecimal digit).
 * 
 * More info:
 * https://www.plutiedev.com/tiles-and-palettes
 * https://blog.bigevilcorporation.co.uk/2012/03/23/sega-megadrive-4-hello-world
 * 
 */

#ifndef MDDEV_TILES_H
#define MDDEV_TILES_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Loads tiles to VRAM using DMA
 * 
 * @param src Source tiles address on RAM/ROM space
 * @param tile_index Destination tile index on VRAM
 * @param length Amount of tiles to load to VRAM
 */
void tiles_load(const void *restrict src, const uint16_t tile_index,
                const uint16_t length);

/**
 * @brief Loads tiles to VRAM using DMA without checks
 * 
 * @param src Source tiles address on RAM/ROM space
 * @param tile_index Destination tile index on VRAM
 * @param length Amount of tiles to load to VRAM
 * 
 * @note This function is meant to use RAM as tile's data source. To use it from
 * ROM, make sure to check 128kB boundaries.
 */
void tiles_load_fast(const void *restrict src, const uint16_t tile_index,
                     const uint16_t length);

#endif /* MDDEV_TILES_H */
