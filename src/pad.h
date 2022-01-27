/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: pad.h
 * Control routines for Sega Megadrive/Genesis gamepads
 *
 * The Sega Megadrive/Genesis has three I/O ports: CTRL1, CTRL2 and EXP. The
 * first two are dedicated to controllers while EXP is for modem support.
 * The generic controllers are the 3-button gamepads which include D-Pad, A, B,
 * C and Start buttons. But there are also newer 6-button gamepads which add
 * four more buttons X, Y, Z and Mode.
 *
 * More info:
 * https://www.plutiedev.com/io-ports
 * https://www.plutiedev.com/controllers
 * https://www.chibiakumas.com/68000/platform2.php#LessonP11
 */

#ifndef MDDEV_PAD_H
#define MDDEV_PAD_H

#include <stdint.h>
#include <stdbool.h>

/* Gamepad identifiers */
#define PAD_1       0x0000
#define PAD_2       0x0001
#define PAD_NUM     0x0002

/* Gamepad types */
#define PAD_TYPE_3BTN       0x00    /**< 3-buttons gamepad type */
#define PAD_TYPE_6BTN       0x01    /**< 6-buttons gamepad type */
#define PAD_TYPE_UNKNOWN    0x0F    /**< Unknown or not connected gamepad */

/* Gamepad buttons identifiers */
#define PAD_BTN_UP      0x0001
#define PAD_BTN_DOWN    0x0002
#define PAD_BTN_LEFT    0x0004
#define PAD_BTN_RIGHT   0x0008
#define PAD_BTN_B       0x0010
#define PAD_BTN_C       0x0020
#define PAD_BTN_A       0x0040
#define PAD_BTN_START   0x0080
#define PAD_BTN_Y       0x0200
#define PAD_BTN_Z       0x0100
#define PAD_BTN_X       0x0400
#define PAD_BTN_MODE    0x0800

/**
 * @brief Initialises the gamepad ports
 * 
 * Before using the gamepads, we must setup them first. This function controls
 * the detection and initialisation process of gamepads.
 * 
 * @note This function is called from the boot process so maybe you don't need
 * to call it anymore unless you want to reset the devices.
 */
void pad_init(void);

/**
 * @brief Updates the gamepads state
 * 
 * Each frame we must update the internal state of gamepads' buttons to control
 * the user's actions
 * 
 * @note This function must be called each frame
 */
void pad_update(void);

/**
 * @brief Gets the type for the specified gamepad id
 * 
 * @param pad Gamepad id to query
 * @return uint8_t Type of queried gamepad id
 */
uint8_t pad_type(const uint16_t pad);

/**
 * @brief Gets the current state of a set of buttons on a gamepad
 * 
 * Checks if a set of buttons on the given gamepad are currently pressed down
 * from the last update.
 * 
 * @param pad Gamepad id to query
 * @param buttons Set of buttons to query
 * @return true If the set of buttons are currently pressed, false otherwise
 */
bool pad_btn_state(const uint16_t pad, const uint16_t buttons);

/**
 * @brief Gets if a set of buttons were just pressed down on a gamepad
 * 
 * It checks if the set of buttons on the given gamepad were just pressed down
 * in the current frame.
 * 
 * @param pad Gamepad id to query
 * @param buttons Set of buttons to query
 * @return true If the set of buttons were currently pressed, false otherwise
 */
bool pad_btn_pressed(const uint16_t pad, const uint16_t buttons);

/**
 * @brief Gets if a set of buttons were just released on a gamepad
 * 
 * It checks if the set of buttons on the given gamepad were just released in
 * the current frame.
 * 
 * @param pad Gamepad id to query
 * @param buttons Set of buttons to query
 * @return true If the set of buttons were currently released, false otherwise
 */
bool pad_btn_released(const uint16_t pad, const uint16_t buttons);

#endif /* MDDEV_PAD_H */
