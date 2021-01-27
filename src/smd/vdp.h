/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Copyright: 2020 Juan Ángel Moreno Fernández (@_tapule)
 * Github: https://github.com/tapule/mddev
 *
 * File: vdp.h
 * Control routines for Sega Megadrive/Genesis VDP
 *
 * The VDP (Video Display Processor) is the main graphics processor, wich has
 * two scrollables graphics planes (A and B) and a static plane (W). It is also
 * capable of rendering up to 80 sprites on screen. Its main internal memory is
 * a 64KB used to store graphics and data. It has also a 128B memory for colors
 * called CRAM and a vertical scroll ram or VSRAM where the vertical scroll data
 * is stored.
 *
 * More info:
 * https://www.plutiedev.com/vdp-setup
 * https://www.plutiedev.com/vdp-registers
 * 
 */

#ifndef VDP_H
#define VDP_H

#include <stdint.h>

/**
 * @brief Initializes the VDP
 * 
 * Before using the VDP, we must setup it first. This function controls the
 * initialization, registers setup and VRAM/CRAM/VSRAM cleaning process. 
 * 
 * @note This function is called from the boot process so maybe you don't need
 * to call it anymore unless you want to reset the devices.
 */
void vdp_init(void);

void vdp_vsync_wait(void);

#endif // VDP_H
