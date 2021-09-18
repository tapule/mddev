/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: vdp.h
 * Constants and definitions for Sega Megadrive/Genesis VDP
 *
 * Diferent VDP's constants and definitions shared across various modules
 * 
 */

#ifndef VDP_H
#define VDP_H

#include <stdint.h>
/*
 * There are 3 ports to talk with the VDP. These ports can be accessed as 16 or
 * 32 bits.
 * To work with the VDP we need to write commands to the control port and if we
 * want to write or read vram, cram or vsram we must write the address to/from
 * the data port.
 * The control port is also the VDP's status register, so it can be read to get
 * this information:
 *              | *| *| *| *| *| *|FE|FF|
 *              |VI|SO|SC|OD|VB|HB|DMA|PAL|
 *  FE: 1 = FIFO is empty.
 *  FF: 1 = FIFO is full.
 *  VI: 1 = Vertical interrupt occurred.
 *  SO: 1 = Sprite limit has been hit on current scanline
 *          17+ in 256 pixel wide mode
 *          21+ in 320 pixel wide mode.
 *  SC: 1 = Collision happened between non-zero pixels in two sprites. Used for
 *          pixel-accurate collision detection.
 *  OD: 1 = Odd frame displayed in interlaced mode
 *  VB: 1 = Vertical blank in progress.
 *  HB: 1 = Horizontal blank in progress.
 *  DMA: 1 = DMA in progress.
 *  PAL: 1 = PAL system
 *       0 = NTSC system.
 */
#define VDP_PORT_DATA_W     ((volatile uint16_t *) 0xC00000)
#define VDP_PORT_DATA_L     ((volatile uint32_t *) 0xC00000)
#define VDP_PORT_CTRL_W     ((volatile uint16_t *) 0xC00004)
#define VDP_PORT_CTRL_L     ((volatile uint32_t *) 0xC00004)
#define VDP_PORT_HV_COUNTER ((volatile uint16_t *) 0xC00008)

/*
 * The VDP has 24 registers (some of them not used) which control how video
 * hardware behaves. These registers are set by writing a control word to the
 * VDP control port where the high byte is as follow:
 *              | 1| 0| 0|R4|R3|R2|R1|R0|
 * R4-R0 is the register to write, so 0x81 (10000001) will write to the register
 * 0x01 wich is the Mode register 2.
 * Registers 0x06, 0x08, 0x09, 0x0E are not used and are always 0x00
 */
#define VDP_REG_MODESET_1       0x8000      /* Mode register #1 */
#define VDP_REG_MODESET_2       0x8100      /* Mode register #2 */
#define VDP_REG_PLANEA_ADDR     0x8200      /* Plane A data table address */
#define VDP_REG_WINDOW_ADDR     0x8300      /* Window data table address */
#define VDP_REG_PLANEB_ADDR     0x8400      /* Plane B data table address */
#define VDP_REG_SPRITE_ADDR     0x8500      /* Sprite data table address */
#define VDP_REG_BGCOLOR         0x8700      /* Background color */
#define VDP_REG_HBLANK_RATE     0x8A00      /* HBlank interrupt rate */
#define VDP_REG_MODESET_3       0x8B00      /* Mode register #3 */
#define VDP_REG_MODESET_4       0x8C00      /* Mode register #4 */
#define VDP_REG_HSCROLL_ADDR    0x8D00      /* HScroll data table address */
#define VDP_REG_AUTOINC         0x8F00      /* Autoincrement data */
#define VDP_REG_PLANE_SIZE      0x9000      /* Plane A and B size */
#define VDP_REG_WINDOW_XPOS     0x9100      /* Window X position */
#define VDP_REG_WINDOW_YPOS     0x9200      /* Window Y position */
#define VDP_REG_DMALEN_L        0x9300      /* DMA length (low) */
#define VDP_REG_DMALEN_H        0x9400      /* DMA length (high) */
#define VDP_REG_DMASRC_L        0x9500      /* DMA source (low) */
#define VDP_REG_DMASRC_M        0x9600      /* DMA source (mid) */
#define VDP_REG_DMASRC_H        0x9700      /* DMA source (high) */

/*
 * Base commands for the control port to do writes to the different VDP's rams
 */
#define VDP_VRAM_WRITE_CMD      0x40000000
#define VDP_CRAM_WRITE_CMD      0xC0000000
#define VDP_VSRAM_WRITE_CMD     0x40000010

/*
 * Base commands for the control port to do DMA writes to different VDP's rams
 */
#define VDP_DMA_VRAM_WRITE_CMD      0x40000080
#define VDP_DMA_CRAM_WRITE_CMD      0xC0000080
#define VDP_DMA_VSRAM_WRITE_CMD     0x40000090

#endif /* VDP_H */
