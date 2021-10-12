/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * tileimagetool v0.02
 *
 * A Sega Megadrive/Genesis tile image extractor
 *
 * Extracts Sega Megadrive/Genesis image tiles from 8bpp indexed png files up to
 * 16 colors.
 *
 * Usage example: tileimagetool -s pngs/path -d dest/path -n res_img
 * 
 * It processes images in "pngs/path/*.png" to extract tiles and to build a tile
 * image drawable in a plane. It generates the C source files "res_img.h" and
 * "res_img.c" in "dest/path" directory.
 * For each png file, tileimagetool adds a define with its dimensions in tiles,
 * a define with its tileset size, a const uint16_t array containing the plane
 * tiles properties and a const uint32_t array containing the tileset data (one
 * tile a row).
 * 
 * If -s parameter is not specified, the current directory will be used as
 * source folder.
 * If -d parameter is not specified, the current directory will be used as
 * destination folder.
 * 
 * If "myimg.png" is in "pngs/path" the previos example usage generates:
 * 
 * dest/path/res_img.h
 * #ifndef RES_IMG_H
 * #define RES_IMG_H
 *
 * #include <stdint.h>
 *
 * #define RES_IMG_MYIMG_WIDTH    2
 * #define RES_IMG_MYIMG_HEIGHT   2
 * #define RES_IMG_MYIMAGE_TILESET_SIZE    2
 *
 * extern const uint16_t res_img_myimg[RES_IMG_MYIMG_WIDTH * RES_IMG_MYIMG_HEIGHT];
 * extern const uint32_t res_img_myimg_tileset[RES_IMG_MYIMG_TILESET_SIZE * 8];
 *
 * #endif // RES_IMG_H
 * 
 * dest/path/res_img.c
 * #include "res_img.h"
 *
 * const uint16_t res_img_myimg[RES_IMG_MYIMG_WIDTH * RES_IMG_MYIMG_HEIGHT] = {
 *    0x0000, 0x0001,
 *    0x0000, 0x0001 
 * };
 * const uint32_t res_img_myimg[RES_IMG_MYIMG_TILESET_SIZE * 8] = {
 *    0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 
 *    0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111211
 * };
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include "lodepng.h"

#define MAX_IMAGES              512	    /* Enough?? */
#define MAX_FILE_NAME_LENGTH    128     /* Max length for file names */
#define MAX_PATH_LENGTH         1024    /* Max length for paths */

#define PARAMS_ERROR            0   /* Error en procesado de parámetros */
#define PARAMS_STOP             1   /* Procesado de parámetros ok, finalizar */
#define PARAMS_CONTINUE         2   /* Procesado de parámetros ok, procesar */

const char version_text [] =
    "tileimagetool v0.02\n"
    "A Sega Megadrive/Genesis tile image extractor\n"
    "Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021\n"
    "Github: https://github.com/tapule/mddev\n";

const char help_text [] =
    "usage: tileimagetool [options]\n"
    "\n"
    "Options:\n"
    "  -v, --version       Show version information and exit\n"
    "  -h, --help          Show this help message and exit\n"
    "  -s <path>|<file>    Use a directory path to look for png files\n"
    "                      or a unique png file to extract images from"
    "                      Current directory will be used as default\n"
    "  -d <path>           Use a path to save generated C source files\n"
    "                      The current directory will be used as default\n"
    "  -n <name>           Use name as prefix for files, defines, vars, etc\n"
    "                      If it is not specified, \"img\" will be used as\n"
    "                      default for multiple files. Source file name itself\n"
    "                      will be used if there is only one source file\n";

/* Stores the input parameters */
typedef struct params_t
{
    char *src_path;   /* Folder with the source images in png files */
    char *dest_path;  /* Destination folder for the generated .h and .c */
    char *dest_name;  /* Base name for the generated .h and .c files */
} params_t;

/* Stores tileset's data */
typedef struct tileset_t
{
    char size_define[MAX_FILE_NAME_LENGTH]; /* Size constant define name  */
    uint8_t *data;                          /* Tiles storage */
    uint16_t size;                          /* Tileset size in tiles */
} tileset_t;

