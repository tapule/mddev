/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: dma.h
 * VDP's Direct Memory Access operations
 *
 * Althought you can write the Sega Megadrive/Genesis VDP's video memory using
 * the m68k directly, the VDP's DMA capabilities let you transfer large blocks
 * of memory (graphics) directly without the CPU's help.
 * DMA operations are faster during the vertical blanking or when the display is
 * disabled.
 *
 * More info:
 * https://www.plutiedev.com/dma-transfer
 * https://github.com/Stephane-D/SGDK/blob/master/inc/dma.h
 */

#ifndef MDDEV_DMA_H
#define MDDEV_DMA_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialises the DMA system
 * 
 * @note This function is called from the boot process so maybe you don't need
 * to call it anymore unless you want to reset the DMA system.
 */
void dma_init(void);

/**
 * @brief Wait for a running DMA copy/fill operation to finish
 * 
 */
void dma_wait(void);

/**
 * @brief Executes a DMA transfer from RAM/ROM to VRAM
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on VRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @return True on success, false otherwise
 */
bool dma_vram_transfer(const void *restrict src, const uint16_t dest,
                       const uint16_t length, const uint16_t increment);

/**
 * @brief Executes a DMA transfer from RAM/ROM to CRAM
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on CRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @return True on success, false otherwise
 */
bool dma_cram_transfer(const void *restrict src, const uint16_t dest,
                       const uint16_t length, const uint16_t increment);

/**
 * @brief Executes a DMA transfer from RAM/ROM to VSRAM
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on VSRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @return True on success, false otherwise
 */
bool dma_vsram_transfer(const void *restrict src, const uint16_t dest,
                        const uint16_t length, const uint16_t increment);

/**
 * @brief Executes a fast DMA transfer from RAM/ROM to VRAM
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on VRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @return True on success, false otherwise
 * 
 * @note Parameters or 128kB boundaries are not checked, so be aware that it is
 * a bit unsafe if you don't know what you are doing.
 */
bool dma_vram_transfer_fast(const void *restrict src, const uint16_t dest,
                            const uint16_t length, const uint16_t increment);

/**
 * @brief Executes a fast DMA transfer from RAM/ROM to CRAM
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on CRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @return True on success, false otherwise
 * 
 * @note Parameters or 128kB boundaries are not checked, so be aware that it is
 * a bit unsafe if you don't know what you are doing.
 */
bool dma_cram_transfer_fast(const void *restrict src, const uint16_t dest,
                            const uint16_t length, const uint16_t increment);

/**
 * @brief Executes a fast DMA transfer from RAM/ROM to VSRAM
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on VSRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @return True on success, false otherwise
 * 
 * @note Parameters or 128kB boundaries are not checked, so be aware that it is
 * a bit unsafe if you don't know what you are doing.
 */
bool dma_vsram_transfer_fast(const void *restrict src, const uint16_t dest,
                             const uint16_t length, const uint16_t increment);

/**
 * @brief Executes a DMA VRAM fill operation
 * 
 * @param dest Destination address on VRAM
 * @param length Transfer length in bytes, minimum 2
 * @param value Value used to fill the vram
 * @param increment Write position increment after each write (normally 1)
 * @return True on success, false otherwise
 * 
 * @note The DMA VRAM fill operation does not stop the m68k, so it is a good
 * idea to use it with dma_wait() function to wait for it to finish the fill
 * operation.
 */
bool dma_vram_fill(const uint16_t dest, uint16_t length,
                   const uint8_t value, const uint16_t increment);

/**
 * @brief Returns the current DMA's queue command size
 * 
 * @return uint16_t Total DMA commands in the queue
 */
uint16_t dma_queue_size(void);

/**
 * @brief Resets the DMA's queue command
 * 
 */
void dma_queue_clear(void);

/**
 * @brief Executes all the pending DMA's commands in the queue and resets it
 * 
 */
void dma_queue_flush(void);

/**
 * @brief Adds a new DMA transfer from RAM/ROM to VRAM in the queue
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on VRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @return True on success, false if the queue is full
 */
bool dma_queue_vram_transfer(const void *restrict src, const uint16_t dest,
                             const uint16_t length, const uint16_t increment);

/**
 * @brief Adds a new DMA transfer from RAM/ROM to CRAM in the queue
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on CRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @return True on success, false if the queue is full
 */
bool dma_queue_cram_transfer(const void *restrict src, const uint16_t dest,
                             const uint16_t length, const uint16_t increment);

/**
 * @brief Adds a new DMA transfer from RAM/ROM to VSRAM in the queue
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on VSRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @return True on success, false if the queue is full
 */
bool dma_queue_vsram_transfer(const void *restrict src, const uint16_t dest,
                              const uint16_t length, const uint16_t increment);

#endif /* MDDEV_DMA_H */
