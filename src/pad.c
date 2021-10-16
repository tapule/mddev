/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: pad.c
 * Control routines for Sega Megadrive/Genesis gamepads
 */ 

#include "pad.h"
#include "z80.h"

/* Gamepads data ports (DATAx) */
#define PAD_1_DATA_PORT     ((volatile uint8_t *) 0xA10003)
#define PAD_2_DATA_PORT     ((volatile uint8_t *) 0xA10005)
#define PAD_EXP_DATA_PORT   ((volatile uint8_t *) 0xA10007)

/* Gamepads control ports (CTRLx) */
#define PAD_1_CTRL_PORT     ((volatile uint8_t *) 0xA10009)
#define PAD_2_CTRL_PORT     ((volatile uint8_t *) 0xA1000B)
#define PAD_EXP_CTRL_PORT   ((volatile uint8_t *) 0xA1000D)

/* Gamepads types for later accesses */
static uint8_t pad_types[PAD_NUM] = {PAD_TYPE_UNKNOWN, PAD_TYPE_UNKNOWN};

/* 
 * Current and previous frame gamepads states
 * A 0 bit means not pressed, a 1 bit means pressed
 */
static uint16_t pad_state[PAD_NUM] = {0, 0};
static uint16_t pad_state_old[PAD_NUM] = {0, 0};

void pad_init(void)
{
    uint16_t i;

    /* Reset types and states */
    for (i = 0; i < PAD_NUM; ++i)
    {
        pad_types[i] = PAD_TYPE_UNKNOWN;
        pad_state[i] = 0;
        pad_state_old[i] = 0;
    }

    /*
     * To setup the controllers, we have to write 0x40 to each port. This will
     * use the TH pin of each control port as output an the remaining pins as
     * input.
     *                  7  6  5  4  3  2  1  0
     * Data/Ctrl pins: |?|TH|TL|TR| R| L| D| U|
     * Data: 0 means a button has been pressed, 1 if it has been released
     * CTrl: 0 means a pin on data port is an output, 1 if it is an input
     * The pin 7 is not connected so we can ignore it.
     * 
     * It is also important to halt the z80 while we are touching these ports to
     * prevent it to access the m68k's bus due to a hardware bug.
     *  
     */
    z80_bus_request_fast();
    *PAD_1_DATA_PORT = 0x40;
    *PAD_1_CTRL_PORT = 0x40;
    *PAD_2_DATA_PORT = 0x40;
    *PAD_2_CTRL_PORT = 0x40;
    *PAD_EXP_DATA_PORT = 0x40;
    *PAD_EXP_CTRL_PORT = 0x40;
    z80_bus_release();
}

void pad_update(void)
{
    pad_state_old[PAD_1] = pad_state[PAD_1];
    pad_state_old[PAD_2] = pad_state[PAD_2];

    z80_bus_request_fast();
    /* 1st step read:
     * | ?| ?| C| B| R| L| D| U|
     */
    *PAD_1_DATA_PORT = 0x40;
    *PAD_2_DATA_PORT = 0x40;
    __asm__ volatile ("\tnop\n");
    __asm__ volatile ("\tnop\n");
    pad_state[PAD_1] = *PAD_1_DATA_PORT & 0x3F;
    pad_state[PAD_2] = *PAD_2_DATA_PORT & 0x3F;
    /* 2nd step read:
     * | ?| ?|St| A| 0| 0| D| U|
     */
    *PAD_1_DATA_PORT = 0x00;
    *PAD_2_DATA_PORT = 0x00;
    __asm__ volatile ("\tnop\n");
    __asm__ volatile ("\tnop\n");
    /* Rearrange into |St| A| C| B| R| L| D| U| */
    pad_state[PAD_1] |= ((*PAD_1_DATA_PORT & 0x30) << 2);
    pad_state[PAD_2] |= ((*PAD_2_DATA_PORT & 0x30) << 2);
    /* Steps 3rd, 4th and 5th:
     * Ignore results
     */
    *PAD_1_DATA_PORT = 0x40;
    *PAD_2_DATA_PORT = 0x40;
    __asm__ volatile ("\tnop\n");
    __asm__ volatile ("\tnop\n");
    *PAD_1_DATA_PORT = 0x00;
    *PAD_2_DATA_PORT = 0x00;
    __asm__ volatile ("\tnop\n");
    __asm__ volatile ("\tnop\n");
    *PAD_1_DATA_PORT = 0x40;
    *PAD_2_DATA_PORT = 0x40;
    __asm__ volatile ("\tnop\n");
    __asm__ volatile ("\tnop\n");
    /* 6th step read:
     * | ?| ?|St| A| 0| 0| 0| 0|
     * If bits 3-0 are 0 it's a 6-button gamepad, otherwhise it's a 3-button.
     */
    *PAD_1_DATA_PORT = 0x00;
    *PAD_2_DATA_PORT = 0x00;
    __asm__ volatile ("\tnop\n");
    __asm__ volatile ("\tnop\n");
    if ((*PAD_1_DATA_PORT & 0x0F) != 0x00)
    {
        pad_types[PAD_1] = PAD_TYPE_3BTN;
    }
    else
    {
        pad_types[PAD_1] = PAD_TYPE_6BTN;
        /* 7th step read:
         * | ?| ?| C| B| Md| X| Y| Z|
        */
        *PAD_1_DATA_PORT = 0x40;
        __asm__ volatile ("\tnop\n");
        __asm__ volatile ("\tnop\n");
        pad_state[PAD_1] |= ((*PAD_1_DATA_PORT & 0x0F) << 8);
    }
    if ((*PAD_2_DATA_PORT & 0x0F) != 0x00)
    {
        pad_types[PAD_2] = PAD_TYPE_3BTN;
    }
    else
    {
        pad_types[PAD_2] = PAD_TYPE_6BTN;
        /* 7th step read:
         * | ?| ?| C| B| Md| X| Y| Z|
        */
        *PAD_2_DATA_PORT = 0x40;
        __asm__ volatile ("\tnop\n");
        __asm__ volatile ("\tnop\n");
        pad_state[PAD_2] |= ((*PAD_2_DATA_PORT & 0x0F) << 8);
    }
    z80_bus_release();
    
    /* Invert the state bits to use 0 as not pressed, 1 as pressed */
    pad_state[PAD_1] ^= 0x0FFF;
    pad_state[PAD_2] ^= 0x0FFF;    
}

inline uint8_t pad_type(const uint16_t pad)
{
    return pad_types[pad];
}

bool pad_btn_state(const uint16_t pad, const uint16_t buttons)
{
	/* TODO: Change these if's for some assert system */
    if (pad > PAD_NUM)
    {
        return false;
    }
    return (pad_state[pad] & buttons);
}

bool pad_btn_pressed(const uint16_t pad, const uint16_t buttons)
{
    if (pad > PAD_NUM)
    {
        return false;
    }
    /* Pressed this frame and not pressed last frame */
    return ((pad_state[pad] & buttons) && !(pad_state_old[pad] & buttons));
 }

bool pad_btn_released(const uint16_t pad, const uint16_t buttons)
{
    if (pad > PAD_NUM)
    {
        return false;
    }
    /* Not pressed this frame and pressed last frame */
    return (!(pad_state[pad] & buttons) && (pad_state_old[pad] & buttons));
}
