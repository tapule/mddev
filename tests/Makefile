# SPDX-License-Identifier: MIT
#
# MDDev development kit
# Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021
# Github: https://github.com/tapule/mddev
#
# Makefile
# Project compiler makefile script
#

# Default paths for Marsdev
MARSDEV  ?= /opt/mars
MARSBIN  := $(MARSDEV)/m68k-elf/bin
TOOLSBIN := $(MARSDEV)/bin

# GCC environment
CC   := $(MARSBIN)/m68k-elf-gcc
AS   := $(MARSBIN)/m68k-elf-as
LD   := $(MARSBIN)/m68k-elf-ld
NM   := $(MARSBIN)/m68k-elf-nm
GDB  := $(MARSBIN)/m68k-elf-gdb
OBJC := $(MARSBIN)/m68k-elf-objcopy

# Z80 Assembler to build XGM driver
ASMZ80  := $(TOOLSBIN)/sjasm

# Tools
# TODO: Include tools to manage resources etc.
BINTOC  := bin/bintoc
BLASTEM := $(MARSDEV)/blastem/blastem
MKDIR   := mkdir -p
RM      := rm -f

# Some files needed are in a versioned directory
GCC_VER := $(shell $(CC) -dumpversion)

# Need the LTO plugin so NM can dump our symbol table
PLUGIN   = $(MARSDEV)/m68k-elf/libexec/gcc/m68k-elf/$(GCC_VER)
LTO_SO   = liblto_plugin.so

# Includes: Local + GCC (+ Newlib, uncomment to use it)
INCS     = -Isrc -Ires -Isrc/tests
INCS    += -I$(MARSDEV)/m68k-elf/lib/gcc/m68k-elf/$(GCC_VER)/include
#INCS   += -I$(MARSDEV)/m68k-elf/m68k-elf/include

# Libraries: GCC (+ Newlib, uncomment to use it)
LIBS     = -L$(MARSDEV)/m68k-elf/lib/gcc/m68k-elf/$(GCC_VER) -lgcc
#LIBS    += -L$(MARSDEV)/m68k-elf/m68k-elf/lib -lnosys

# Default base flags
#CCFLAGS  = -m68000 -Wall -Wextra -std=c17 -ffreestanding
#CCFLAGS  = -m68000 -Wall -Wextra -Wno-unused-local-typedefs -std=c17 -ffreestanding
CCFLAGS  = -m68000 -Wno-unused-local-typedefs -std=c17 -ffreestanding
ASFLAGS  = -m68000 --register-prefix-optional
LDFLAGS  = -T src/smd.ld -nostdlib
Z80FLAGS = -isrc/xgm

# Extra flags set by debug or release target as needed
EXFLAGS  =

