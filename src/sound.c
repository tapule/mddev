/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: sound.c
 * ??
 */

#include <stdint.h>
#include <stdalign.h>
#include "sound.h"
#include "z80.h"
#include "null_data.h"
#include "z80_xgm.h"
#include "kdebug.h"

/* 
 * XGM Sound driver has a sample table in the z80 RAM used to store sample
 * addresses and sizes. The first entry of this table (id 0) is used for the
 * silent sample (null_data can be used as silent sample).
 */
#define XGM_SAMPLE_ID_TABLE ((volatile uint8_t *) 0xA01C00)

/* XGM status address and statuses definition */
#define XGM_STATUS_ADDR ((volatile uint8_t *) 0xA00102)
#define XGM_STATUS_PLAYING  1
#define XGM_STATUS_READY    128


#define XGM_COMMAND_ADDR ((volatile uint8_t *) 0xA00100)
#define XGM_PARAMS_ADDR ((volatile uint8_t *) 0xA00104)


void sound_init(void)
{
    volatile uint8_t *smp_table = XGM_SAMPLE_ID_TABLE;
    uint32_t smp_null_addr = (uint32_t) null_data;
    uint16_t xgm_ready;

    z80_bus_request();

    /* Loads the xgm driver into the z80 memmory space */
    z80_data_load(z80_xgm, 0, Z80_XGM_SIZE);

    /* 
     * Set the silent sample address in the sample table. We use the null_data
     * array here as it is 256 bytes aligned as required for the xgm samples
     */
    smp_table[0] = smp_null_addr >> 8;
    smp_table[1] = smp_null_addr >> 16;
    /* Set the silent sample size in the sample id table*/    
    smp_table[2] = NULL_DATA_SIZE >> 8;
    smp_table[3] = NULL_DATA_SIZE >> 16;

    z80_reset();
    xgm_ready = *XGM_STATUS_ADDR & XGM_STATUS_READY;
    z80_bus_release();

    /* 
     * The XGM does some kind of initialization so we need to wait for it to
     * be ready. 
     */
    do
    {
        while (!z80_is_bus_free())
        {
        }
        /* Request the bus here to read the xgm status */
        z80_bus_request();
            xgm_ready = *XGM_STATUS_ADDR & XGM_STATUS_READY;
        z80_bus_release();
    } while (!xgm_ready);
}
