/*------------------------------------------------------------------------------
 *    Sega startup code:
 *   - m68k vector table
 *   - Sega MegaDrive header
 *   - Bootstrap code
 *   - Interrupt handlers
 -----------------------------------------------------------------------------*/

.section .text.vectors
    .org    0x00000000          /* Forces linker to put it at the beginning */

_rom_start:
_vector_table:
    dc.l    0xAFAFAFAF
    dc.l    0xEBEBEBEB