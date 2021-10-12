# SPDX-License-Identifier: MIT
#
# MDDev development kit
# Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021
# Github: https://github.com/tapule/mddev
#
# Makefile
# Tools main makefile compiler script
#

.PHONY: all paltool tilesettool tileimagetool bintoc wavtoraw clean

all: paltool tilesettool tileimagetool bintoc wavtoraw

paltool:
	@echo "-> Building paltool..."
	@make -C paltool

tilesettool:
	@echo "-> Building tilesettool..."
	@make -C tilesettool

tileimagetool:
	@echo "-> Building tileimagetool..."
	@make -C tileimagetool

bintoc:
	@echo "-> Building bintoc..."
	@make -C bintoc

wavtoraw:
	@echo "-> Building wawtoraw..."
	@make -C wavtoraw

clean:
	@echo "-> Cleaning tools..."
	@make -C paltool clean
	@make -C tilesettool clean
	@make -C tileimagetool clean
	@make -C bintoc clean
	@make -C wavtoraw clean


