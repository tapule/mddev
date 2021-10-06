# SPDX-License-Identifier: MIT
#
# SGKD
# Coded by: Stephane Dallongeville
# Github: https://github.com/Stephane-D/SGDK
# Modified version by: Juan Ángel Moreno Fernández (@_tapule) 2021
# Github: https://github.com/tapule/mddev
#
# bintos a binary to m68k assembler resource converter
#
# Makefile
# bintos compiler makefile script
#

# MDDev bin dir
MDDEVBIN = ../../bin

# Default base flags
CFLAGS  := $(CFLAGS) -Wall -Wextra -std=c17
LDFLAGS := $(LDFLAGS)

# Sources
CSRC  = $(wildcard src/*.c)

# Objets files
OBJS  = $(CSRC:.c=.o)

.PHONY: all release debug clean

all: release

release: EXFLAGS  = -O3
release: bintos

debug: EXFLAGS = -g -Og -DDEBUG
debug: bintos

bintos: $(OBJS)
	@mkdir -p $(MDDEVBIN)
	$(CC) $(LDFLAGS) -o $(MDDEVBIN)/$@ $(OBJS)

src/%.o: src/%.c
	$(CC) $(CCFLAGS) $(EXFLAGS) -c $< -o $@

clean:
	@rm -f src/*.o
	@rm -f $(MDDEVBIN)/bintos

