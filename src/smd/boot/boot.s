/*------------------------------------------------------------------------------
 *    Sega startup code:
 *   - m68k vector table
 *   - Sega MegaDrive header
 *   - Bootstrap code
 *   - Interrupt handlers
 -----------------------------------------------------------------------------*/

.section .text.smdboot

_smd_boot:
    dc.l    0xEEEEEEEE
    dc.l    0xFFFFFFFF
    