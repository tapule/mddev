/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: config.h
 * Configuration parameters
 *
 * Diferent configuration preferences for the Sega MegaDrive/Genesis hardware.
 * Here you can configure how the hardware sould start and behave.
 * These values are used internally to do some operations, so it is mandatory to
 * define them.
 * 
 */

#ifndef MDDEV_CONFIG_H
#define MDDEV_CONFIG_H

#include "video.h"

/* 
 * Video configuration default values
 * 
 *  VDP memory layout:
 *  #0000..#BFFF - 1536 tiles
 *  #C000..#CFFF - Plane A (64x32, 4096 Bytes)
 *  #D000..#D000 - Plane W (0x0)
 *  #D000..#DFFF - 128 tiles
 *  #E000..#EFFF - Plane B (64x32, 4096 Bytes)
 *  #F000..#F7FF - 64 Tiles
 *  #F800..#FBBF - HScroll table (960 Bytes)
 *  #FBC0..#FBFF - 2 tiles
 *  #FC00..#FE7F - Sprite table (640 Bytes)
 *  #FE80..#FFFF - 12 tiles
 *  -> 1742 free tiles
 */
/* Plane start address in VRAM */
#define VID_PLANE_A_ADDR 0xC000
#define VID_PLANE_B_ADDR 0xE000
#define VID_PLANE_W_ADDR 0xD000
/* Sprite and horizontal scroll tables address in VRAM */
#define VID_SPRITE_TABLE_ADDR  0xFC00
#define VID_HSCROLL_TABLE_ADDR 0xF800
/* Horizontal and vertical plane scroll modes */
#define VID_HSCROLL_MODE VID_HSCROLL_TILE
#define VID_VSCROLL_MODE VID_VSCROLL_PLANE 
/* Planes sizes in tiles */
#define VID_PLANE_SIZE VID_PLANE_SIZE_64X32 
#define VID_PLANE_TILES 2048
#define VID_PLANE_WIDTH 64
#define VID_PLANE_HEIGTH 32

/* 
 * DMA configuration default values
 */
/* DMA internal queue size in operations */
#define DMA_QUEUE_SIZE 64

#endif /* MDDEV_CONFIG_H */
