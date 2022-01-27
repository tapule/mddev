/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: plane.h
 * VDP's planes drawing functions
 *
 * VDP's planes are tables stored in VRam that define what tiles will be drawn
 * on screen. Each cell in these tables is a word (16 bits) describing what tile
 * to draw and how to draw it according to this format:
 *      PCCVHTTTTTTTTTTT
 *      P: Priority flag
 *      C: Palete select
 *      V: Vertical flip flag
 *      H: Horizontal flip flag
 *      T: Tile index in VRam to drawn
 *
 * More info:
 * https://blog.bigevilcorporation.co.uk/2012/03/23/sega-megadrive-4-hello-world/
 * 
 */

#ifndef MDDEV_PLANE_H
#define MDDEV_PLANE_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

/* Plane convenient defines */
#define PLANE_A VID_PLANE_A_ADDR
#define PLANE_B VID_PLANE_B_ADDR
#define PLANE_W VID_PLANE_W_ADDR


/**
 * @brief Configures a plane cell tile with all its draw properties
 * 
 * @param tile_index VRam index of tile to drawn
 * @param palette CRam palete index (0..3)
 * @param h_flip Horizontal flip property (0 no flip, 1 flip horizontally)
 * @param v_flip Vertical flip property (0 no flip, 1 flip vertically)
 * @param priority Drawing priority (0 low priority, 1 high priority)
 * @return uint16_t Plane cell with all the properties configured in
 */
uint16_t plane_cell_config(const uint16_t tile_index, const uint16_t palette,
                           const uint16_t h_flip, const uint16_t v_flip,
                           const uint16_t priority);

/**
 * @brief Clears an entire VDP plane
 * 
 * @param plane Plane to clear
 * 
 * @note This function clears the plane immediately. Use it wisely with the
 * display off or in the vertical blank, otherwise you will get some glitches.
 */
void plane_clear(const uint16_t plane);

/**
 * @brief Draws a rectangle of tiles in a concrete position of a plane
 * 
 * @param plane Destination plane where tiles should be drawn
 * @param tile Tile index or a full cell tile config to use as fill value
 * @param x Plane horizontal position in cells 
 * @param y Plane vertical position in cells
 * @param width Rectangle width in tiles
 * @param height Rectangle height in tiles
 * 
 * @note This function draws the plane immediately. Use it wisely with the
 * display off or in the vertical blank, otherwise you will get some glitches.
 */
void plane_rect_fill(const uint16_t plane, const uint16_t tile,
                     const uint16_t x, const uint16_t y, const uint16_t width,
                     const uint16_t height);

/**
 * @brief Draws a tile in a concrete position of a plane
 * 
 * @param plane Destination plane where the tile should be drawn
 * @param tile Tile index or a full cell tile config
 * @param x Plane horizontal position in cells 
 * @param y Plane vertical position in cells
 *
 * @note This function draws the tile immediately. Use it wisely with the
 * display off or in the vertical blank, otherwise you will get some glitches.
 */
void plane_tile_draw(const uint16_t plane, const uint16_t tile,
                     const uint16_t x, const uint16_t y);

/**
 * @brief Draws a horizontal line of tiles in a concrete position of a plane
 * 
 * @param plane Destination plane where tiles should be drawn
 * @param tiles Source tiles indexes or full cells tiles configurations
 * @param x Plane horizontal position in cells 
 * @param y Plane vertical position in cells
 * @param length Line of tiles length
 * @param defer True to enqueue the operation, false to do it directly
 */
void plane_hline_draw(const uint16_t plane, const uint16_t *restrict tiles,
                      const uint16_t x, const uint16_t y, const uint16_t length,
                      const bool defer);

/**
 * @brief Draws faster a horizontal line of tiles in a position of a plane
 * 
 * @param plane Destination plane where tiles should be drawn
 * @param tiles Source tiles indexes or full cells tiles configurations
 * @param x Plane horizontal position in cells 
 * @param y Plane vertical position in cells
 * @param length Line of tiles length
 *
 * @note This function is meant to use RAM as tile's data source. To use it from
 * ROM, make sure to check 128kB boundaries. It also draws the line immediately.
 * Use it wisely with the display off or in the vertical blank, otherwise you
 * will get some glitches.
 */
void plane_hline_draw_fast(const uint16_t plane, const uint16_t *restrict tiles,
                           const uint16_t x, const uint16_t y,
                           const uint16_t length);

/**
 * @brief Draws a vertical line of tiles in a concrete position of a plane
 * 
 * @param plane Destination plane where tiles should be drawn
 * @param tiles Source tiles indexes or full cells tiles configurations
 * @param x Plane horizontal position in cells 
 * @param y Plane vertical position in cells
 * @param length Line of tiles length
 * @param defer True to enqueue the operation, false to do it directly
 */
void plane_vline_draw(const uint16_t plane, const uint16_t *restrict tiles,
                      const uint16_t x, const uint16_t y, const uint16_t length,
                      const bool defer);

/**
 * @brief Draws faster a vertical line of tiles in a position of a plane
 * 
 * @param plane Destination plane where tiles should be drawn
 * @param tiles Source tiles indexes or full cells tiles configurations
 * @param x Plane horizontal position in cells 
 * @param y Plane vertical position in cells
 * @param length Line of tiles length
 *
 * @note This function is meant to use RAM as tile's data source. To use it from
 * ROM, make sure to check 128kB boundaries. It also draws the line immediately.
 * Use it wisely with the display off or in the vertical blank, otherwise you
 * will get some glitches.
 */
void plane_vline_draw_fast(const uint16_t plane, const uint16_t *restrict tiles,
                           const uint16_t x, const uint16_t y,
                           const uint16_t length);

/**
 * @brief Draws a rectangle of tiles in a concrete position of a plane
 * 
 * @param plane Destination plane where tiles should be drawn
 * @param tiles Source tiles indexes or full cells tiles configurations
 * @param x Plane horizontal position in cells 
 * @param y Plane vertical position in cells
 * @param width Rectangle width in tiles
 * @param height Rectangle height in tiles
 * @param defer True to enqueue the operation, false to do it directly
 */
void plane_rect_draw(const uint16_t plane, const uint16_t *restrict tiles,
                     const uint16_t x, const uint16_t y, const uint16_t width,
                     const uint16_t height, const bool defer);

/**
 * @brief Draws faster a rectangle of tiles in a concrete position of a plane
 * 
 * @param plane Destination plane where tiles should be drawn
 * @param tiles Source tiles indexes or full cells tiles configurations
 * @param x Plane horizontal position in cells 
 * @param y Plane vertical position in cells
 * @param width Rectangle width in tiles
 * @param height Rectangle height in tiles
 *
 * @note This function is meant to use RAM as tile's data source. To use it from
 * ROM, make sure to check 128kB boundaries. It also draws the rectangle
 * immediately. Use it wisely with the display off or in the vertical blank,
 * otherwise you will get some glitches.
 */
void plane_rect_draw_fast(const uint16_t plane, const uint16_t *restrict tiles,
                          const uint16_t x, const uint16_t y,
                          const uint16_t width, const uint16_t height);

#endif /* MDDEV_PLANE_H */
