/* Generated with tileimagetool v0.01                    */
/* A Sega Megadrive/Genesis tile image extractor         */
/* Github: https://github.com/tapule/md-customtools      */

#ifndef RES_IMG_H
#define RES_IMG_H

#include <stdint.h>

#define RES_IMG_SKEL_WIDTH    8
#define RES_IMG_SKEL_HEIGHT    8
#define RES_IMG_SKEL_TILESET_SIZE    64

#define RES_IMG_2TILE_WIDTH    2
#define RES_IMG_2TILE_HEIGHT    1
#define RES_IMG_2TILE_TILESET_SIZE    2

#define RES_IMG_6TILES_WIDTH    6
#define RES_IMG_6TILES_HEIGHT    1
#define RES_IMG_6TILES_TILESET_SIZE    2

#define RES_IMG_36TILES_WIDTH    6
#define RES_IMG_36TILES_HEIGHT    6
#define RES_IMG_36TILES_TILESET_SIZE    2

#define RES_IMG_1TILE_WIDTH    1
#define RES_IMG_1TILE_HEIGHT    1
#define RES_IMG_1TILE_TILESET_SIZE    1


extern const uint16_t res_img_skel[RES_IMG_SKEL_WIDTH * RES_IMG_SKEL_HEIGHT];
extern const uint32_t res_img_skel_tileset[RES_IMG_SKEL_TILESET_SIZE * 8];

extern const uint16_t res_img_2tile[RES_IMG_2TILE_WIDTH * RES_IMG_2TILE_HEIGHT];
extern const uint32_t res_img_2tile_tileset[RES_IMG_2TILE_TILESET_SIZE * 8];

extern const uint16_t res_img_6tiles[RES_IMG_6TILES_WIDTH * RES_IMG_6TILES_HEIGHT];
extern const uint32_t res_img_6tiles_tileset[RES_IMG_6TILES_TILESET_SIZE * 8];

extern const uint16_t res_img_36tiles[RES_IMG_36TILES_WIDTH * RES_IMG_36TILES_HEIGHT];
extern const uint32_t res_img_36tiles_tileset[RES_IMG_36TILES_TILESET_SIZE * 8];

extern const uint16_t res_img_1tile[RES_IMG_1TILE_WIDTH * RES_IMG_1TILE_HEIGHT];
extern const uint32_t res_img_1tile_tileset[RES_IMG_1TILE_TILESET_SIZE * 8];


#endif /* RES_IMG_H */
