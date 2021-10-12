# SPDX-License-Identifier: MIT
#
# SGKD
# Coded by: Stephane Dallongeville
# Github: https://github.com/Stephane-D/SGDK
#
# wavtoraw a .wav sound file to binary format converter
#
# Makefile
# wavtoraw compiler makefile script
#

# MDDev bin dir
MDDEVBIN = ../../bin

# Default base flags
CFLAGS  := $(CFLAGS) -Wall -Wextra -std=c17
LDFLAGS := $(LDFLAGS)
LIBS    := -lm

# Sources
CSRC  = $(wildcard src/*.c)

# Objets files
OBJS  = $(CSRC:.c=.o)

.PHONY: all release debug clean

all: release

release: EXFLAGS  = -O3
release: wavtoraw

debug: EXFLAGS = -g -Og -DDEBUG
debug: wavtoraw

wavtoraw: $(OBJS)
	@mkdir -p $(MDDEVBIN)
	$(CC) $(LDFLAGS) -o $(MDDEVBIN)/$@ $(OBJS) $(LIBS)

src/%.o: src/%.c
	$(CC) $(CCFLAGS) $(EXFLAGS) -c $< -o $@

clean:
	@rm -f src/*.o
	@rm -f $(MDDEVBIN)/wavtoraw

