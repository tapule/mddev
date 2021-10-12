# Tools
Small and simple set of tools for Sega MegaDrive/Genesis hombrew development.
Some of them written by me, some not.

## paltool
Converts indexed png files up to 64 colors to Sega Megadrive/Genesis palette
format. Writes the resulting palette data as plain C arrays.

## tilesettool
Converts indexed png files up to 16 colors to Sega Megadrive/Genesis tile
format. Writes the resulting tileset data as plain C arrays.
Source images must be 4bpp or 8bpp png images with its size in pixels multiple
of 8.

## tileimagetool
Extracts Sega Megadrive/Genesis plane image tiles from indexed png files up to
16 colors. Writes the resulting plane image tiles and tileset data as plain C
arrays.
Source images must be 4bpp or 8bpp png images with its size in pixels multiple
of 8.

## bintoc
Converts binary data files to C language data structures. It lets you specify
the desired array data type, memory alignment, size alignment, etc.

## wavtoraw
A .wav sound file format to binary format converter. This tool was previously in
the SGDK tools set.

## Credits
- [lodepng](https://github.com/lvandeve/lodepng) PNG encoder/decoder by Lode
  Vandevenne.
- [SGDK](https://github.com/Stephane-D/SGDK) wavtoraw tool by Stephane
  Dallongeville