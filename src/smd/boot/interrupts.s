/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: interrupts.s
 * Interrupt handlers
 * 
 * Response handlers for the most important interrupts on SMD
 */

 .section .text

.global _int_external
 _int_external:
    rte

.global _int_hblank
 _int_hblank:
    rte

/**
 * @brief Handler for the vertical blank interrupt
 * 
 * This handler is called whenever a 
 * 
 */
.global _int_vblank
 _int_vblank:
    st.b    (vid_vblank_flag)
    rte

.global _int_unhandled
 _int_unhandled:
    rte
