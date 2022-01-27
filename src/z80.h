/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
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

#ifndef MDDEV_Z80_H
#define MDDEV_Z80_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief initialises the z80 CPU.
 * 
 * Controls the initialisation process of the z80 CPU. It reset the z80 and
 * performs an internal RAM clear.
 * 
 * @note This function is called from the boot process so maybe you don't need
 * to call it anymore.
 */
void z80_init(void);

/**
 * @brief Reset the z80 CPU.
 * 
 * Forces the z80 to reset and start executing code from the first line of its
 * internal memory. This is done by cleaning the PC, I and R internal registers.
 */
void z80_reset(void);

/**
 * @brief Performs a z80 bus request.
 * 
 * We can't access the z80 memory while it's running so we need to ask it for
 * its bus. This will pause the z80 and grand us secure access to its RAM.
 */
void z80_bus_request(void);

/**
 * @brief Performs a fast z80 bus request.
 * 
 * Sometimes we don't need to access the z80 memory, but we need to pause it. In
 * these cases, we can ask the z80 for the bus, but there is no need to wait for
 * it to be ready. One example of this situation is doing safe DMA transfers.
 */
void z80_bus_request_fast(void);

/**
 * @brief Release the z80 bus.
 * 
 * Once we ended using the z80 bus, we must release it and let the CPU run
 * again.
 */
void z80_bus_release(void);

/**
 * @brief Checks if the z80 is halted and the bus is free
 * 
 * @return True if we own the bus, false if z80 is the owner
 */
bool z80_is_bus_free(void);

/**
 * @brief Load a chunk of data on the z80.
 * 
 * Uploads data to the z80 internal RAM.
 * 
 * @param src   Pointer to the source data.
 * @param dest  z80 RAM destination offset
 * @param size  Size in bytes of our data chunk.
 * 
 * @note This function does not request the bus, so be aware that it is a bit
 * unsafe if you don't manage the bus request/release in advance.
 */
void z80_data_load(const uint8_t *src, const uint16_t dest, uint16_t size);

/**
 * @brief Load a new program on the z80.
 * 
 * Uploads a program to the z80 internal RAM and reset it to start running the
 * code at the very beginning of this code.
 * 
 * @param src   Pointer to the source program data.
 * @param size  Size in bytes of our program data.
 */
void z80_program_load(const uint8_t *restrict src, uint16_t size);

#endif /* MDDEV_Z80_H */