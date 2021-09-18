/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: dma.c
 * VDP's Direct Memory Access operations
 */

#include "dma.h"
#include "config.h"
#include "vdp.h"
#include "z80.h"

/* Defines a DMA queue command operation */
typedef struct dma_command
{
    uint16_t autoinc;       /* Autoincrement register in bytes */
    uint16_t length_h;      /* Length register (high) in words */
    uint16_t length_l;      /* Length register (low) in words */
    uint16_t addr_h;        /* Source address register (high) in words */
    uint16_t addr_m;        /* Source address register (middle) in words */
    uint16_t addr_l;        /* Source address register (low) in words */
    uint16_t ctrl_addr_h;   /* VDP command with the destination address */
    uint16_t ctrl_addr_l;   /* VDP command (low). Start transfer */
} dma_command_t;

/* DMA commands queue and position index */
static dma_command_t dma_queue[DMA_QUEUE_SIZE];
static uint16_t dma_queue_index;

/**
 * @brief Builds a VDP ctrl port write address set command
 * 
 * @param xram_addr VRAM/CRAM/VSRAM DMA address base command
 * @param dest Destination ram address
 * @return uint32_t Ctrl port write address command
 */
static inline uint32_t dma_ctrl_addr_build(const uint32_t xram_addr,
                                           const uint32_t dest)
{
    return (((uint32_t)(xram_addr)) | (((uint32_t)(dest) & 0x3FFF) << 16) |
            ((uint32_t)(dest) >> 14));
}

/**
 * @brief Executes a DMA transfer from RAM/ROM to VRAM/CRAM/VSRAM without
 *        checking 128kB boundaries
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on VRAM/CRAM/VSRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @param xram_addr VRAM/CRAM/VSRAM DMA address base command
 * @return true Always, for compatibility with the other set of functions
 */
bool dma_transfer_fast(const uint32_t src, const uint16_t dest,
                       const uint16_t length, const uint16_t increment,
                       const uint32_t xram_addr)
{
    /* Used to issue the dma from a ram space */
    volatile uint32_t cmd;
    uint16_t *cmd_p = (uint16_t *) &cmd;

    /* Prevent VDP corruption waiting for a running DMA copy/fill operation */
    dma_wait();

    /* Sets the autoincrement on word writes */
    *VDP_PORT_CTRL_W = VDP_REG_AUTOINC | increment;
    /* Sets the DMA length in words */
    *VDP_PORT_CTRL_W = VDP_REG_DMALEN_L | (length & 0xFF);
    *VDP_PORT_CTRL_W = VDP_REG_DMALEN_H | ((length >> 8) & 0xFF);    
    /*
     * Sets the DMA source address. An additional lshift is needed to convert
     * src from bytes to words
     */
    *VDP_PORT_CTRL_W = VDP_REG_DMASRC_L | ((src >> 1) & 0xFF);
    *VDP_PORT_CTRL_W = VDP_REG_DMASRC_M | ((src >> 9) & 0xFF);
    *VDP_PORT_CTRL_W = VDP_REG_DMASRC_H | ((src >> 17) & 0x7F);
    /* Builds the ctrl port write address command in a ram variable */
    cmd = dma_ctrl_addr_build(xram_addr, dest);
    /* Issues the DMA from a ram varible and in words (see SEGA notes on DMA) */ 
    *VDP_PORT_CTRL_W = *cmd_p;
    ++cmd_p;
    z80_bus_request_fast();
    *VDP_PORT_CTRL_W = *cmd_p;
    z80_bus_release();

    return true;
}

/**
 * @brief Executes a DMA transfer from RAM/ROM to VRAM/CRAM/VSRAM checking 128kB
 *        boundaries
 * 
 * When a transfer from RAM/ROM crosses a 128KB boundary, it must be split in
 * two halves due to a bug in the VDP's DMA
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on VRAM/CRAM/VSRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @param xram_addr VRAM/CRAM/VSRAM DMA address base command
 * @return true On success, false otherwise
 */
bool dma_transfer(const uint32_t src, const uint16_t dest, uint16_t length,
                  const uint16_t increment, const uint32_t xram_addr)
{
    uint32_t bytes_to_128k;
    uint32_t words_to_128k;

    if (increment < 2 || length == 0)
    {
        return false;
    }

    /*
     * We need to control transfers which cross 128kB boundaries due to a bug in
     * the VDP's DMA. If a transfer crosses a 128kB boundary, the data that
     * crosses the limit will be random, that is, garbage data.
     * If the data crosses the limit, we need to split the transfer in two
     * halves.There is no need to do more than two transfers as the VDPS's
     * maximum ram (vram, cram, vsram) size is 64kB.
     */
    /* How many bytes there are until the next 128k jump */
    bytes_to_128k = 0x20000 - (src & 0x1FFFF);
    /* How many words there are until the next 128k jump */
    words_to_128k = 0x20000 - (src & 0x1FFFF);
    if (length > words_to_128k)
    {
        /* Does a fast transfer of second half */
        dma_transfer_fast(src + bytes_to_128k, dest + bytes_to_128k,
                          length - words_to_128k, increment, xram_addr);
        length = words_to_128k;
    }
    /* Does a fast transfer here (first half if we splited) */
    dma_transfer_fast(src, dest, length, increment, xram_addr);
    return true;
}

