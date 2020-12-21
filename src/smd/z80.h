/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Copyright: 2020 Juan Ángel Moreno Fernández (@_tapule)
 * Github: https://github.com/tapule/mddev
 *
 * File: z80.h
 * Control routines for the Zilog Z80 CPU
 *
 * The secondary Z80 CPU in the Sega Megadrive/Genesis, is used to handle the
 * sound hardware and releasing the m68k from these tasks.
 *
 * More info:
 * https://www.plutiedev.com/using-the-z80
 */

#ifndef Z80_H
#define Z80_H

#include <stdint.h>

void z80_init(void);

void z80_bus_request(void);

void z80_bus_request_fast(void);

void z80_bus_release(void);

void z80_reset_start(void);

void z80_reset_end(void);

void z80_program_load(const uint8_t *src, uint16_t size);

#endif // Z80_H