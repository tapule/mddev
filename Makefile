# Default paths
MARSDEV ?= /opt/mars
MARSBIN  = $(MARSDEV)/m68k-elf/bin
TOOLSBIN = $(MARSDEV)/bin

# GCC environment
CC   = $(MARSBIN)/m68k-elf-gcc
AS   = $(MARSBIN)/m68k-elf-as
LD   = $(MARSBIN)/m68k-elf-ld
NM   = $(MARSBIN)/m68k-elf-nm
OBJC = $(MARSBIN)/m68k-elf-objcopy
