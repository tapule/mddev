/*
 * SEGA Megadrive/Genesis linker script for C/C++ language
 * Based on work by Chilly Willy and Stephane Dallongeville 
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
 * |        __text_start|
 * |        ctor list   | the ctor and dtor lists are for
 * |        dtor list   | C++ support
 * |        __text_end  |
 * +--------------------+
 * .                    .
 * .                    .
 * .                    .
 * +--------------------+ <- 0x00FF0000
 * | .data              | initialized data goes here
 * |        __data_start|
 * |        __data_end  |
 * +--------------------+
 * | .bss               |
 * |        __bss_start | start of bss, cleared by crt0
 * |        __bss_end   | start of heap, used by sbrk()
 * +--------------------+
 * .                    .
 * .                    .
 * .                    .
 * |        __stack     | top of stack
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
 * Section definitions: What part of a program should go where
 */
SECTIONS
{
    /*
     * .text: Code & constant. Added to the rom
     */
    .text 0x00000000:
    {
        __text_start = .;
        /* Keep section .text.vectors at the start of .text section in rom */
        KEEP(*(.text.vectors))
        /* Put all functions of all files in section .text in rom */
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

        *(.rodata .rodata.*)

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
        SHORT (0x4E75)	/* rts */

        /*
        * .fini section constains only a function call that calls all the C++
        * destructors registered in __DTOR_LIST__
        * __FINI_SECTION__ is a head labels for this section
        */
        __FINI_SECTION__ = . ;
        *(.fini)
        SHORT (0x4E75)	/* rts */

        __text_end = .;
        *(.lit)         /* ???? */

        *(.rodata_bin)  /* ???? */
        *(.rodata_binf) /* ???? */
    } > rom
    __text_size = SIZEOF (.text);

    /*
     * .data: Initialized data
     */
    .data 0xFF0000 :
    AT ( ADDR (.text) + SIZEOF (.text) )
    {
        __data_start = .;
        *(.got.plt) *(.got)
        *(.shdata)
        *(.data .data.*)
        __data_end = .;
    } > ram
    __data_size = SIZEOF (.data);

    /*
     * .bss: Uninitialized data
     */
    .bss 0xFF0000 + SIZEOF (.data) :
    {
        __bss_start = . ;
        *(.shbss)
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = . ;
    } > ram
    __bss_size = SIZEOF (.bss);

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
