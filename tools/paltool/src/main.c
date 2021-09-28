/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * paltool v0.02
 *
 * Sega Megadrive/Genesis palette converter
 *
 * Convert indexed png files up to 64 colors to Sega Megadrive/Genesis palette
 * format.
 *
 * Usage example: paltool -s pngs/path -p . -n res_pal
 * 
 * It extracts all palettes in "pngs/path/*.png" and generates the C source
 * files "base_name.h" and "base_name.c" in "dest/path" directory.
 * For each png file, paltool adds a define with its size in colors and a const
 * uint16_t array containing the palette color data.
 * 
 * If -p parameter is not specified, the current directory will be used as
 * destination folder.
 * 
 * If "mypal.png" is in "pngs/path" the previos example usage generates:
 * 
 * res_pal.h
 * #ifndef RES_PAL_H
 * #define RES_PAL_H
 *
 * #include <stdint.h>
 *
 * #define RES_PAL_MYPAL_SIZE    16
 *
 * extern const uint16_t res_pal_mypal[RES_PAL_MYPAL_SIZE];
 *
 * #endif // RES_PAL_H
 * 
 * res_pal.c
 * #include "res_pal.h"
 *
 * const uint16_t res_pal_mypal[RES_PAL_MYPAL_SIZE] = {
 *    0x0000, 0x0222, 0x0008, 0x0024, 0x002E, 0x0444, 0x006C, 0x0C80, 
 *    0x0888, 0x008E, 0x02AE, 0x0CCC, 0x0EE8, 0x0AEE, 0x0EEE
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

#define MAX_PALETTES            512		/* Who needs more?? */
#define MAX_COLORS              64      /* Max colors in a Megadrive palete */
#define MAX_FILE_NAME_LENGTH    128     /* Max length for file names */
#define MAX_PATH_LENGTH         1024    /* Max length for paths */

const char version_text [] =
    "paltool v0.02\n"
    "Sega Megadrive/Genesis palette converter\n"
    "Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021\n"
    "Github: https://github.com/tapule/md-customtools\n";

const char help_text [] =
    "usage: paltool [options] [-s] SRC_DIR -[p] DEST_DIR -[n] BASE_NAME\n"
    "\n"
    "Options:\n"
    "  -v, --version       show version information and exit\n"
    "  -h, --help          show this help message and exit\n"
    "  -s SRC_DIR          use SRC_DIR to search png files to extract palettes\n"
    "  -p DEST_DIR         use DEST_DIR to save generated C source files\n"
    "                      The current directory will be used as default\n"
    "  -n BASE_NAME        use BASE_NAME as prefix for files, defines, vars, etc\n";

/* Stores the input parameters */
typedef struct params_t
{
    const char *src_path;   /* Folder with the source palettes in png files */
    const char *dest_path;  /* Destination folder for the generated .h and .c */
    const char *dest_name;  /* Base name for the generated .h and .c files */
} params_t;

/* Stores palette's data */
typedef struct palette_t
{
    char file[MAX_FILE_NAME_LENGTH];        /* Original png file */
    char name[MAX_FILE_NAME_LENGTH];        /* Name without the extension */
    char size_define[MAX_FILE_NAME_LENGTH]; /* Size constant define name  */
    uint16_t colors[MAX_COLORS];            /* Color storage */
    uint8_t size;                           /* Palette's size in colors */
} palette_t;

/* Global storage for the parsed palettes */
palette_t palettes[MAX_PALETTES];

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
 * @brief Parses the input parameters
 * 
 * @param argc Input arguments counter
 * @param argv Input arguments vector
 * @param params Where to store the input paramss
 * @return True if the arguments parse was ok, false otherwise
 */
bool parse_params(uint32_t argc, char** argv, params_t *params)
{
    uint32_t i;
 
    i = 1;
    while (i < argc)
    {
        if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0))
        {
            fputs(version_text, stdout);
            return true;
        }
        else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
        {
            fputs(help_text, stdout);
            return true;
        }
        /* Source path where palette files are */
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
                return false;
            }
        }
        /* Destination path to save the generated .h and .c files */
        else if (strcmp(argv[i], "-p") == 0)
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
                return false;
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
                return false;
            }
        }
        ++i;
    }
    return true;
}

