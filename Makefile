# Default paths for Marsdev
MARSDEV ?= /opt/mars
MARSBIN  = $(MARSDEV)/m68k-elf/bin
TOOLSBIN = $(MARSDEV)/bin

# GCC environment
CC   = $(MARSBIN)/m68k-elf-gcc
AS   = $(MARSBIN)/m68k-elf-as
LD   = $(MARSBIN)/m68k-elf-ld
NM   = $(MARSBIN)/m68k-elf-nm
OBJC = $(MARSBIN)/m68k-elf-objcopy

# Z80 Assembler to build XGM driver
ASMZ80   = $(TOOLSBIN)/sjasm

# Tools
# TODO: Include tools to manage resources etc.
BINTOS  = $(TOOLSBIN)/bintos
BLASTEM = $(MARSBIN)/blastem/blastem

# Some files needed are in a versioned directory
GCC_VER := $(shell $(CC) -dumpversion)

# Need the LTO plugin so NM can dump our symbol table
PLUGIN   = $(MARSDEV)/m68k-elf/libexec/gcc/m68k-elf/$(GCC_VER)
LTO_SO   = liblto_plugin.so

# Includes: Local + GCC (+ Newlib, uncomment to use it)
INCS     = -Isrc -Ismd -Ires
INCS    += -I$(MARSDEV)/m68k-elf/lib/gcc/m68k-elf/$(GCC_VER)/include
#INCS   += -I$(MARSDEV)/m68k-elf/m68k-elf/include

# Libraries: GCC (+ Newlib, uncomment to use it)
LIBS     = -L$(MARSDEV)/m68k-elf/lib/gcc/m68k-elf/$(GCC_VER) -lgcc
#LIBS    += -L$(MARSDEV)/m68k-elf/m68k-elf/lib -lnosys

# Default base flags
CCFLAGS  = -m68000 -Wall -Wextra -std=c99 -ffreestanding
ASFLAGS  = -m68000 --register-prefix-optional
LDFLAGS  = -T src/smd/smd.ld -nostdlib
Z80FLAGS = -isrc/smd/xgm

# Extra flags set by debug or release target as needed
EXFLAGS  = 

# Sources
CSRC  = $(wildcard src/*.c)
CSRC += $(wildcard src/smd/*.c)
SSRC  = $(wildcard src/*.s)
SSRC += $(wildcard src/smd/*.s)
# Z80 source for XGM driver
ZSRC  = $(wildcard src/smd/xgm/*.s80)
# Resources
# TODO: Include resources
RSRC  = 

# Objets files
OBJS    = $(RSRC:.res=.o)
OBJS   += $(CSRC:.c=.o)
OBJS   += $(SSRC:.s=.o)
OBJS   += $(ZSRC:.s80=.o)
OUTOBJS = $(addprefix obj/, $(OBJS))

# ASM listings
ASM    = $(CSRC:.c=.lst)
OUTASM = $(addprefix obj/, $(ASM))

.PHONY: all release asm debug

all: release

# Release target including optimizations
release: EXFLAGS  = -O3 -fno-web -fno-gcse -fno-unit-at-a-time -fshort-enums
release: EXFLAGS += -fomit-frame-pointer -flto -fuse-linker-plugin
release: EXFLAGS += -fno-unwind-tables
# release: EXFLAGS += -Wno-shift-negative-value -Wno-main -Wno-unused-parameter -fno-builtin
release: bin/rom.bin obj/symbol.txt

# Debug target, enables GDB tracing for Blastem, GensKMod, etc.
debug: EXFLAGS = -g -Og -DDEBUG -DKDEBUG
debug: bin/rom.bin obj/symbol.txt

# ASM output target. Generates asm listings
asm: EXFLAGS  = -O3 -fno-web -fno-gcse -fno-unit-at-a-time -fomit-frame-pointer
asm: EXFLAGS += -fshort-enums
asm: $(OUTASM)

obj/src/smd/boot/boot.o:
	@echo "-> Building rom boot..."
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) src/smd/boot/boot.s -o $@

bin/rom.elf: obj/src/smd/boot/boot.o $(OUTOBJS)
	@echo "-> Building ELF..."
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(LDFLAGS) obj/src/smd/boot/boot.o $(OUTOBJS) $(LIBS)

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

%.o80: %.s80
	$(ASMZ80) $(Z80FLAGS) $< $@ obj/z80out.lst

%.s: %.o80
	$(BINTOS) $<

# This generates a symbol table that is very helpful in debugging crashes,
# even with an optimized release build!
# Cross reference symbol.txt with the addresses displayed in the crash handler
obj/symbol.txt: bin/rom.bin
	@echo "-> Exporting symbol table..."
	$(NM) --plugin=$(PLUGIN)/$(LTO_SO) -n bin/rom.elf > obj/symbol.txt

.PHONY: clean

clean:
	@echo "-> Cleaning project..."
	@rm -rf obj
	@rm -f bin/rom.elf bin/unpad.bin

