/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * File: smdcpp.ld
 * SEGA Megadrive/Genesis linker script for C++ language
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
 * |        ctor list   | the ctor and dtor lists are for
 * |        dtor list   | C++ support 
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
 * | .data              | Initialized data goes here
 * |        _sdata      | Must be copied at boot time
 * |        _edata      |
 * +--------------------+
 * | .bss               | Uninitialized data goes here
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

        /*
         * .ctors section holds an array of the global C++ constructor function
         * ponters.
         * __CTOR_LIST__ and __CTOR_END__ are head and tail labels for this
         * section
         */
        . = ALIGN(0x4);        
        __CTOR_LIST__ = .;
        LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
        *(.ctors)
        LONG(0)
        __CTOR_END__ = .;

        /*
        * .dtors section holds an array of the global C++ destructor function
        * ponters.
        * __DTOR_LIST__ and __DTOR_END__ are head and tail labels for this
        * section
        */
        __DTOR_LIST__ = .;
        LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
        *(.dtors)
        LONG(0)
        __DTOR_END__ = .;

        /* C++ Exception handling information to unwind the stack */
        *(.gcc_except_table .gcc_except_table.*)

        /*
        * .init section constains only a function call that calls all the C++
        * constructors registered in __CTOR_LIST__
        * __INIT_SECTION__ is a head labels for this section
        */        
        . = ALIGN(0x4);
        __INIT_SECTION__ = . ;
        *(.init)
        SHORT (0x4E75)  /* rts */

        /*
        * .fini section constains only a function call that calls all the C++
        * destructors registered in __DTOR_LIST__
        * __FINI_SECTION__ is a head labels for this section
        */
        __FINI_SECTION__ = . ;
        *(.fini)
        SHORT (0x4E75)  /* rts */

        /* Define tag _etext at the end of this section */
        _etext = .;  
    } > rom
    __text_size = SIZEOF (.text);

     /*
     * .rodata output section: Read only data, global variables marked as const
     * and string constants.
     */
    .rodata :
    AT ( ADDR (.text) + SIZEOF (.text) )
    {
        _srodata = .;
        *(.rodata .rodata.*)
        *(.lit)

        /* Used by SGDK's Rescomp to put resources */
        *(.rodata_bin)
        *(.rodata_binf)        
        _erodata = .;        
    } > rom
    _rodata_size = SIZEOF (.rodata);

     /*
     * .data output section: Read-write initialized global and static data
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
     * .bss output section: Read-write Uninitialized data
     * It would be placed at runtime just after .data section.
     * This data has to be initialized to zero at boot time!!
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

    /*
     * C++ exception handling tables to unwind the stack
     */
    .eh_frame 0 (NOLOAD) :
    {
        *(.eh_frame)
    }
}
