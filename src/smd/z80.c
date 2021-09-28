/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: z80.c
 * Control routines for the Zilog Z80 CPU
 */

#include "z80.h"

/* Z80 control ports */
#define Z80_BUS_PORT    ((volatile uint16_t *) 0xA11100)
#define Z80_RESET_PORT  ((volatile uint16_t *) 0xA11200)

/* Z80 memory pointer and size (8KB) */
#define Z80_RAM_ADDRESS     ((uint8_t *) 0xA00000)
#define Z80_RAM_SIZE        0x2000;

static void z80_ram_clear(void)
{
    /* We need a 0 byte, not a word */
    const uint8_t zero = 0;

    uint8_t *dest = Z80_RAM_ADDRESS;
    uint16_t size = Z80_RAM_SIZE;

    /* We must access the Z80 RAM using bytes, words won't work */
    while (size--)
    {
        *dest = zero; 
        ++dest;
    }
}

void z80_init(void)
{
    z80_bus_request();
    z80_ram_clear();
    /* Reset the z80 and let it run */
    z80_reset();
    z80_bus_release();
}

void z80_reset(void)
{
    /* Estimated wait time borrowed from Sik z80 tutorial */
    uint16_t wait = 0x20;

    /* Assert the z80 reset line */
    *Z80_RESET_PORT = 0x000;
    /* We need to wait a while until the reset is done */
    while (wait--)
    {
    }
    /* Release the z80 reset line */    
    *Z80_RESET_PORT = 0x100;
}

void z80_bus_request(void)
{
    /* Request the bus */
    *Z80_BUS_PORT = 0x100;
    /* If there is a reset process, force it to end now */
    *Z80_RESET_PORT = 0x100;
    /* The bus is busy until it retuns a 0x100 so we wait for it */
    while (*Z80_BUS_PORT & 0x100)
    {
    }
}

inline void z80_bus_request_fast(void)
{
    *Z80_BUS_PORT = 0x100;
}

inline void z80_bus_release(void)
{
    *Z80_BUS_PORT = 0x000;
}

bool z80_is_bus_free(void)
{
    if (*Z80_BUS_PORT & 0x100)
    {
        return true;
    }
    return false;
}

void z80_program_load(const uint8_t *src, uint16_t size)
{
    /* Copy program to the start of z80 internal RAM */
    uint8_t *dest = Z80_RAM_ADDRESS;

    z80_bus_request();

    while (size--)
    {
        *dest = *src; 
        ++dest;
        ++src;
    }

    z80_reset();
    z80_bus_release();    
}
