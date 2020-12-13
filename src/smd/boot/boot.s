/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Copyright: 2020 Juan Ángel Moreno Fernández (@_tapule)
 * Github: https://github.com/tapule/mddev
 *
 * File: boot.s
 * Sega Megadrive/Genesis bootstrap code
 * 
 * At boot, the SMD has nothing initialised. We have to set up all pieces to
 * recover a clean state.
 * On top of that, we must do some extra tasks to use the C language.
 *
 * More info:
 * https://blog.bigevilcorporation.co.uk/2012/02/28/sega-megadrive-1-getting-started
 * https://www.namelessalgorithm.com/genesis/blog/genesis/
 */

.section .text.smdboot

.global _smd_boot
_smd_boot:
    move.w	#0x2700, sr             /* Disable ints */

   	move.b	0xa10001, d0            /* Check version, TMSS only on model 1+ */
	andi.b	#0x0F, d0               /* Version 0 = skip TMSS */
	beq	.skip_tmss
	move.l	#0x53454741, 0xa14000   /* Write 'SEGA' to TMSS register */

.skip_tmss:
 

    jmp	main
