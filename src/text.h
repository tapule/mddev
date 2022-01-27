/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: text.h
 * Text rendering functions
 *
 * Provides basic functions to render text to a buffer based on a font tileset
 * in VRAM.
 * It is recommended that font tilesets contain the ASCII glyphs between 32-95
 * (64 glyphs) or the ASCII glyphs between 32-127 (96 glyphs):
 *          !"#$%&'()*+,-./0123456789:;<=>?
 *          @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
 *          `abcdefghijklmnopqrstuvwxyz{|}~␡
 *
 */

#ifndef MDDEV_TEXT_H
#define MDDEV_TEXT_H

#include <stdint.h>

/**
 * @brief Sets the font tileset starting index in VRAM
 * 
 * @param tileset_index Starting index of font tileset to use when writing text
 */
void text_font_set(const uint16_t tileset_index);

/**
 * @brief Sets the palette to use for text rendering
 * 
 * @param palette CRam palete index (0..3)
 */
void text_pal_set(const uint16_t palette);

/**
 * @brief Sets the drawing priority to use for text rendering
 * 
 * @param priority Drawing priority (0 low priority, 1 high priority)
 */
void text_priority_set(const uint16_t priority);

/**
 * @brief Writes an ASCII string as font tiles in a buffer using the current
 *        text configuration.
 * 
 * @param str Source ASCII string
 * @param dest Destination buffer
 * @return uint16_t Total written cells (glyphs) in the buffer
 * 
 * @note Destination buffer must have enough space. It writes until the
 * terminating null byte ('\0') from the source string.
 */
uint16_t text_render(const char *str, uint16_t *dest);

/**
 * @brief Writes up to size ASCII characters from the source string as font
 *        tiles in a buffer using the current text configuration.
 * 
 * @param str Source ASCII string
 * @param dest Destination buffer
 * @param size Number of characters to wirte
 * @return uint16_t Total written cells (glyphs) in the buffer
 * 
 * @note Destination buffer must have enough space. It writes until the
 * terminating null byte ('\0') from the source string.
 */
uint16_t text_nrender(const char *str, uint16_t *dest, const uint16_t size);

#endif /* MDDEV_TEXT_H */