/**
 * @brief Pushes a DMA transfer operation from RAM/ROM to VRAM/CRAM/VSRAM into
 *        the DMA's queue without checking 128kB boundaries
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on VRAM/CRAM/VSRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @param xram_addr VRAM/CRAM/VSRAM DMA address base command
 */
void dma_queue_push_fast(const uint32_t src, const uint16_t dest,
                         const uint16_t length, const uint16_t increment,
                         const uint32_t xram_addr)
{
    dma_command_t *cmd;
    uint32_t *ctrl_addr_p;

    cmd = &dma_queue[dma_queue_index];
    ctrl_addr_p = (uint32_t *) &(cmd->ctrl_addr_h);

    /* Sets the autoincrement on word writes */
    cmd->autoinc = VDP_REG_AUTOINC | increment;
    /* Sets the DMA length in words */
    cmd->length_l = VDP_REG_DMALEN_L | (length & 0xFF);
    cmd->length_h = VDP_REG_DMALEN_H | ((length >> 8) & 0xFF);    
    /*
     * Sets the DMA source address. An additional lshift is needed to convert
     * src from bytes to words
     */
    cmd->addr_l = VDP_REG_DMASRC_L | ((src >> 1) & 0xFF);
    cmd->addr_m = VDP_REG_DMASRC_M | ((src >> 9) & 0xFF);
    cmd->addr_h = VDP_REG_DMASRC_H | ((src >> 17) & 0x7F);
    /* Builds the ctrl port write address command in a ram variable */
    *ctrl_addr_p = dma_ctrl_addr_build(xram_addr, dest);
    /* Advances the queue slot index */
    ++dma_queue_index;
}

/**
 * @brief Pushes a DMA transfer operation from RAM/ROM to VRAM/CRAM/VSRAM into
 *        the DMA's queue checking 128kB boundaries
 * 
 * When a transfer operation from RAM/ROM crosses a 128KB boundary, it is
 * splitted in two halves due to a bug in the VDP's DMA. Two DMA commands are
 * pushed to the queue.
 * 
 * @param src Source address on RAM/ROM space
 * @param dest Destination address on VRAM/CRAM/VSRAM
 * @param length Transfer length in words
 * @param increment Write position increment after each write (normally 2)
 * @param xram_addr VRAM/CRAM/VSRAM DMA address base command
 * @return true On success, false otherwise
 */
bool dma_queue_push(const uint32_t src, const uint16_t dest, uint16_t length,
                    const uint16_t increment, const uint32_t xram_addr)
{
    uint32_t bytes_to_128k;
    uint32_t words_to_128k;

    if (increment < 2 || length == 0 || (dma_queue_index >= DMA_QUEUE_SIZE))
    {
        return false;
    }

    /*
     * We need to control transfers which cross 128kB boundaries due to a bug in
     * the VDP's DMA. If a transfer crosses a 128kB boundary, the data that
     * crosses the limit will be random, that is, garbage data.
     * If the data crosses de limit, we need to split the transfer in two
     * halves.There is no need to do more than two transfers as the VDPS's
     * maximum ram (vram, cram, vsram) size is 64kB.
     */
    /* How many bytes there are until the next 128k jump */
    bytes_to_128k = 0x20000 - (src & 0x1FFFF);
    /* How many words there are until the next 128k jump */
    words_to_128k = 0x20000 - (src & 0x1FFFF);
    if (length > words_to_128k)
    {
        /* There is at least space for one commad, but we need two */
        if ((dma_queue_index + 1) >= DMA_QUEUE_SIZE)
        {
            return false;
        }
        /* Pushes a transfer command of second half */
        dma_queue_push_fast(src + bytes_to_128k, dest + bytes_to_128k,
                            length - words_to_128k, increment, xram_addr);
        length = words_to_128k;
    }
    /* Pushes transfer command here (first half if we splited) */
    dma_queue_push_fast(src, dest, length, increment, xram_addr);
    return true;
}

inline void dma_init(void)
{
    dma_queue_index = 0;
}

inline void dma_wait(void)
{
    /* Checks the DMA in progress flag in status register */
    while (*VDP_PORT_CTRL_L & 0x10)
    {
        __asm__ volatile ("\tnop\n");
    }
}

inline bool dma_vram_transfer(const void *restrict src, const uint16_t dest,
                              const uint16_t length, const uint16_t increment)
{
    return dma_transfer((uint32_t) src, dest, length, increment,
                        VDP_DMA_VRAM_WRITE_CMD);
}

