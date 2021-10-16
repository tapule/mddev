/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: smd.ld
 * SEGA Megadrive/Genesis linker script for C language
 *
 * Linker script for C language based on work by Chilly Willy and Stephane
 * Dallongeville
 *
 * More info:
 * https://doc.ecoscentric.com/gnutools/doc/ld.pdf
 * https://interrupt.memfault.com/blog/how-to-write-linker-scripts-for-firmware
 */

OUTPUT_ARCH(m68k)
SEARCH_DIR(.)
/*GROUP(-lbcc -lc -lgcc)*/
/* No shared dynamic libraries */
__DYNAMIC  =  0;

/*
 * Setup the memory map of the SEGA Megadrive/Genesis.
 * stack grows down from high memory.
 *
 * The memory map look like this:
 * +--------------------+ <- low memory
 * | .text              |
 * |        _stext      |
 * |        _etext      |
 * +--------------------+
 * | .rodata            | Read only data
 * |        _srodata    |
 * |        _erodata    |
 * +--------------------+
 * .                    .
 * .                    .
 * .                    .
 * +--------------------+ <- 0x00FF0000
 * | .data              | initialised data goes here
 * |        _sdata      | Must be copied at boot time
 * |        _edata      |
 * +--------------------+
 * | .bss               | Uninitialised data goes here
 * |        _sbss       | Must be cleared at boot time
 * |        _ebss       | Start of heap, used by sbrk()
 * +--------------------+
 * .                    .
 * .                    .
 * .                    .
 * |        __stack     | Top of stack
 * +--------------------+ <- 0x01000000
 */

/*
 * Memory Layout: What memory is available where
 */
MEMORY
{
    rom (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00A00000
    ram (rwx) : ORIGIN = 0x00FF0000, LENGTH = 0x00010000
}

/*
 * Allocate the stack to be at the top of memory, since the stack grows down
 */
PROVIDE (__stack = 0x01000000);

/*
 * Section definitions: What part of a program should go where, that is,
 * describes the memory layout of the output file specifying how the sections
 * are going to be merged and at what location are going to be placed.
 */
SECTIONS
{
    /*
     * .text output section : Code & constant. Added to the rom
     */
    .text 0x00000000:
    {
        /* Define tag _stext at location counter position */
        _stext = .;
        /*
         * Which input section should be placed into this output section.
         *
         * Put input sections .text.vectors .text.smdheader .text.smdboot at the
         * very start of .text section in rom and mark them as non-removables
         * by link-time garbage collector.
         */
        KEEP(*(.text.smdvectors))
        KEEP(*(.text.smdheader))
        KEEP(*(.text.smdboot))

        /* Put all .text input section of all files here */
        *(.text.*) *(.text)
    	/* . = ALIGN(0x4); */
        /* Define tag _etext at the end of this section */
        _etext = .;        

		/* Read only data: global vars marked as const and string constants */
        _srodata = .;
        *(.lit)
        *(.rodata .rodata.*)
        _erodata = .;        
    } > rom
    _text_size = SIZEOF (.text);

    /*
     * .data output section: Read-write initialised global and static data
     * 0xFF0000 is the VMA (Virtual Memory Address): Address where this section
     * would be during execution.
     * ADDR (.text) + SIZEOF (.text) is the LMA (Load Memory Address): Address
     * where it would be stored in rom to be loaded at runtime.
     * This data has to be copied from rom to ram at boot time!!
     */
    .data 0xFF0000 :
    AT ( ADDR (.text) + SIZEOF (.text) + SIZEOF(.rodata) )
    {
        _sdata = .;
        *(.got.plt) *(.got)
        *(.shdata)
        *(.data .data.*)
        _edata = .;
    } > ram
    _data_size = SIZEOF (.data);

    /*
     * .bss output section: Read-write Uninitialised data
     * It would be placed at runtime just after .data section.
     * This data has to be initialised to zero at boot time!!
     */
    .bss 0xFF0000 + SIZEOF (.data) :
    {
        _sbss = . ;
        /* Statically-allocated variables represented solely by zero-valued bits initially */
        *(.shbss)
        *(.bss .bss.*)
        *(COMMON)
        _ebss = . ;
    } > ram
    _bss_size = SIZEOF (.bss);

    /*
     * Debugging information encapsulated in stab assembler directives.
     * Contains an array of fixed length structures, one struct per stab
     */
    .stab 0 (NOLOAD) :
    {
       *(.stab)
    }

    /*
     * Degugging information, containing all the variable length strings that
     * are referenced by stabs in the .stab section
     */
    .stabstr 0 (NOLOAD) :
    {
        *(.stabstr)
    }
}