/**
 * @brief Processes a png file and convert its palete to Megadrive format
 * 
 * @param path File path
 * @param file Png file to process
 * @param pal_index Index in the palettes array to store the data
 * @return 0 if success, lodepng error code in other case
 */
uint32_t palette_read(const char* path, const char *file,
                      const uint32_t pal_index)
{
    char file_path[MAX_PATH_LENGTH];
    char *file_ext;
    uint32_t error;
    uint8_t *png_data = NULL;
    size_t png_size;
    LodePNGState png_state;
    uint8_t *img_data = NULL;
    uint32_t img_width;
    uint32_t img_height;
    uint32_t palette_size;
    uint32_t i;
    uint8_t r_component;
    uint8_t g_component;
    uint8_t b_component;

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
    png_state.decoder.color_convert = false;
    /* Decode our png image */
    error = lodepng_decode(&img_data, &img_width, &img_height, &png_state,
                           png_data, png_size);
    free(png_data);
    free(img_data);
    /* Checks for errors in the decode stage */
    if (error)
    {
        printf("\tSkiping file: ");
        printf(lodepng_error_text(error));
        putchar('\n');
        return error;
    }

    /* Checks if the image is an indexed one */
    if (png_state.info_raw.colortype != LCT_PALETTE)
    {
        printf("\tSkiping file: The image must be in indexed color mode\n");
        return 1;
    }

    /* Read a maximum of 64 colors */
    palette_size = png_state.info_png.color.palettesize > MAX_COLORS
                   ? MAX_COLORS : png_state.info_png.color.palettesize;

    /* 
        Do the conversion to a Sega Megadrive/Genesis palette
        Based on SGDK rescomp pal parser
    */
    for(i = 0; i < palette_size; ++i)
    {
        /* Read the color components from the png palette */
        r_component = png_state.info_png.color.palette[(i*4) + 0];
        g_component = png_state.info_png.color.palette[(i*4) + 1];
        b_component = png_state.info_png.color.palette[(i*4) + 2];
        
        /*
            Convert color components to Sega Megadrive/Genesis format:
    			000 BBB0 GGG0 RRR0
            9bits: 3bits of blue, 3bits of green, 3bits of red (inverse order)
        */
        r_component = (r_component >> 4) & 0xE;
        g_component = (g_component >> 4) & 0xE;
        b_component = (b_component >> 4) & 0xE;

        palettes[pal_index].colors[i] = (r_component << 0) |
                                        (g_component << 4) |
                                        (b_component << 8);
    }

    /* Save the palette file name and color size */
    strcpy(palettes[pal_index].file, file);
    palettes[pal_index].size = palette_size;

    /* Save the palette name without the extension in the palette struct */
    strcpy(palettes[pal_index].name, file);
    file_ext = strrchr(palettes[pal_index].name, '.');
    if (file_ext)
    {
        *file_ext = '\0';
    }

    return 0;
}

/**
 * @brief Builds the C header file for the generated palettes
 * 
 * @param path Destinatio path for the .h file
 * @param name Base name for the .h file (name + .h)
 * @param palette_count Number of palettes to process from the global palettes
 * @return true if everythig was correct, false otherwise
 */
