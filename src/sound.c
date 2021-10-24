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
#include "sys.h"
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

/* XGM commands */
#define XGM_COMMAND_ADDR ((volatile uint8_t *) 0xA00100)
#define XGM_COMMAND_PARAMS_ADDR ((volatile uint8_t *) 0xA00104)
#define XGM_COMMAND_PLAY    1
#define XGM_COMMAND_STOP    16

/* XGM variables for sound synchronisation */
/* Default reference music tempo in ticks per second: 60 */
static uint16_t xgm_tempo;
/* System music tempo in ticks per second: 60 in NTSC, 50 in PAL */
static uint16_t xgm_tempo_def;
/* Counter for music synchronization */
static int16_t xgm_tempo_cnt;

/* Next channel that sould be used for sfx */
static uint16_t sound_sfx_next_channel;
/* Is sfx muted? */
static bool sound_sfx_muted;


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

    /* Skip channel 0 for sfx as it is normally used for music */
    sound_sfx_next_channel = 1;
    sound_sfx_muted = false;
    /* Initialize XGM synchronisation variables */
    xgm_tempo = 60;
	xgm_tempo_def = smd_is_pal() ? 50 : 60;
    xgm_tempo_cnt = 0;
}

inline void sound_update(void)
{
    int16_t cnt = xgm_tempo_cnt;
    uint16_t step = xgm_tempo_def;
    uint16_t num = 0;

    /* 
     * Calculates number of pending frame depending on whether system is in PAL
     * or NTSC mode. In NTSC each frame notifies one tick to the z80 but in PAL
     * we must notify 60 ticks per second so every 5 frames notifies one more
     * tick (6 ticks every 5 fames = 60 ticks every 50 frames or one second)
     */
    while (cnt <= 0)
    {
        ++num;
        cnt += step;
    }
    xgm_tempo_cnt = cnt - xgm_tempo;

    while (true)
    {
        z80_bus_request();
        /* 
         * XGM MODIFYING_F (0x0E) variable controls whether the z80 is accessing
         * the PENDING_FRM (0x0F) variable or not. We check here if this is
         * happening and wait for the z80 to finish doing it.
         */
        if (!XGM_COMMAND_PARAMS_ADDR[0x0E])
        {
            break;
        }
        z80_bus_release();

        /* Wait a bit (about 80 cycles) */
        __asm__ volatile ("\t\tmovm.l %d0-%d3,-(%sp)\n");
        __asm__ volatile ("\t\tmovm.l (%sp)+,%d0-%d3\n");        
    }
    /* 
     * XGM PENDING_FRM (0x0F) variable contains number of XGM frame to process.
     * Increment it here.
     */
    XGM_COMMAND_PARAMS_ADDR[0x0F] += num;
}

void sound_sfx_set(const uint8_t id, const uint8_t *sample,
                   const uint32_t length)
{
    /* Get the sample id index in the sample table */
    volatile uint8_t *smp_table_ptr = (XGM_SAMPLE_ID_TABLE + (uint32_t) (id << 2));

    /* Set the sample address in the sample table index */
    smp_table_ptr[0] = ((uint32_t) sample) >> 8;
    smp_table_ptr[1] = ((uint32_t) sample) >> 16;
    /* Set the sample size in the sample table */    
    smp_table_ptr[2] = length >> 8;
    smp_table_ptr[3] = length >> 16;
}

void sound_sfx_play(const uint8_t id, uint8_t priority, const uint16_t channel)
{
    volatile uint8_t *pcm_params;

    if (!sound_sfx_muted)
    {
        smd_ints_disable();
        z80_bus_request();

        pcm_params = XGM_COMMAND_PARAMS_ADDR + 0x04 + (channel * 2);

        /* Set PCM priority and id */
        pcm_params[0] = priority & 0xF;
        pcm_params[1] = id;

        /* Set play PCM channel command */
        *XGM_COMMAND_ADDR |= (XGM_COMMAND_PLAY << channel);

        z80_bus_release();
        smd_ints_enable();

        /* Adjust play auto next channel skipping channel 0 */
        sound_sfx_next_channel = channel + 1;
        if (sound_sfx_next_channel > 3)
        {
            sound_sfx_next_channel = 1;
        }
    }
}

inline void sound_sfx_play_auto(const uint8_t id, uint8_t priority)
{
    sound_sfx_play(id, priority, sound_sfx_next_channel);
}

inline void sound_sfx_stop(const uint16_t channel)
{
    /* Stop a channel is the same as playing silent sample on it */
    sound_sfx_play(0, 15, channel);
}

void sound_sfx_mute(void)
{
    int16_t i;

    if (!sound_sfx_muted)
    {
        /* Stop all sample channels first */
        for (i = 0; i < 4; ++i)
        {
            sound_sfx_stop(i);
        }
        sound_sfx_muted = true;
    }
}

inline void sound_sfx_unmute(void)
{
    sound_sfx_muted = false;
}

inline bool sound_sfx_is_muted(void)
{
    return sound_sfx_muted;
}