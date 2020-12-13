/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Copyright: 2020 Juan Ángel Moreno Fernández (@_tapule)
 * Github: https://github.com/tapule/mddev
 *
 * File: interrupts.s
 * Interrupt handlers
 * 
 * Response handlers for the more important interrupts on SMD
 */

 .section .text

.global _int_external
 _int_external:
    rte

.global _int_hblank
 _int_hblank:
    rte

.global _int_vblank
 _int_vblank:
    rte

.global _int_unhandled
 _int_unhandled:
    rte
