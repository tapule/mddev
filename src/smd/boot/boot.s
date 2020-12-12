/**
 * @file header.s
 * @author Tapule
 * @brief Sega Megadrive/Genesis bootstrap code
 *
 * @see https://blog.bigevilcorporation.co.uk/2012/03/09/sega-megadrive-3-awaking-the-beast/
 * @see https://darkdust.net/writings/megadrive/initializing
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