/* Stores image's data */
typedef struct image_t
{
    char file[MAX_FILE_NAME_LENGTH];           /* Original png file */
    char name[MAX_FILE_NAME_LENGTH];           /* Name without the extension */
    char width_define[MAX_FILE_NAME_LENGTH];   /* Width constant define name */
    char height_define[MAX_FILE_NAME_LENGTH];  /* Height constant define name */
    uint16_t *data;                            /* Plane tiles data storage */
    uint16_t width;                            /* Image width in tiles */
    uint16_t height;                           /* Image height in tiles */
    tileset_t tileset;                         /* Tileset data */
} image_t;

/* Global storage for the parsed images */
image_t images[MAX_IMAGES];

/**
 * @brief Convert a string to upper case
 * 
 * @param str string to convert
 */
void strtoupper(char *str)
{
    char *c;
    c = str;

    while (*c)
    {
        *c = toupper(*c);
        ++c;
    }
}

/**
 * @brief Swap nibbles in a byte
 * 
 * @param value Original byte to swap
 * @return uint8_t Swapped version of value
 */
uint8_t swap_nibbles(const uint8_t value)
{
    return (value >> 4) | (value << 4);
}

/**
 * @brief Converts a 8bpp image data buffer to 4bpp
 * 
 * @param image 8bpp image data buffer
 * @param size Size of image data in bytes (equals pixels)
 * @return uint8_t* The new 4bpp image data buffer
 */
uint8_t *image_to_4bpp(uint8_t *image, const uint32_t size)
{
    uint8_t *image_4bpp = NULL;
    uint8_t *src = NULL;
    uint8_t *dest = NULL;
    uint32_t i;

    /* Requests memory for the new image which needs half the original */
    image_4bpp = malloc(size / 2);
    if (!image_4bpp)
    {
        return NULL;
    }
    src = image;
    dest = image_4bpp;

    /* Converts two bytes in 8bpp (2 pixels) to one byte in 4bpp */
    for (i = 0; i < size / 2; ++i)
    {
        *dest = ((src[0] & 0x0F) << 4) | ((src[1] & 0x0F) << 0);
        ++dest;
        src += 2;
    }

    return image_4bpp;
}

/**
 * @brief Builds a flip X version of an input tile
 * 
 * @param tile The input tile to flip on X axis
 * @return uint8_t* Flipped version or null on error
 */
uint8_t *tile_flip_x(const uint8_t *tile)
{
    uint8_t *flip_tile = NULL;      /* New fliped X tile */
    uint8_t *flip_tile_p = NULL;    /* Pointer to work in our new tile */
    uint32_t tile_row;              /* Index in rows */

    flip_tile = malloc(32);
    if (!flip_tile)
    {
        return NULL;
    }
    /* Set the pointer to the start of our new tile */
    flip_tile_p = flip_tile;

    /* Invert each row and swap nibbles in each byte */
    for(tile_row = 0; tile_row < 8; ++tile_row)
    {
        flip_tile_p[0] = swap_nibbles(tile[3]);
        flip_tile_p[1] = swap_nibbles(tile[2]);
        flip_tile_p[2] = swap_nibbles(tile[1]);
        flip_tile_p[3] = swap_nibbles(tile[0]);
        /* Jump to next tile row */
        tile += 4;
        flip_tile_p += 4;
    }
    return flip_tile;
}

/**
 * @brief Builds a flip Y version of an input tile
 * 
 * @param tile The input tile to flip on Y axis
 * @return uint8_t* Flipped version or null on error
 */
uint8_t *tile_flip_y(const uint8_t *tile)
{
    uint8_t *flip_tile = NULL;      /* New fliped Y tile */
    uint8_t *flip_tile_p = NULL;    /* Pointer to work in our new tile */
    uint32_t tile_row;              /* Index in rows */

    flip_tile = malloc(32);
    if (!flip_tile)
    {
        return NULL;
    }
    /* Set the pointer to last row in our new tile (32 - 4) */
    flip_tile_p = flip_tile + 28;

    /* Put original tile's rows in the fliped version in inverse order */
    for(tile_row = 0; tile_row < 8; ++tile_row)
    {
        flip_tile_p[0] = tile[0];
        flip_tile_p[1] = tile[1];
        flip_tile_p[2] = tile[2];
        flip_tile_p[3] = tile[3];
        /* Jump to next tile row on the original tile */
        tile += 4;
        /* Jumps to the previous row in the fliped tile */
        flip_tile_p -= 4;
    }
    return flip_tile;
}

