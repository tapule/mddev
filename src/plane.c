/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: plane.c
 * VDP's planes drawing functions
 */

#include "plane.h"
#include "config.h"
#include "vdp.h"
#include "dma.h"

inline uint16_t plane_cell_config(const uint16_t tile_index,
                                 const uint16_t palette, const uint16_t h_flip,
                                 const uint16_t v_flip, const uint16_t priority)
{
    return (priority << 15) | (palette << 13) | (v_flip << 12) |
           (h_flip << 11) | tile_index;
}

inline void plane_clear(const uint16_t plane)
{
    dma_vram_fill(plane, VID_PLANE_TILES << 1, 0x00, 1);
}

void plane_rect_fill(const uint16_t plane, const uint16_t tile,
                     const uint16_t x, const uint16_t y, const uint16_t width,
                     const uint16_t height)
{
    uint16_t tile_row[VID_PLANE_WIDTH];
    uint16_t i;
    
    /* Setup tile buffer */
    for (i = 0; i < width; ++i)
    {
        tile_row[i] = tile;
    }
    /* Draws rows in the plane */
    for (i = 0; i < height; ++i)
    {
        /* x, y, i and VID_PLANE_WIDTH are in tiles, convert to words */
        dma_vram_transfer_fast(&tile_row,
                               plane + ((x + ((y + i) * VID_PLANE_WIDTH)) << 1),
                               width, 2);
    }
}

void plane_tile_draw(const uint16_t plane, const uint16_t tile,
                     const uint16_t x, const uint16_t y)
{
    uint16_t vram_addr;

    /* It doesn't make sense to use DMA for only one tile. Write it directly  */
    vram_addr = plane + ((x + (y * VID_PLANE_WIDTH)) << 1);
    *VDP_PORT_CTRL_L = (((uint32_t)(VDP_VRAM_WRITE_CMD)) |
                       (((uint32_t)(vram_addr) & 0x3FFF) << 16) |
                       ((uint32_t)(vram_addr) >> 14));
    *VDP_PORT_DATA_W = tile;
}

void plane_hline_draw(const uint16_t plane, const uint16_t *restrict tiles,
                      const uint16_t x, const uint16_t y, const uint16_t length,
                      const bool defer)
{
    if (defer)
    {
        dma_queue_vram_transfer(tiles,
                                plane + ((x + (y * VID_PLANE_WIDTH)) << 1),
                                length, 2);
    }
    else
    {
        dma_vram_transfer(tiles, plane + ((x + (y * VID_PLANE_WIDTH)) << 1),
                          length, 2);
    }
}

void plane_hline_draw_fast(const uint16_t plane, const uint16_t *restrict tiles,
                           const uint16_t x, const uint16_t y,
                           const uint16_t length)
{
    dma_vram_transfer_fast(tiles, plane + ((x + (y * VID_PLANE_WIDTH)) << 1),
                           length, 2);
}

void plane_vline_draw(const uint16_t plane, const uint16_t *restrict tiles,
                      const uint16_t x, const uint16_t y, const uint16_t length,
                      const bool defer)
{
    if (defer)
    {
        dma_queue_vram_transfer(tiles,
                                plane + ((x + (y * VID_PLANE_WIDTH)) << 1),
                                length, VID_PLANE_WIDTH << 1);
    }
    else
    {
        dma_vram_transfer(tiles,
                          plane + ((x + (y * VID_PLANE_WIDTH)) << 1), length,
                          VID_PLANE_WIDTH << 1);
    }
}

void plane_vline_draw_fast(const uint16_t plane, const uint16_t *restrict tiles,
                           const uint16_t x, const uint16_t y,
                           const uint16_t length)
{
    dma_vram_transfer_fast(tiles, plane + ((x + (y * VID_PLANE_WIDTH)) << 1),
                           length, VID_PLANE_WIDTH << 1);
}

void plane_rect_draw(const uint16_t plane, const uint16_t *restrict tiles,
                     const uint16_t x, const uint16_t y, const uint16_t width,
                     const uint16_t height, const bool defer)
{
    uint16_t row;

    if (defer)
    {
        for (row = 0; row < height; ++row)
        {
            dma_queue_vram_transfer(tiles + (row * width),
                             plane + ((x + ((y + row) * VID_PLANE_WIDTH)) << 1),
                             width, 2);
        }
    }
    else
    {
        for (row = 0; row < height; ++row)
        {

            dma_vram_transfer(tiles + (row * width),
                             plane + ((x + ((y + row) * VID_PLANE_WIDTH)) << 1),
                             width, 2);
        }
    }
}

void plane_rect_draw_fast(const uint16_t plane, const uint16_t *restrict tiles,
                          const uint16_t x, const uint16_t y,
                          const uint16_t width, const uint16_t height)
{
    uint16_t row;

    for (row = 0; row < height; ++row)
    {

        dma_vram_transfer_fast(tiles + (row * width),
                             plane + ((x + ((y + row) * VID_PLANE_WIDTH)) << 1),
                             width, 2);
    }
}
