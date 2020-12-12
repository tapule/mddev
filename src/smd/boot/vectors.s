/*------------------------------------------------------------------------------
 *    Sega startup code:
 *   - m68k vector table
 *   - Sega MegaDrive header
 *   - Bootstrap code
 *   - Interrupt handlers
 -----------------------------------------------------------------------------*/

.section .text.smdvectors
    .org    0x00000000

_rom_start:
_smd_vectors:
    dc.l    0xAAAAAAAA
    dc.l    0xAAAAAAAA
    dc.l    0xAAAAAAAA
    dc.l    0xAAAAAAAA