/**
 * @brief Checks if a tile exist, in any form, in a tile set and returns its
 * plane tile configuration
 * 
 * @param tile Tile to check
 * @param tile_storage Storage with the tiles to compare to
 * @param size Number of tiles in the storage
 * @return Plane tile configuration with vertical and horizontal flags set
 *
 * @note This function checks the source tile against each tile in the tileset
 * and also with the flip X, flip Y and flip XY version.
 */
uint16_t tile_search(const uint8_t *tile, const uint8_t *tile_storage,
                     const uint32_t size)
{
    uint32_t i;
    uint16_t plane_tile;        /* Plane tile configuration */
    uint8_t *work_tile = NULL;  /* Temporary tiles to work with */
    uint8_t *work_tile2 = NULL;

    for (i = 0; i < size; ++i)
    {
        /* Index of tile in the tileset storage */
        plane_tile = i;

        /* Compare tiles directly */
        if (!memcmp(tile, tile_storage, 32))
        {
            return plane_tile;
        }
        /* Compare with the flip Y version */        
        work_tile = tile_flip_y(tile_storage);
        if (!memcmp(tile, work_tile, 32))
        {
            /* Sets the vertical flag on */
            plane_tile |= 0x1000;
            return plane_tile;
        }
        /* Compare with the flip Y, flip X version */        
        work_tile2 = tile_flip_x(work_tile);
        free(work_tile);
        if (!memcmp(tile, work_tile2, 32))
        {
            /* Sets the vertical and horizontal flag on */
            plane_tile |= 0x1800;
            return plane_tile;
        }
        /* Compare with the flip X version */        
        work_tile = tile_flip_y(work_tile2);
        free(work_tile2);
        if (!memcmp(tile, work_tile, 32))
        {
            /* Sets the horizontal flag on */
            plane_tile |= 0x0800;
            return plane_tile;
        }
        free(work_tile);
        /* Advance to the next tile */
        tile_storage += 32;
    }

    /* If the tile wasn't found, size is the next index for the tile */
    return size;
}

/**
 * @brief Extracts a plane image and its tileset from a 4bpp image
 * 
 * @param image Source image to extract the tiles from
 * @param width Width in pixels of source image
 * @param height Height in pixels of source image
 * @return uint8_t* Buffer containing the extracted tiles
 */
bool plane_image_extract(uint8_t *image, const uint32_t width,
                         const uint32_t height, image_t *plane_image)
{
    uint32_t tile_width;    /* Width in tiles of our image */
    uint32_t tile_height;   /* Height in tiles of our image */
    uint8_t *tiles = NULL;  /* Temporary memory storage for the tileset */
    uint8_t *tiles_p = NULL;/* Current position in the tileset memory */
    uint32_t tiles_count;   /* Current number of tiles in the tileset */
    uint8_t *image_p = NULL;/* Current position in the image memory */
    uint32_t pitch;         /* Jump to the next tile row */
    uint32_t tile_x;        /* Tile x positon counter */
    uint32_t tile_y;        /* Tile y position counter */
    uint32_t tile_row;      /* Row copy position counter */
    uint16_t plane_tile;    /* Plane tile configuration */
    uint16_t *plane_image_p;/* Current position in the plane image tiles*/

    /* Image dimesions are in pixels, convert to tiles */
    tile_width = width / 8;
    tile_height = height / 8;

    /*
     A tile is 32 bytes, 8 rows of 4 bytes each. Pitch is the jump in bytes in
     the original image to point to the start of the next row in a tile 
    */
    pitch = tile_width * 4;
    
    /* Requests 32 bytes of memory for each tile to have enough space */
    tiles = malloc(tile_width * tile_height * 32);
    if (!tiles)
    {
        return false;
    }
    /* Requests memory for plane image storage */
    plane_image->data = malloc(tile_width * tile_height *
                               sizeof(uint16_t));
    if (!plane_image->data)
    {
        return false;
    }

    /* Point to the start of tiles buffer */
    tiles_p = tiles;
    tiles_count = 0;
    plane_image_p = plane_image->data;

    for (tile_y = 0; tile_y < tile_height; ++tile_y)
    {
        for (tile_x = 0; tile_x < tile_width; ++tile_x)
        {
            /* Move the image pointer to the start of next tile to process */
            image_p = &image[((tile_y * 8) * pitch) + (tile_x * 4)];

            /* Save the current tile to search for duplicates */
            for(tile_row = 0; tile_row < 8; ++tile_row)
            {
                tiles_p[0] = image_p[0];
                tiles_p[1] = image_p[1];
                tiles_p[2] = image_p[2];
                tiles_p[3] = image_p[3];
                /* Jump to next tile row */
                image_p += pitch;
                tiles_p += 4;
            }

            /* Compares the current tile with all the others */
            plane_tile = tile_search(tiles_p - 32, tiles, tiles_count);
            /* The tile wasn't found, we already added it to the tileset */
            if (plane_tile == tiles_count)
            {
                ++tiles_count;
            }
            else
            {
                /* Not found, adjust the write position in the tiles buffer */
                tiles_p -= 32;
            }
            /* Save the plane tile config */
            *plane_image_p = plane_tile;
            ++plane_image_p;
        }
    }
    /* Save plane image sizes */
    plane_image->width = tile_width;
    plane_image->height = tile_height;
 
    /* Setup and copy the resulting tileset */
    plane_image->tileset.size = tiles_count;
    plane_image->tileset.data = malloc(tiles_count * 32);
    memcpy(plane_image->tileset.data, tiles, tiles_count * 32);
    free(tiles);
 
    printf("\tImage size in tiles: %dx%d\n", tile_width, tile_height);
    printf("\tImage tileset size: %d\n", tiles_count);
 
    return true;
}

