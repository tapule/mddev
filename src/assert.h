/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021
 * Github: https://github.com/tapule/mddev
 *
 * File: assert.h
 * Compile time assert
 *
 * This file provides static_assert macro for _Static_assert keyword that
 * validates a predicate at compile time without generating any code.
 */

#ifndef MDDEV_CTASSERT_H
#define MDDEV_CTASSERT_H

#undef static_assert
#define static_assert _Static_assert

#endif /* MDDEV_CTASSERT_H */