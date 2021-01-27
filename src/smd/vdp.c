/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Copyright: 2020 Juan Ángel Moreno Fernández (@_tapule)
 * Github: https://github.com/tapule/mddev
 *
 * File: vdp.c
 * Control routines for Sega Megadrive/Genesis VDP
 */

#include "vdp.h"

/* Ports to talk with the VDP. Can be accessed as 16bits or 32bits */
static volatile uint16_t *const vdp_port_ctrl_w = (uint16_t*) 0xC00004;
static volatile uint32_t *const vdp_port_ctrl_l = (uint32_t*) 0xC00004;
static volatile uint16_t *const vdp_port_data = (uint16_t*) 0xC00000;
static volatile uint16_t *const vdp_port_hv_counter = (uint16_t*) 0xC00008;

/*
 * The VDP has 24 registers (some of them not used) which control how video
 * hardware behaves. These registers are set by writing a control word to the
 * VDP control port where the high byte is as follow:
 *              | 1| 0| 0|R4|R3|R2|R1|R0|
 * R4-R0 is the register to write, so 0x81 (10000001) will write to the register
 * 0x01 or Mode register 2.
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
#define VDP_REG_WINDOW_HPOS     0x9100      /* Window H position */
#define VDP_REG_WINDOW_VPOS     0x9200      /* Window V position */
#define VDP_REG_DMALEN_L        0x9300      /* DMA length (low) */
#define VDP_REG_DMALEN_H        0x9400      /* DMA length (high) */
#define VDP_REG_DMASRC_L        0x9500      /* DMA source (low) */
#define VDP_REG_DMASRC_M        0x9600      /* DMA source (mid) */
#define VDP_REG_DMASRC_H        0x9700      /* DMA source (high) */

/* Stores if the VDP is working in PAL mode */
static uint8_t vdp_pal_mode;


void vdp_init(void)
{
    /*
     * We need to start reading the control port because it cancels whatever it
     * was doing and put it into a well known state.
     * At the same time, we use this read to save the PAL mode.
     */
    vdp_pal_mode = *vdp_port_ctrl_w & 0x01;

}