/**
 * @brief Parses the input parameters
 * 
 * @param argc Input arguments counter
 * @param argv Input arguments vector
 * @param params Where to store the input paramss
 * @return 0 if there was an error
 *         1 if the arguments parse was ok but we must end (-v or -h) 
 *         2 if the arguments parse was ok and we can continue
 */
uint8_t parse_params(uint32_t argc, char** argv, params_t *params)
{
    uint32_t i;
 
    i = 1;
    while (i < argc)
    {
        if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0))
        {
            fputs(version_text, stdout);
            return PARAMS_STOP;
        }
        else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
        {
            fputs(help_text, stdout);
            return PARAMS_STOP;
        }
        /* Source path where png files are */
        else if (strcmp(argv[i], "-s") == 0)
        {
            if (i < argc - 1)
            {
                params->src_path = argv[i + 1];
                ++i;
            }
            else
            {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n",
                        argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        }
        /* Destination path to save the generated .h and .c files */
        else if (strcmp(argv[i], "-d") == 0)
        {
            if (i < argc - 1)
            {
                params->dest_path = argv[i + 1];
                ++i;
            }
            else
            {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n",
                        argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        }
        /* Base name for variables, defines, and  generated .h and .c files */
        else if (strcmp(argv[i], "-n") == 0)
        {
            if (i < argc - 1)
            {
                params->dest_name = argv[i + 1];
                ++i;
            }
            else
            {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n",
                        argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        }
        else 
        {
            fprintf(stderr, "%s: unknown option: '%s'\n", argv[0], argv[i]);
            return PARAMS_ERROR;
        }           
        ++i;
    }
    return PARAMS_CONTINUE;
}

/**
 * @brief Processes a png image file and extracts its tiles in Megadrive format
 * 
 * @param path File path
 * @param file Png image file to process
 * @param image_index Index in the images array to store the data
 * @return 0 if success, lodepng error code in other case
 */
uint32_t image_read(const char* path, const char *file,
                    const uint32_t image_index)
{
    char file_path[MAX_PATH_LENGTH];
    char *file_ext;
    uint32_t error;
    uint8_t *png_data = NULL;
    size_t png_size;
    LodePNGState png_state;
    uint8_t *image_data = NULL;
    uint32_t image_width;
    uint32_t image_height;
    uint8_t *image_4bpp = NULL;
    uint32_t i;

    /* Builds the complete file path */
    strcpy(file_path, path);
    strcat(file_path, "/");
    strcat(file_path, file);
    printf("File %s\n", file_path);

    /* Load the file into a memory buffer, no png checks here */
    error = lodepng_load_file(&png_data, &png_size, file_path);
    if (error)
    {
        free(png_data);
        printf(lodepng_error_text(error));
        return error;
    }
    
    lodepng_state_init(&png_state);
    /* Get colors and pixels without conversion */
    png_state.decoder.color_convert = false;
    /* Decode our png image */
    error = lodepng_decode(&image_data, &image_width, &image_height, &png_state,
                           png_data, png_size);
    free(png_data);
    /* Checks for errors in the decode stage */
    if (error)
    {
        printf("\tSkiping file: ");
        printf(lodepng_error_text(error));
        putchar('\n');
        return error;
    }

    /* Checks if the image is an indexed one */
    if (png_state.info_png.color.colortype != LCT_PALETTE)
    {
        printf("\tSkiping file: The image must be in indexed color mode\n");
        return 1;
    }
 
    /* Checks if the image is a 4bpp or 8bpp one */
    if (png_state.info_png.color.bitdepth != 4 &&
        png_state.info_png.color.bitdepth != 8)
    {
        printf("\tSkiping file: %d bpp not suported. Only 4bpp and 8bpp png files supported \n");
        return 1;
    }

    /* Checks if the image has more than 16 colors */
    if (png_state.info_png.color.palettesize > 16)
    {
        printf("\tSkiping file: More than 16 colors png image detected. \n");
        return 1;
    }

    /* Checks if image width is multiple of 8 */
    if (image_width % 8)
    {
        printf("\tSkiping file: Image width is not multiple of 8. \n");
        return 1;
    }
 
    /* Checks if image height is multiple of 8 */
    if (image_height % 8)
    {
        printf("\tSkiping file: Image height is not multiple of 8. \n");
        return 1;
    }

    /* Converts the image to Megadrive 4bpp format only if it is 8bpp */
    if (png_state.info_png.color.bitdepth == 8)
    {
        image_4bpp = image_to_4bpp(image_data, image_width * image_width);
        free(image_data);
    
        if (!image_4bpp)
        {
            printf("\tError: Can't conver image to 4bpp. \n");
            return 1;
        }
    }
    else
    {
        image_4bpp = image_data;
    }

    /* Extract the plane image and tileset from our 4bpp image data */
    plane_image_extract(image_4bpp, image_width, image_height, 
                        &images[image_index]);
    free(image_4bpp);

    /* Save the image file name and sizes in tiles */
    strcpy(images[image_index].file, file);
 
    /* Save the tileset name without the extension in the tileset struct */
    strcpy( images[image_index].name, file);
    file_ext = strrchr( images[image_index].name, '.');
    if (file_ext)
    {
        *file_ext = '\0';
    }

    return 0;
}

/**
 * @brief Builds the C header file for the generated plane images
 * 
 * @param path Destinatio path for the .h file
 * @param name Base name for the .h file (name + .h)
 * @param use_prefix Indicate if a prefix should be used for vars, etc.
 * @param image_count Number of images to process from the global image storage
 * @return true if everythig was correct, false otherwise
 */
bool build_header_file(const char *path, const char *name,
                       const bool use_prefix, const uint32_t image_count)
{
    FILE *h_file;
    char buff[1024];
    uint32_t i, j;

    /* Builds the .h complete file path */
    strcpy(buff, path);
    strcat(buff, "/");
    strcat(buff, name);
    strcat(buff, ".h");

    h_file = fopen(buff, "w");
    if (!h_file)
    {
        return false;
    }

    /* An information message */
    fprintf(h_file, "/* Generated with tileimagetool v0.02                    */\n");
    fprintf(h_file, "/* A Sega Megadrive/Genesis tile image extractor         */\n");
    fprintf(h_file, "/* Github: https://github.com/tapule/mddev               */\n\n");

    /* Header include guard */
    strcpy(buff, name);
    strtoupper(buff);
    strcat(buff, "_H");
    fprintf(h_file, "#ifndef %s\n", buff);
    fprintf(h_file, "#define %s\n\n", buff);
    fprintf(h_file, "#include <stdint.h>\n\n");

    /* Image and tileset sizes defines */
    for (i = 0; i < image_count; ++i)
    {
        /* BASENAME_IMAGENAME */
        images[i].width_define[0] = '\0';
        if (use_prefix)
        {
            strcpy(images[i].width_define, name);
            strcat(images[i].width_define, "_");
        }          
        strcat(images[i].width_define, images[i].name);
        strtoupper(images[i].width_define);
        /* Copy to height and tileset size */
        strcpy(images[i].height_define, images[i].width_define);
        strcpy(images[i].tileset.size_define, images[i].width_define);

        /* BASENAME_IMAGENAME_WIDTH */
        strcat(images[i].width_define, "_WIDTH");
        fprintf(h_file, "#define %s    %d\n", images[i].width_define,
                images[i].width);

        /* BASENAME_IMAGENAME_HEIGHT */
        strcat(images[i].height_define, "_HEIGHT");
        fprintf(h_file, "#define %s    %d\n", images[i].height_define,
                images[i].height);

        /* BASENAME_IMAGENAME_TILESET_SIZE */
        strcat(images[i].tileset.size_define, "_TILESET_SIZE");
        fprintf(h_file, "#define %s    %d\n", images[i].tileset.size_define,
                images[i].tileset.size);
        fprintf(h_file, "\n");
    }
    fprintf(h_file, "\n");

    /* Images and tilesets declarations */   
    for (i = 0; i < image_count; ++i)
    {
        buff[0] = '\0';
        if (use_prefix)
        {
            strcpy(buff, name);
            strcat(buff, "_");
        }        
        strcat(buff, images[i].name);
        fprintf(h_file, "extern const uint16_t %s[%s * %s];\n", buff,
                images[i].width_define, images[i].height_define);

        strcat(buff, "_tileset");
        fprintf(h_file, "extern const uint32_t %s[%s * 8];\n", buff,
                images[i].tileset.size_define);
        fprintf(h_file, "\n");
    }
    fprintf(h_file, "\n");

    /* End of header include guard */
    strcpy(buff, name);
    strtoupper(buff);
    strcat(buff, "_H");
    fprintf(h_file, "#endif /* %s */\n", buff);

    fclose(h_file);
    return true;
}

/**
 * @brief Builds the C source file for the extracted images
 * 
 * @param path Destinatio path for the .c file
 * @param name Base name for the .c file (name + .c)
 * @param use_prefix Indicate if a prefix should be used for files, vars, etc.
 * @param image_count Number of images to process from the global image storage
 * @return true if everythig was correct, false otherwise
 */
bool build_source_file(const char *path, const char *name,
                       const bool use_prefix, const uint32_t image_count)
{
    FILE *c_file;
    char buff[1024];
    uint32_t image;     /* Current image to process */
    uint32_t tile;      /* Current tile to process */
    uint32_t row;       /* Current row */
    uint32_t column;    /* Current column */
    uint8_t line_feed; 

    /* Builds the .c complete file path */
    strcpy(buff, path);
    strcat(buff, "/");
    strcat(buff, name);
    strcat(buff, ".c");

    c_file = fopen(buff, "w");
    if (!c_file)
    {
        return false;
    }

    /* Header include */
    strcpy(buff, name);
    strcat(buff, ".h");
    fprintf(c_file, "#include \"%s\"\n\n", buff);

    /* How many values we write per line */
    line_feed = 9;

    for (image = 0; image < image_count; ++image)
    {
        buff[0] = '\0';
        if (use_prefix)
        {
            strcpy(buff, name);
            strcat(buff, "_");
        }          
        /* Writes the plane image definition */
        strcat(buff, images[image].name);
        fprintf(c_file, "const uint16_t %s[%s * %s] = {", buff,
                images[image].width_define, images[image].height_define);

        for (row = 0; row < images[image].height; ++row)
        {
            /* Separate image definition from text line start */
            fprintf(c_file, "\n    ");
            /* Writes all the image's tiles */
            for (column = 0; column < images[image].width; ++column)
            {
                fprintf(c_file, "0x%04X",
                        images[image].data[(row * images[image].width) + column]);
                /* If we aren't done, add a separator*/
                if ((row * images[image].width) + column + 1 < 
                    (images[image].width * images[image].height))
                {
                    fprintf(c_file, ", ");
                }
            }
        }
        fprintf(c_file, "\n};\n");

        /* Writes the plane image tileset definition */
        strcat(buff, "_tileset");
        fprintf(c_file, "const uint32_t %s[%s * 8] = {", buff,
                images[image].tileset.size_define);

        for (tile = 0; tile < images[image].tileset.size; ++tile)
        {
            /* Do we need to write a comma after the las value? */
            if (tile)
            {
                fprintf(c_file, ", ");
            }            
            /* Separate tile definition from text line start */
            fprintf(c_file, "\n    ");
            /* Writes all the tile rows in a single line */
            for (row = 0; row < 8; ++row)
            {
                /* Each tile row is 4 hex values */
                fprintf(c_file, "0x");
                fprintf(c_file, "%02X", images[image].tileset.data[(tile * 32) + (row * 4)]);
                fprintf(c_file, "%02X", images[image].tileset.data[(tile * 32) + (row * 4) + 1]);
                fprintf(c_file, "%02X", images[image].tileset.data[(tile * 32) + (row * 4) + 2]);
                fprintf(c_file, "%02X", images[image].tileset.data[(tile * 32) + (row * 4) + 3]);
                /* Add a separator for each tile row declaration */
                if (row < 7)
                {
                    fprintf(c_file, ", ");
                }
            }
        }
        fprintf(c_file, "\n};\n\n");
    }

    fclose(c_file);
    return true;
}

int main(int argc, char **argv)
{
    params_t params = {0};
    uint32_t image_index = 0;
    DIR *dir;
    char *file_name;     
    struct dirent *dir_entry;
    uint8_t params_status;     

    /* Set default values here */
    params.src_path = ".";
    params.dest_path = ".";

    /* Argument reading and processing */
    params_status = parse_params(argc, argv, &params);
    if (params_status == PARAMS_ERROR)
    {
        return EXIT_FAILURE;
    }
    if (params_status == PARAMS_STOP)
    {
        return EXIT_SUCCESS;
    }

    /* First try to open source path as a directory */
    dir = opendir(params.src_path);
    if (dir != NULL)
    {
        printf(version_text);
        printf("\nReading files...\n");
        while ((dir_entry = readdir(dir)) != NULL)
        {
            /* Checks max allowed images */
            if (image_index >= MAX_IMAGES)
            {
                closedir(dir);
                fprintf(stderr, "Error: More than %d files in the source directory\n", MAX_IMAGES);
                return EXIT_FAILURE;
            }
            
            /* Process only regular files */
            if (dir_entry->d_type == DT_REG)
            {
                if (!image_read(params.src_path, dir_entry->d_name, image_index))
                {
                    printf("\tPng file to plane image: %s -> %s\n", dir_entry->d_name, 
                        images[image_index].name);
                    ++image_index;
                }
            }
        }
        closedir(dir);
    }
    /* We can't open source path as directory, try to open it as file instead */
    else
    {
        /* Get the file name and path */
        file_name = strrchr(params.src_path, '/');
        if (file_name)
        {
            *file_name = '\0';
            ++file_name;
        }
        else
        {
            file_name = params.src_path;
            params.src_path = ".";
        }
        printf(version_text);
        printf("\nReading file...\n");
        if (!image_read(params.src_path, file_name, image_index))
        {
            printf("\tFile to binary: %s -> %s\n", file_name, 
                images[image_index].name);
            ++image_index;
        }
    }

    printf("%d images readed.\n", image_index);
    

    if (image_index > 0)
    {
        /* By default use BASE_NAME as prefix for files, defines, vars, etc */
        bool use_prefix = true;

        /* Adjust the destination base name if it was not specified */
        if (!params.dest_name)
        {
            /* Only one file, use its name as base name and no prefix */
            if (image_index == 1)
            {
                params.dest_name = images[0].name;
                use_prefix = false;
            }
            /* More than one file, use "bins" as base name */
            else
            {
                params.dest_name = "img";
            }
        }

        printf("Building C header file...\n");
        build_header_file(params.dest_path, params.dest_name, use_prefix,
                          image_index);
        printf("Building C source file...\n");
        build_source_file(params.dest_path, params.dest_name, use_prefix,
                          image_index);
        printf("Done.\n");
    }

    return EXIT_SUCCESS;
}
