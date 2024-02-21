# SPDX-License-Identifier: MIT
#
# MDDev development kit
# Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021
# Github: https://github.com/tapule/mddev
#
# bintoc a binary to C language resource converter
#
# Makefile
# bintoc compiler makefile script
#

# Application name
APP := bintoc

# Tools
CC		:= gcc
MKDIR   := mkdir -p
RM      := rm -f

# MDDev bin dir
BINDIR  := ../../bin
SRCDIR  := src
SRCTREE := $(shell find $(SRCDIR) -type d)
OBJDIR  := obj
OBJTREE := $(SRCTREE:$(SRCDIR)%=$(OBJDIR)%)

# Default base flags
CFLAGS  := $(CFLAGS) -Wall -Wextra -pedantic -std=c23
LDFLAGS := $(LDFLAGS)

# Sources and objects
CSRC  := $(foreach DIR,$(SRCTREE),$(wildcard $(DIR)/*.c))
OBJS  := $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(CSRC:.c=.o))

.PHONY: all release debug clean info

all: release

release: EXFLAGS  = -O3
release: $(APP)

debug: EXFLAGS = -g -Og -DDEBUG
debug: $(APP)

$(APP): $(BINDIR) $(OBJTREE) $(OBJS)
	$(CC) $(LDFLAGS) -o $(BINDIR)/$@ $(OBJS)

obj/%.o: src/%.c
	$(CC) $(CCFLAGS) $(EXFLAGS) -c $< -o $@

$(BINDIR) $(OBJTREE):
	@$(MKDIR) $@

clean:
	@rm -rf obj
	@rm -f $(MDDEVBIN)/bintoc

info:
	$(info $(SRCTREE))
	$(info $(OBJTREE))
	$(info $(CSRC))
	$(info $(OBJS))