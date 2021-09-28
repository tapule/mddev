/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: kdebug.c
 * Gens KMod Debugging routines 
 */

#include "kdebug.h"
#include "vdp.h"

void __kdebug_halt(void)
{
    /* Pause game command */
    *VDP_PORT_CTRL_W = VDP_REG_KMOD_CONTROL | 0x00;
}

void __kdebug_alert(const char *str)
{
    const char *c;
    c = str;

    /* We need to write string byte by byte */
    while (*c)
    {
        *VDP_PORT_CTRL_W = VDP_REG_KMOD_MESSAGE | *c;
        ++c;
    }
    /* Session ends with a 0 */
    *VDP_PORT_CTRL_W = VDP_REG_KMOD_MESSAGE | 0x00;
}

void __kdebug_timer_start(void)
{
    /* Start emulator timer command */
    *VDP_PORT_CTRL_W = VDP_REG_KMOD_TIMER | 0x80;
}

void __kdebug_timer_stop(void)
{
    /* Stop and output emulator timer command */
    *VDP_PORT_CTRL_W = VDP_REG_KMOD_TIMER | 0x40;
}

void __kdebug_timer_output(void)
{
    /* Output emulator timer command */
    *VDP_PORT_CTRL_W = VDP_REG_KMOD_TIMER | 0x00;
}
