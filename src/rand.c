/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: rand.c
 * Control routines for Pseudo-Random Numbers Generation
 */

#include "rand.h"
#include "vdp.h"

/* Stores the lastest (current) calculated seed */
static uint16_t rnd_seed;

void rnd_init(void)
{
    uint16_t rnd_var;

    /* CHECKME: Seems that this is not so random */
    /* Mix a random generated value with the MegaDrive HV counter */
    rnd_var = (uint16_t) 0xCE52 ^ (uint16_t) (0xCE52 << 9);
    rnd_seed = *VDP_PORT_HV_COUNTER ^ (*VDP_PORT_HV_COUNTER >> 7);
    rnd_seed = rnd_seed ^ rnd_var ^ (rnd_var << 13);
}

void rnd_seed_set(const uint16_t seed)
{
    /* seed must be a non-zero value */
    if (seed == 0)
    {
        rnd_init();
    }
    rnd_seed = seed;
}

inline uint16_t rnd_seed_get(void)
{
    return rnd_seed;
}

uint16_t rnd_get(void)
{
    /* Xorshift algorithm */
    rnd_seed ^= rnd_seed << 7;
    rnd_seed ^= rnd_seed >> 9;
    rnd_seed ^= rnd_seed << 8;

    return rnd_seed;
}