bool build_header_file(const char *path, const char *name,
                       const uint32_t palette_count)
{
    FILE *h_file;
    char buff[1024];
    uint32_t i, j;

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
    fprintf(h_file, "/* Generated with paltool v0.02                     */\n");
    fprintf(h_file, "/* a Sega Megadrive/Genesis palette converter       */\n");
    fprintf(h_file, "/* Github: https://github.com/tapule/md-customtools */\n\n");

    /* Header include guard */
    strcpy(buff, name);
    strtoupper(buff);
    strcat(buff, "_H");
    fprintf(h_file, "#ifndef %s\n", buff);
    fprintf(h_file, "#define %s\n\n", buff);
    fprintf(h_file, "#include <stdint.h>\n\n");

    /* Palette sizes defines */
    for (i = 0; i < palette_count; ++i)
    {
        strcpy(palettes[i].size_define, name);
        strcat(palettes[i].size_define, "_");
        strcat(palettes[i].size_define, palettes[i].name);
        strcat(palettes[i].size_define, "_SIZE");
        strtoupper(palettes[i].size_define);
        fprintf(h_file, "#define %s    %d\n", palettes[i].size_define,
                palettes[i].size);
    }
    fprintf(h_file, "\n");

    /* Palette declarations */   
    for (i = 0; i < palette_count; ++i)
    {
        strcpy(buff, name);
        strcat(buff, "_");
        strcat(buff, palettes[i].name);
        fprintf(h_file, "extern const uint16_t %s[%s];\n", buff,
                palettes[i].size_define);
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
 * @brief Builds the C source file for the generated palettes
 * 
 * @param path Destinatio path for the .c file
 * @param name Base name for the .c file (name + .c)
 * @param palette_count Number of palettes to process from the global palettes
 * @return true if everythig was correct, false otherwise
 */
bool build_source_file(const char *path, const char *name,
                       const uint32_t palette_count)
{
    FILE *c_file;
    char buff[1024];
    uint32_t i;
    uint32_t j;

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

    /* Palette definitions */
    for (i = 0; i < palette_count; ++i)
    {
        strcpy(buff, name);
        strcat(buff, "_");
        strcat(buff, palettes[i].name);
        fprintf(c_file, "const uint16_t %s[%s] = {", buff,
                palettes[i].size_define);
        for (j = 0; j < palettes[i].size; ++j)
        {
            /* Every 8 hex colors, add a line feed */
            if (j % 8 == 0)
            {
                fprintf(c_file, "\n    ");
            }
            fprintf(c_file, "0x%04X", palettes[i].colors[j]);
            /* If we aren't done, add a separator*/
            if (j + 1 < palettes[i].size)
            {
                fprintf(c_file, ", ");
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
    uint32_t palette_index = 0;
    DIR *dir;
    struct dirent *dir_entry;

    /* Argument reading and processing */
    if (!parse_params(argc, argv, &params))
    {
        return EXIT_FAILURE;
    }
    if (!params.src_path)
    {
        fprintf(stderr, "%s: no source path specified\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (!params.dest_path)
    {
        /* If no destination path especified, use the current dir */
        params.dest_path = ".";
    }
    if (!params.dest_name)
    {
        fprintf(stderr, "%s: no destination name specified\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Source palettes folder reading */
    dir = opendir(params.src_path);
    if (dir == NULL)
    {
        return EXIT_FAILURE;
    }
    printf(version_text);
    printf("\nReading files...\n");
    while ((dir_entry = readdir(dir)) != NULL)
    {
        /* Process only regular files */
        if (dir_entry->d_type == DT_REG)
        {
            if (!palette_read(params.src_path, dir_entry->d_name, palette_index))
            {
                printf("\tPng file to pal: %s -> %s\n", dir_entry->d_name,
                       palettes[palette_index].name);
                ++palette_index;
            }
        }
    }
    printf("%d palettes readed.\n", palette_index);
    closedir(dir);

    if (palette_index > 0)
    {
        printf("Building C header file...\n");
        build_header_file(params.dest_path, params.dest_name, palette_index);
        printf("Building C source file...\n");
        build_source_file(params.dest_path, params.dest_name, palette_index);
        printf("Done.\n");
    }

    return EXIT_SUCCESS;
}
