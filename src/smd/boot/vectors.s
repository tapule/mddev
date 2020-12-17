/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Copyright: 2020 Juan Ángel Moreno Fernández (@_tapule)
 * Github: https://github.com/tapule/mddev
 *
 * File: vectors.s
 * Sega Megadrive/Genesis m68k vector table
 * 
 * All SMD m68k programs must have this vector table at their very beginning.
 * Each entry (32bit address) is used by the m68k in concrete cases.
 *
 * More info:
 * https://blog.bigevilcorporation.co.uk/2012/02/28/sega-megadrive-1-getting-started
 * https://www.namelessalgorithm.com/genesis/blog/genesis/
 */
 
.section .text.smdvectors
    .org    0x00000000            /* Forces linker to put it at the beginning */

_rom_start:
_smd_vectors:
    .long    0x00000000                  /* Initial stack ptr value (0 - 1 = ffffffff) */
    .long    _smd_boot                   /* Program entry point */

    /* Hardware exception vectors */
    .long    _exc_bus_error             /* Bus error */
    .long    _exc_address_error         /* Address error */
    .long    _exc_illegal_instruction   /* Illegal instruction */
    .long    _exc_zero_divide           /* Division by zero */
    .long    _exc_chk_instruction       /* CHK CPU_Exception */
    .long    _exc_trapv                 /* TRAPV CPU_Exception */
    .long    _exc_privilege_violation   /* Privilege violation */
    .long    _exc_unhandled             /* TRACE exception */
    .long    _exc_unhandled             /* Line-A emulator */
    .long    _exc_unhandled             /* Line-F emulator */
    /* Unused exceptions (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    .long    _exc_unhandled             /* Unused (reserved) */
    /* Hardware Interrupts */
    .long    _int_unhandled             /* Spurious interrupt */    
    .long    _int_unhandled             /* IRQ level 1 */
    .long    _int_external              /* IRQ level 2 (External interrupt) */
    .long    _int_unhandled             /* IRQ level 3 */
    .long    _int_hblank                /* IRQ level 4 (H retrace interrupt) */
    .long    _int_unhandled             /* IRQ level 5 */
    .long    _int_vblank                /* IRQ level 6 (V retrace interrupt) */
    .long    _int_unhandled             /* IRQ level 7 */
    /* Trap exceptions vectors */
    .long    _exc_unhandled             /* TRAP #00 exception */
    .long    _exc_unhandled             /* TRAP #01 exception */
    .long    _exc_unhandled             /* TRAP #02 exception */
    .long    _exc_unhandled             /* TRAP #03 exception */
    .long    _exc_unhandled             /* TRAP #04 exception */
    .long    _exc_unhandled             /* TRAP #05 exception */
    .long    _exc_unhandled             /* TRAP #06 exception */
    .long    _exc_unhandled             /* TRAP #07 exception */
    .long    _exc_unhandled             /* TRAP #08 exception */
    .long    _exc_unhandled             /* TRAP #09 exception */
    .long    _exc_unhandled             /* TRAP #10 exception */
    .long    _exc_unhandled             /* TRAP #11 exception */
    .long    _exc_unhandled             /* TRAP #12 exception */
    .long    _exc_unhandled             /* TRAP #13 exception */
    .long    _exc_unhandled             /* TRAP #14 exception */
    .long    _exc_unhandled             /* TRAP #15 exception */
    /* Unused interrupt vectors (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
    .long    _int_unhandled             /* Unused (reserved) */
                                        /* ------------------------ */
                                        /* 256B  total */ 
