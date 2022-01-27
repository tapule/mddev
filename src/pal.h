/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: pal.h
 * Palette routines
 *
 * The Sega Megadrive/Genesis VDP can handle up to four paletes of 16 colors
 * each. The first color of each palette is used as transparency, unless used as
 * background color.
 * Tiles and sprites use this palettes to render its pixels.
 * Palettes are stored in the VDP's internal CRAM where 32 bytes are used for
 * each (2 bytes per color). Colors are stored in BGR format were each component
 * is a hexadecimal digit:
 *       0x0BGR  ->  0000 BBB0 GGG0 RRR0
 * Only even numbers can be used (i.e. 02468ACE).
 * There is no need to write an entire palette, you can write individual colors
 * too.
 *
 * More info:
 * https://www.plutiedev.com/tiles-and-palettes
 * 
 */

#ifndef MDDEV_PAL_H
#define MDDEV_PAL_H

#include <stdint.h>
#include <stdbool.h>

/* Palette identifiers */
#define PAL_0   0       /* Palette 0 - Colors 0..15 */
#define PAL_1   1       /* Palette 1 - Colors 16..31 */
#define PAL_2   2       /* Palette 2 - Colors 32..47 */
#define PAL_3   3       /* Palette 3 - Colors 48..64 */

/* Palettes CRAM starting indexes */
#define PAL_0_INDEX   0       /* Colors 0..15 */
#define PAL_1_INDEX   16      /* Colors 16..31 */
#define PAL_2_INDEX   32      /* Colors 32..47 */
#define PAL_3_INDEX   48      /* Colors 48..64 */

/**
 * @brief Initialises the palette system
 * 
 * We use two internal palete buffers (primary and alternate) to store color
 * values and to do operations (fades, palette swap). This function initialises
 * the needed data to manage the buffers.
 * 
 * @note This function is called from the boot process so maybe you don't need
 * to call it anymore.
 */
void pal_init(void);

/**
 * @brief Sets colors in the primary internal color buffer
 * 
 * @param index Position in the buffer were the color copy starts (0..63)
 * @param count Number of colors to copy (1..64)
 * @param colors Source color data
 * 
 * @note No boundary checks are done in the input parameters, keep them safe.
 */
void pal_primary_set(const uint16_t index, uint16_t count,
                     const uint16_t *restrict colors);

/**
 * @brief Sets colors in the alternate internal color buffer
 * 
 * @param index Position in the buffer were the color copy starts (0..63)
 * @param count Number of colors to copy (1..64)
 * @param colors Source color dapal_primary_set(16, 16, font00_pal);
 * 
 * @note No boundary checks are done in the input parameters, keep them safe.
 */
void pal_alternate_set(const uint16_t index, uint16_t count,
                       const uint16_t *restrict colors);

/**
 * @brief Sets colors directly in CRAM using DMA, bypassing the internal buffers
 * 
 * @param index Position in the buffer were the color copy starts (0..63)
 * @param count Number of colors to copy (1..64)
 * @param colors Source color data
 * 
 * @note No boundary checks are done in the input parameters, keep them safe.
 */
void pal_cram_set(uint16_t index, const uint16_t count,
                  const uint16_t *restrict colors);

/**
 * @brief Swaps the internal color buffers
 * 
 */
void pal_swap(void);

/**
 * @brief Starts a fade operation from the primary to alternate color buffers
 * 
 * @param speed Speed in frames between color fade updates
 */
void pal_fade(const uint16_t speed);

/**
 * @brief Advances the current color fade operation one step
 * 
 * @return True if fade operation still running, false if it ended
 */
bool pal_fade_step(void);

/**
 * @brief Stops the current running fade operation 
 * 
 */
void pal_fade_stop(void);

/**
 * @brief Waits for a runnig fade operation to finish
 * 
 */
void pal_fade_wait(void);

/**
 * @brief Tells if there is a color fade operation running
 * 
 * @return true if there is a color fade operation running, false otherwhise
 */
bool pal_is_fading(void);

/**
 * @brief Updates internal status and upload the primary buffer to CRAM
 * 
 * @note This function updates CRAM, so you should call it every frame after
 * waiting for the vertical blank (see vid_vsync_wait) or whenever you need to
 * upload your palettes to CRAM.
 */
void pal_update(void);

#endif /* MDDEV_PAL_H */