inline bool dma_cram_transfer(const void *restrict src, const uint16_t dest,
                              const uint16_t length, const uint16_t increment)
{
    return dma_transfer((uint32_t) src, dest, length, increment,
                        VDP_DMA_CRAM_WRITE_CMD);
}

inline bool dma_vsram_transfer(const void *restrict src, const uint16_t dest,
                               const uint16_t length, const uint16_t increment)
{
    return dma_transfer((uint32_t) src, dest, length, increment,
                        VDP_DMA_VSRAM_WRITE_CMD);
}

inline bool dma_vram_transfer_fast(const void *restrict src,
                                   const uint16_t dest, const uint16_t length,
                                   const uint16_t increment)
{
    return dma_transfer_fast((uint32_t) src, dest, length, increment,
                             VDP_DMA_VRAM_WRITE_CMD);
}

inline bool dma_cram_transfer_fast(const void *restrict src,
                                   const uint16_t dest, const uint16_t length,
                                   const uint16_t increment)
{
    return dma_transfer_fast((uint32_t) src, dest, length, increment,
                             VDP_DMA_CRAM_WRITE_CMD);
}

inline bool dma_vsram_transfer_fast(const void *restrict src,
                                    const uint16_t dest, const uint16_t length,
                                    const uint16_t increment)
{
    return dma_transfer_fast((uint32_t) src, dest, length, increment,
                             VDP_DMA_VSRAM_WRITE_CMD);
}

bool dma_vram_fill(const uint16_t dest, uint16_t length,
                   const uint8_t value, const uint16_t increment)
{
    if (length < 2)
    {
        return false;
    }
    /*
     * In a DMA fill operation, the first write writes an entire word instead of
     * a byte. Then, in each write a byte is written. Therefore, a length of 1
     * will write 2 bytes, a length of 2 will write 3 bytes, etc.
     * We need to do an initial decrement to length.
     *      value = 0xFF
     *      length = 2 -> length = 1 -> FF FF
     *      length = 3 -> length = 2 -> FF FF FF
     *      length = 4 -> length = 3 -> FF FF FF FF
     */
    --length;

    /* Prevent VDP corruption waiting for a running DMA copy/fill operation */
    dma_wait();

    /* Sets the autoincrement after each write */
    *VDP_PORT_CTRL_W = VDP_REG_AUTOINC | increment;
    /* Sets the DMA length in bytes */
    *VDP_PORT_CTRL_W = VDP_REG_DMALEN_L | (length & 0xFF);
    *VDP_PORT_CTRL_W = VDP_REG_DMALEN_H | ((length >> 8) & 0xFF);    
    /* Sets the DMA operation to VRAM fill operation */
    *VDP_PORT_CTRL_W = VDP_REG_DMASRC_H | 0x80;
    /* Builds the ctrl port write address command */
    *VDP_PORT_CTRL_L = dma_ctrl_addr_build(VDP_DMA_VRAM_WRITE_CMD, dest);
    /* Set fill value. The high byte must be equal for the first write */
    *VDP_PORT_DATA_W = (value << 8) | value;
    return true;
}

inline uint16_t dma_queue_size(void)
{
    return dma_queue_index;
}

inline void dma_queue_clear(void)
{
    dma_queue_index = 0;
}

void dma_queue_flush(void)
{
    uint32_t *queue_p = (uint32_t *) dma_queue;
    uint16_t i;

    z80_bus_request_fast();
    for (i = 0; i < dma_queue_index; ++i)
    {
        /*
         * Sets the autoincrement on word writes and the high part of the DMA
         * length in words
         */
        *VDP_PORT_CTRL_L = *queue_p++;
        /*
         * Sets the low part of the DMA length in words and the high part of
         * source address
         */
        *VDP_PORT_CTRL_L = *queue_p++;
        /* Sets the middle and low part of the DMA source address */
        *VDP_PORT_CTRL_L = *queue_p++;
        /* Issues the DMA from ram space and in words (see SEGA notes on DMA) */ 
        *VDP_PORT_CTRL_W = *queue_p >> 16;
        *VDP_PORT_CTRL_W = *queue_p++;
    }
    z80_bus_release();
    dma_queue_clear();
}

bool dma_queue_vram_transfer(const void *restrict src, const uint16_t dest,
                             const uint16_t length, const uint16_t increment)
{
    return dma_queue_push((uint32_t) src, dest, length, increment,
                          VDP_DMA_VRAM_WRITE_CMD);
}

bool dma_queue_cram_transfer(const void *restrict src, const uint16_t dest,
                             const uint16_t length, const uint16_t increment)
{
    return dma_queue_push((uint32_t) src, dest, length, increment,
                          VDP_DMA_CRAM_WRITE_CMD);
}

bool dma_queue_vsram_transfer(const void *restrict src, const uint16_t dest,
                              const uint16_t length, const uint16_t increment)
{
    return dma_queue_push((uint32_t) src, dest, length, increment,
                          VDP_DMA_VSRAM_WRITE_CMD);
}
