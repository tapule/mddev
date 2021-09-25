# SPDX-License-Identifier: MIT
#
# MDDev development kit
# Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021
# Github: https://github.com/tapule/mddev
#
# tileimagetool a Sega Megadrive/Genesis tile image extractor
#
# Makefile
# tileimagetool compiler makefile script
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
release: tileimagetool

debug: EXFLAGS = -g -Og -DDEBUG
debug: tileimagetool

tileimagetool: $(OBJS)
	@mkdir -p $(MDDEVBIN)
	$(CC) $(LDFLAGS) -o $(MDDEVBIN)/$@ $(OBJS)

src/%.o: src/%.c
	$(CC) $(CCFLAGS) $(EXFLAGS) -c $< -o $@

clean:
	@rm -f src/*.o
	@rm -f $(MDDEVBIN)/tileimagetool

