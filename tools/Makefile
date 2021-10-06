# SPDX-License-Identifier: MIT
#
# MDDev development kit
# Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021
# Github: https://github.com/tapule/mddev
#
# Makefile
# Tools main makefile compiler script
#

.PHONY: all paltool tilesettool tileimagetool bintos clean

all: paltool tilesettool tileimagetool bintos

paltool:
	@echo "-> Building paltool..."
	@make -C paltool

tilesettool:
	@echo "-> Building tilesettool..."
	@make -C tilesettool

tileimagetool:
	@echo "-> Building tileimagetool..."
	@make -C tileimagetool

bintos:
	@echo "-> Building bintos..."
	@make -C bintos

clean:
	@echo "-> Cleaning tools..."
	@make -C paltool clean
	@make -C tilesettool clean
	@make -C tileimagetool clean
	@make -C bintos clean

