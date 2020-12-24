/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Copyright: 2020 Juan Ángel Moreno Fernández (@_tapule)
 * Github: https://github.com/tapule/mddev
 *
 * File: z80.x
 * Control routines for the Zilog Z80 CPU
 */	

#include <stdint.h>
#include "z80.h"

/* Z80 control ports */
static volatile uint16_t *const z80_bus_port = (uint16_t *) 0xA11100;
static volatile uint16_t *const z80_reset_port = (uint16_t *) 0xA11200;

/* Z80 memory pointer and size (8KB) */
static const uint8_t *const z80_ram_address = (uint8_t*) 0xA00000;
static const uint16_t z80_ram_size = 0x2000;

static void z80_ram_clear(void)
{
    /* We need a 0 byte, not a word */
    const uint8_t zero = 0;

    uint8_t *dest = (uint8_t *) z80_ram_address;
    uint16_t size = z80_ram_size;

    /* We must access the Z80 RAM using bytes, words won't work */
    while(size--)
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
    *z80_reset_port = 0x000;
    /* We need to wait a while until the reset is done */
    while(wait--)
    {
    }
    /* Release the z80 reset line */    
    *z80_reset_port = 0x100;
}

void z80_bus_request(void)
{
    /* Request the bus */
    *z80_bus_port = 0x100;
    /* If there is a reset process, force it to end now */
    *z80_reset_port = 0x100;
    /* Wait until the bus is free */
    while(*z80_bus_port & 0x100)
    {
    }
}

void z80_bus_request_fast(void)
{
    *z80_bus_port = 0x100;
}

void z80_bus_release(void)
{
    *z80_bus_port = 0x000;
}

void z80_program_load(const uint8_t *src, uint16_t size)
{
    /* Copy program from the start of z80 internal RAM */
    uint8_t *dest = (uint8_t *) z80_ram_address;

    z80_bus_request();

    while(size--)
    {
        *dest = *src; 
        ++dest;
        ++src;
    }

    z80_reset();
    z80_bus_release();    
}
