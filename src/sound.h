/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: sound.h
 * ??
 *
 * ??
 *
 * More info:
 * 
 */

#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief initialises the sound system
 * 
 * Controls the sound system initialisation process using the XGM driver. It
 * loads the driver in the z80 memory space performs the initialization
 * proccess.
 * 
 * @note This function is called from the boot process so maybe you don't need
 * to call it anymore.
 */
void sound_init(void);

/**
 * @brief Manages sound synchronization
 * 
 * Handles the sound timing notifying the z80 in each frame.
 * 
 * @note This function is called automatically in the vint so you don't need to
 * call it. 
 */
void sound_update(void);

/**
 * @brief Adds a PCM sample to the XGM sample table
 * 
 * The XGM sample table can hold up to 255 samples. Sample id 0 is not allowed
 * and normally samples id < 64 are used by music, but you can use them if you
 * know what are you doing. The norm for SFX is using samples id [64..255].
 * By design the XGM driver needs its samples aligned to 256 bytes boundary with
 * a size multiple of 256.
 * 
 * @param id XGM sample table id to define
 * @param sample Sample address (must be aligned to 256 bytes boundary)
 * @param length Sample size in bytes (must be multiple of 256 bytes)
 * 
 * @note For optimization purposes, this function does not request the z80 bus
 * so be aware that you must request it in your code:
 *   z80_bus_request();
 *     sound_sfx_set(64, my_sfx_1, MY_SFX_1_SIZE);
 *     sound_sfx_set(65, my_sfx_2, MY_SFX_2_SIZE);
 *   z80_bus_release();
 */
void sound_sfx_set(const uint8_t id, const uint8_t *sample,
                   const uint32_t length);

/**
 * @brief Start playing a PCM sample on a specific channel
 * 
 * Plays a sample id previously defined with sound_sfx_set in the selected
 * channel. There are 4 channels available [0.. 3], but be aware that the first
 * one is usually used by music. The norm for SFX is using channels [1.. 3].
 * Priority is used to decide whether a sample which is playing in the channel
 * should be replaced with this one. If priority is higher than priority in the
 * current sample, it will be replaced with the new one.
 * 
 * @param id Sample id in the XGM sample table to play
 * @param priority Playing priority ranging from 0 (lowest) to 15 highest
 * @param channel Desired channel to play the sample
 */
void sound_sfx_play(const uint8_t id, uint8_t priority, const uint16_t channel);

/**
 * @brief Start playing a PCM sample autoselecting the channel
 * 
 * Acts like sound_sfx_play but selects the channel to use automatically.
 * Priority is used to decide whether a sample which is playing in the channel
 * should be replaced with this one. If priority is higher than priority in the
 * current sample, it will be replaced with the new one.
 * 
 * @param id Sample id in the XGM sample table to play
 * @param priority Playing priority ranging from 0 (lowest) to 15 highest
 */
void sound_sfx_play_auto(const uint8_t id, uint8_t priority);

/**
 * @brief Stop playing the PCM sample on a specific channel
 * 
 * @param channel Desired channel to stop
 */
void sound_sfx_stop(const uint16_t channel);

/**
 * @brief Mute PCM sounds
 * 
 * Stops all sample channels and prevent playing them
 */
void sound_sfx_mute(void);

/**
 * @brief Unmute PCM sounds
 */
void sound_sfx_unmute(void);

/**
 * @brief Checks whether PCM sounds are muted
 * 
 * @return True if PCM sounds are muted, false otherwise
 */
bool sound_sfx_is_muted(void);



#endif /* SOUND_H */
