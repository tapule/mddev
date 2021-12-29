/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: video.c
 * Control routines for Sega Megadrive/Genesis VDP
 */

#include "video.h"
#include "vdp.h"

/* Stores if the console is working in PAL mode */
static uint8_t pal_mode_flag;

/* This flag is set when the vertical blank starts */
volatile uint8_t vid_vblank_flag;

void vid_init(void)
{
    /*
     * We need to start reading the control port because it cancels whatever it
     * was doing and put it into a well known state.
     * At the same time, we use this read to save the PAL mode.
     */
    pal_mode_flag = *VDP_PORT_CTRL_W & 0x01;

    /* Initialise the VDP register */
    /* H interrupt off, HV counter on */
    *VDP_PORT_CTRL_W = VDP_REG_MODESET_1 | 0x04;
    /* Display off, V interrupt on, DMA on, V30 cells mode in pal, V28 ntsc  */
    *VDP_PORT_CTRL_W = VDP_REG_MODESET_2 | 0x34 | (pal_mode_flag ? 8 : 0);
    /* Plane A table address (divided by 0x2000 and lshifted 3 = rshift 10 ) */
    *VDP_PORT_CTRL_W = VDP_REG_PLANEA_ADDR | (VID_PLANE_A_ADDR >> 10);
    /* Plane W table address (divided by 0x800 and lshifted 1 = rsifht 10) */
    *VDP_PORT_CTRL_W = VDP_REG_WINDOW_ADDR | (VID_PLANE_W_ADDR >> 10);
    /* Plane B table address (divided by 0x2000 = rsifht 13) */
    *VDP_PORT_CTRL_W = VDP_REG_PLANEB_ADDR | (VID_PLANE_B_ADDR >> 13);
    /* Sprite table address (divided by 0x200 = rsifht 9) */
    *VDP_PORT_CTRL_W = VDP_REG_SPRITE_ADDR | (VID_SPRITE_TABLE_ADDR >> 9);
    /* Background color: palette 0, color 0 */
    *VDP_PORT_CTRL_W = VDP_REG_BGCOLOR | 0x00;
    /* H interrupt frequency in raster lines (As we disabled it, set maximum) */
    *VDP_PORT_CTRL_W = VDP_REG_HBLANK_RATE | 0xFF;
    /* External interrupt off, V scroll, H scroll */
    *VDP_PORT_CTRL_W = VDP_REG_MODESET_3 | VID_VSCROLL_MODE | VID_HSCROLL_MODE;
    /* H40 cells mode, shadows and highlights off, interlace mode off */
    *VDP_PORT_CTRL_W = VDP_REG_MODESET_4 | 0x81;
    /* H Scroll table address (divided by 0x400 = rsifht 10) */
    *VDP_PORT_CTRL_W = VDP_REG_HSCROLL_ADDR | (VID_HSCROLL_TABLE_ADDR >> 10);
    /* Auto increment in bytes for the VDP's address reg after read or write */
    *VDP_PORT_CTRL_W = VDP_REG_AUTOINC | 0x02;
    /* Scroll size (planes A and B size) */
    *VDP_PORT_CTRL_W = VDP_REG_PLANE_SIZE | VID_PLANE_SIZE;
    /* Window plane X position (no window) */
    *VDP_PORT_CTRL_W = VDP_REG_WINDOW_XPOS | 0x00;
    /* Window plane Y position (no window) */
    *VDP_PORT_CTRL_W = VDP_REG_WINDOW_YPOS | 0x00;

    /* Clean the VDP's rams */
    vid_vram_clear();
    vid_cram_clear();
    vid_vsram_clear();
}

inline void vid_display_enable(void)
{
    *VDP_PORT_CTRL_W = VDP_REG_MODESET_2 | 0x74 | (pal_mode_flag ? 8 : 0);
}

inline void vid_display_disable(void)
{
    *VDP_PORT_CTRL_W = VDP_REG_MODESET_2 | 0x34 | (pal_mode_flag ? 8 : 0);
}

void vid_vsync_wait(void)
{
    /* Set the vblak flag to 0 and wait for the vblank interrupt to change it */
    vid_vblank_flag = 0;
    while (!vid_vblank_flag)
    {
        __asm__ volatile ("\tnop\n");
    }
    vid_vblank_flag = 0;
}

void vid_vram_clear(void)
{
    uint16_t i;

    *VDP_PORT_CTRL_L = VDP_VRAM_WRITE_CMD;

    for (i = 0; i < (65536 / 4); ++i)
    {
        *VDP_PORT_DATA_L = 0x00;
    }
}

void vid_cram_clear(void)
{
    uint16_t i;

    *VDP_PORT_CTRL_L = VDP_CRAM_WRITE_CMD;

    for (i = 0; i < (128 / 4); ++i)
    {
        *VDP_PORT_DATA_L = 0;
    }
}

void vid_vsram_clear(void)
{
    uint16_t i;

    *VDP_PORT_CTRL_L = VDP_VSRAM_WRITE_CMD;

    for (i = 0; i < (80 / 4); ++i)
    {
        *VDP_PORT_DATA_L = 0;
    }
}

inline void vid_background_color_set(const uint8_t index)
{
    *VDP_PORT_CTRL_W = VDP_REG_BGCOLOR | index;
}

inline void vid_scroll_mode_set(const vid_hscroll_mode_t hscroll_mode,
                                const vid_vscroll_mode_t vscroll_mode)
{
    *VDP_PORT_CTRL_W = VDP_REG_MODESET_3 | vscroll_mode | hscroll_mode;
}

inline void vid_plane_size_set(const vid_plane_size_t size)
{
    *VDP_PORT_CTRL_W = VDP_REG_PLANE_SIZE | size;
}

inline void vid_autoinc_set(const uint8_t increment)
{
    *VDP_PORT_CTRL_W = VDP_REG_AUTOINC | increment;
}
