/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * paltool v0.03
 *
 * Sega Megadrive/Genesis palette converter
 *
 * Convert indexed png files up to 64 colors to Sega Megadrive/Genesis palette
 * format.
 *
 * Usage example: paltool -s pngs/path -d dest/path -n res_pal
 * 
 * It extracts all palettes in "pngs/path/*.png" and generates the C source
 * files "res_pal.h" and "res_pal.c" in "dest/path" directory.
 * For each png file, paltool adds a define with its size in colors and a const
 * uint16_t array containing the palette color data.
 * 
 * If -s parameter is not specified, the current directory will be used as
 * source folder.
 * If -d parameter is not specified, the current directory will be used as
 * destination folder.
 * 
 * If "mypal.png" is in "pngs/path" the previos example usage generates:
 * 
 * dest/path/res_pal.h
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
 * dest/path/res_pal.c
 * #include "res_pal.h"
 *
 * const uint16_t res_pal_mypal[RES_PAL_MYPAL_SIZE] = {
 *    0x0000, 0x0222, 0x0008, 0x0024, 0x002E, 0x0444, 0x006C, 0x0C80, 
 *    0x0888, 0x008E, 0x02AE, 0x0CCC, 0x0EE8, 0x0AEE, 0x0EEE
 * };
 *
 * You can convert a unique file too:
 *  paltool -s pngs/path/file.png -d dest/path -n res_pal
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

#define PARAMS_ERROR            0   /* Error en procesado de parámetros */
#define PARAMS_STOP             1   /* Procesado de parámetros ok, finalizar */
#define PARAMS_CONTINUE         2   /* Procesado de parámetros ok, procesar */

const char version_text [] =
    "paltool v0.03\n"
    "Sega Megadrive/Genesis palette converter\n"
    "Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021\n"
    "Github: https://github.com/tapule/mddev\n";

const char help_text [] =
    "Usage: paltool [options]\n"
    "\n"
    "Options:\n"
    "  -v, --version       Show version information and exit\n"
    "  -h, --help          Show this help message and exit\n"
    "  -s <path>|<file>    Use a directory path to look for png files\n"
    "                      or a unique png file to extract palettes from"
    "                      Current directory will be used as default\n"
    "  -d <path>           Use a path to save generated C source files\n"
    "                      The current directory will be used as default\n"
    "  -n <name>           Use name as prefix for files, defines, vars, etc\n"
    "                      If it is not specified, \"pal\" will be used as\n"
    "                      default for multiple files. Source file name itself\n"
    "                      will be used if there is only one source file\n";

/* Stores the input parameters */
typedef struct params_t
{
    char *src_path;   /* Folder with the source palettes in png files */
    char *dest_path;  /* Destination folder for the generated .h and .c */
    char *dest_name;  /* Base name for the generated .h and .c files */
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
 * @param use_prefix Indicate if a prefix should be used for vars, etc.
 * @param palette_count Number of palettes to process from the global palettes
 * @return true if everythig was correct, false otherwise
 */
bool build_header_file(const char *path, const char *name,
                       const bool use_prefix, const uint32_t palette_count)
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
    fprintf(h_file, "/* Generated with paltool v0.03                     */\n");
    fprintf(h_file, "/* a Sega Megadrive/Genesis palette converter       */\n");
    fprintf(h_file, "/* Github: https://github.com/tapule/mddev          */\n\n");

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
        palettes[i].size_define[0] = '\0';
        if (use_prefix)
        {
            strcpy(palettes[i].size_define, name);
            strcat(palettes[i].size_define, "_");
        }        
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
        buff[0] = '\0';
        if (use_prefix)
        {
            strcpy(buff, name);
            strcat(buff, "_");
        }
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
 * @param use_prefix Indicate if a prefix should be used for files, vars, etc.
 * @param palette_count Number of palettes to process from the global palettes
 * @return true if everythig was correct, false otherwise
 */
bool build_source_file(const char *path, const char *name,
                       const bool use_prefix, const uint32_t palette_count)
{
    FILE *c_file;
    char buff[1024];
    uint32_t i;
    uint32_t j;
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

    /* Palette definitions */
    for (i = 0; i < palette_count; ++i)
    {
        buff[0] = '\0';
        if (use_prefix)
        {
            strcpy(buff, name);
            strcat(buff, "_");
        }        
        strcat(buff, palettes[i].name);
        fprintf(c_file, "const uint16_t %s[%s] = {", buff,
                palettes[i].size_define);
        for (j = 0; j < palettes[i].size; ++j)
        {
            /* Do we need to write a comma after the las value? */
            if (j)
            {
                fprintf(c_file, ", ");
            }
            /* Every line_feed written values, add a line feed */
            if (j % line_feed == 0)
            {
                fprintf(c_file, "\n    ");
            }            
            fprintf(c_file, "0x%04X", palettes[i].colors[j]);
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
            /* Checks max allowed palettes */
            if (palette_index >= MAX_PALETTES)
            {
                closedir(dir);
                fprintf(stderr, "Error: More than %d files in the source directory\n", MAX_PALETTES);
                return EXIT_FAILURE;
            }
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
        if (!palette_read(params.src_path, file_name, palette_index))
        {
            printf("\tFile to binary: %s -> %s\n", file_name, 
                palettes[palette_index].name);
            ++palette_index;
        }
    }

    printf("%d palettes readed.\n", palette_index);

    if (palette_index > 0)
    {
        /* By default use BASE_NAME as prefix for files, defines, vars, etc */
        bool use_prefix = true;

        /* Adjust the destination base name if it was not specified */
        if (!params.dest_name)
        {
            /* Only one file, use its name as base name and no prefix */
            if (palette_index == 1)
            {
                params.dest_name = palettes[0].name;
                use_prefix = false;
            }
            /* More than one file, use "bins" as base name */
            else
            {
                params.dest_name = "pal";
            }
        }

        printf("Building C header file...\n");
        build_header_file(params.dest_path, params.dest_name, use_prefix,
                          palette_index);
        printf("Building C source file...\n");
        build_source_file(params.dest_path, params.dest_name, use_prefix,
                          palette_index);
        printf("Done.\n");
    }

    return EXIT_SUCCESS;
}