# Sources
CSRC  = $(wildcard src/*.c)
CSRC += $(wildcard src/tests/*.c)
SSRC =  $(wildcard src/boot/*.s)
SSRC += $(wildcard src/*.s)
SSRC += $(wildcard src/tests/*.s)
SSRC += $(wildcard res/*.s)

# Z80 source for XGM driver
# ZSRC  = $(wildcard src/xgm/*.s80)

# Resources
# TODO: Include resources
RSRC  = $(wildcard res/*.c)

# Objets files
OBJS    = $(RSRC:.c=.o)
OBJS   += $(CSRC:.c=.o)
OBJS   += $(SSRC:.s=.o)
# OBJS   += $(ZSRC:.s80=.o)
OUTOBJS = $(addprefix obj/, $(OBJS))

# ASM listings
ASM    = $(CSRC:.c=.lst)
OUTASM = $(addprefix obj/, $(ASM))

.PHONY: all release asm debug tools run drun clean cleanall

all: release
#all: tools release

# Release target including optimizations
release: EXFLAGS  = -O3 -fno-web -fno-gcse -fno-unit-at-a-time -fshort-enums
release: EXFLAGS += -fomit-frame-pointer -flto -fuse-linker-plugin
release: EXFLAGS += -fno-unwind-tables -DNDEBUG
# release: EXFLAGS += -Wno-shift-negative-value -Wno-main -Wno-unused-parameter -fno-builtin
release: bin/rom.bin obj/symbol.txt

# Debug target, enables GDB tracing for Blastem, GensKMod, etc.
debug: EXFLAGS = -g -Og -DDEBUG
debug: bin/rom.bin obj/symbol.txt

# ASM output target. Generates asm listings
#asm: EXFLAGS  = -O3 -fno-web -fno-gcse -fno-unit-at-a-time -fomit-frame-pointer
#asm: EXFLAGS += -fshort-enums
asm: EXFLAGS  = -O3 -fno-web -fno-gcse -fno-unit-at-a-time -fshort-enums
asm: EXFLAGS += -fomit-frame-pointer -fuse-linker-plugin
asm: EXFLAGS += -fno-unwind-tables -DNDEBUG
asm: $(OUTASM)

bin/rom.elf: $(OUTOBJS)
	@echo "-> Building ELF..."
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(LDFLAGS) $(OUTOBJS) $(LIBS)

bin/rom.bin: bin/rom.elf
	@echo "-> Stripping ELF header..."
	@mkdir -p $(dir $@)
	@$(OBJC) -O binary $< bin/unpad.bin
	@echo "-> Padding rom file..."
	@dd if=bin/unpad.bin of=$@ bs=8192 conv=sync
	@rm -f bin/unpad.bin

obj/%.o: %.c
	@echo "CC $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) $(EXFLAGS) $(INCS) -c $< -o $@

obj/%.o: %.s
	@echo "AS $<"
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) $< -o $@

obj/%.lst: %.c
	@echo "-> Exporting ASM listings..."
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) $(EXFLAGS) $(INCS) -S -c $< -o $@
#	@$(CC) $(CCFLAGS) $(EXFLAGS) $(INCS) -S -fverbose-asm -c $< -o $@

# %.o80: %.s80
#	$(ASMZ80) $(Z80FLAGS) $< $@ obj/z80out.lst
#
# %.s: %.o80
#	$(BINTOS) $<

# This objective compiles the XGM z80 sound driver and converts it to C sources
xgm:
	@echo "-> Building XGM sound driver..."
	@mkdir -p obj/src/xgm
	$(ASMZ80) $(Z80FLAGS) src/xgm/z80_xgm.s80 obj/src/xgm/z80_xgm.o80 obj/z80out.lst
	$(BINTOC) -s obj/src/xgm/z80_xgm.o80 -d src -t u8
# 	$(BINTOC) -s src/xgm/stop_xgm.bin -d src -t u8

# This generates a symbol table that is very helpful in debugging crashes,
# even with an optimized release build!
# Cross reference symbol.txt with the addresses displayed in the crash handler
obj/symbol.txt: bin/rom.bin
	@echo "-> Exporting symbol table..."
	$(NM) --plugin=$(PLUGIN)/$(LTO_SO) -n bin/rom.elf > obj/symbol.txt

tools:
	@echo "-> Building tools..."
	@make -C tools

run: release
	@echo "-> running..."
	@$(BLASTEM) bin/rom.bin

drun: debug
	@echo "-> running gdb"
#	@$(GDB) -ex "target remote | $(BLASTEM) bin/rom.bin -D" bin/rom.elf
#	@gdbgui --gdb-cmd="$(GDB) -ex \"target remote | $(BLASTEM) bin/rom.bin -D\" bin/rom.elf" bin/rom.elf
	@mame megadriv -debug -cart bin/rom.bin

clean:
	@echo "-> Cleaning project..."
	@rm -rf obj
	@rm -f bin/rom.elf bin/unpad.bin bin/rom.bin

cleanall:
	@echo "-> Cleaning project and tools..."
	@rm -rf obj
	@rm -f bin/rom.elf bin/unpad.bin bin/rom.bin
	@make -C tools clean
	@rm -f src/z80_xgm.h src/z80_xgm.c
