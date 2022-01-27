/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: null_data.h
 * 256 zero bytes of data usable for any purpose. It is also aligned to 256
 * bytes boundary so it can be used as silent sample for the XGM sound driver. 
 */

#ifndef MDDEV_NULL_DATA_H
#define MDDEV_NULL_DATA_H

#include <stdint.h>

#define NULL_DATA_SIZE 256

extern const uint8_t null_data[NULL_DATA_SIZE];

#endif /* MDDEV_NULL_DATA_H */
