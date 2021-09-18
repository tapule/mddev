/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: pal.c
 * Palette routines
 */

#include "pal.h"
#include "video.h"
#include "dma.h"

/* Internal palette color buffers */
static uint16_t pal_buffers[2][64];
static uint16_t *pal_primary;
static uint16_t *pal_alternate;

/* Should us update CRAM with the primary buffer? */
static bool pal_update_needed;

/* Is there a fade operation running? */
static bool pal_fading;
/* Fade operation speed in frames */
static uint8_t pal_fade_speed;
/* Fade operation frame counter */
static uint8_t pal_fade_counter;

void pal_init(void)
{
    pal_primary = &pal_buffers[0][0];
    pal_alternate = &pal_buffers[1][0];
    pal_update_needed = false;
    pal_fading = false;
    pal_fade_speed = 0;
    pal_fade_counter = 0;
}

void pal_primary_set(const uint16_t index, uint16_t count,
                     const uint16_t *restrict colors)
{
    /* We update the primary buffer, so we need to update CRAM */
    pal_update_needed = true;

    while (count)
    {
        /* Adjust the offset to sum using 0..n style */
        --count;
        pal_primary[index + count] = colors[count];
    }
}

void pal_alternate_set(const uint16_t index, uint16_t count,
                       const uint16_t *restrict colors)
{
    while (count)
    {
        /* Adjust the offset to sum using 0..n style */
        --count;
        pal_alternate[index + count] = colors[count];
    }
}

inline void pal_cram_set(uint16_t index, const uint16_t count,
                         const uint16_t *restrict colors)
{
    /* We can't use a fast transfer here as we don't know the source address */
    /* CHECKME: Should I synchonize the internal primary buffer here?? */
    dma_cram_transfer(colors, index << 1, count, 2);
}

inline void pal_swap(void)
{
    uint16_t *tmp;

    tmp = pal_primary;
    pal_primary = pal_alternate;
    pal_alternate = tmp;

    pal_update_needed = true;
}

inline void pal_fade(const uint16_t speed)
{
    /* Fade operation setup */
    pal_fade_speed = speed;
    pal_fade_counter = 0;
    pal_fading = true;
}

bool pal_fade_step(void)
{
    uint16_t i = 64;
    uint16_t primary_component;
    uint16_t alternate_component;

    if (!pal_fading)
    {
        return false;
    }

    ++pal_fade_counter;
    if (pal_fade_counter == pal_fade_speed)
    {
        pal_fade_counter = 0;
        pal_update_needed = false;
        while (i)
        {
            /* Adjust the index to use 0..63 instead of 1..64 */
            --i;
            /* Updates red component in the primary color buffer */
            primary_component = pal_primary[i] & 0x00E;
            alternate_component = pal_alternate[i] & 0x00E;
            if (primary_component != alternate_component)
            {
                pal_update_needed = true;
                pal_primary[i] += primary_component < alternate_component ? 0x002 : -0x002;
            }
            /* Updates green component in the primary color buffer */
            primary_component = pal_primary[i] & 0x0E0;
            alternate_component = pal_alternate[i] & 0x0E0;
            if (primary_component != alternate_component)
            {
                pal_update_needed = true;
                pal_primary[i] += primary_component < alternate_component ? 0x020 : -0x020;
            }
            /* Updates blue component  in the primary color buffer */
            primary_component = pal_primary[i] & 0xE00;
            alternate_component = pal_alternate[i] & 0xE00;
            if (primary_component != alternate_component)
            {
                pal_update_needed = true;
                pal_primary[i] += primary_component < alternate_component ? 0x200 : -0x200;
            }
        }
        /* No color change in this step, so the fade operation ended */
        if (!pal_update_needed)
        {
            pal_fading = false;
            return false;
        }
    }
    /* The fade operation still running */
    return true;
}

inline void pal_fade_stop(void)
{
    pal_fading = false;
}

void pal_fade_wait(void)
{
    while (pal_fade_step())
    {
        vid_vsync_wait();
        pal_update();
    }
}

inline bool pal_is_fading(void)
{
    return pal_fading;
}

void pal_update(void)
{
    if (pal_update_needed)
    {
        pal_update_needed = false;
        /* Transfer the whole primary color buffer to CRAM */
        dma_cram_transfer_fast(pal_primary, 0, 64, 2);
    }
}
