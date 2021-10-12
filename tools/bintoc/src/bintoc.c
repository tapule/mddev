/* SPDX-License-Identifier: MIT */
/**
 * MDDev development kit
 * Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021 
 * Github: https://github.com/tapule/mddev
 *
 * bintoc v0.01
 *
 * A binary to C language resource converter
 *
 * Convert binary files to C language data structures
 *
 * Usage example: bintoc -s src/path -d dest/path -n res_bin -t u8 -ma 16 -sa 48
 * 
 * It processes files in "src/path/*" to extract binary data and to build C
 * language data structures. It generates the C source files "res_bin.h" and
 * "res_bin.c" in "dest/path" directory.
 * For each file, bintoc adds a define with the total data size in the data type
 * specified with the parameter -t (or uint8_t by default) and a const data type
 * array containing the binary data aligned to the data type size. If -ms is
 * specified, the binary data will bie aligned to this size too, filling with
 * zeroes up to the needed size. If -ma is used, the array will be aligned in
 * memory to that size.
 * 
 * If -s parameter is not specified, the current directory will be used as
 * source folder.
 * If -d parameter is not specified, the current directory will be used as
 * destination folder.
 * 
 * If "myfile.bin" is in "src/path" and its size is 88 bytes the previos example
 * usage generates:
 * 
 * dest/path/res_bin.h
 * #ifndef RES_BIN_H
 * #define RES_BIN_H
 *
* #include <stdint.h>
 *
 * #define RES_BIN_MYFILE_SIZE    64
 *
 * extern const uint8_t res_bin_myfile[RES_BIN_MYFILE_SIZE];
 *
 * #endif // RES_BIN_H
 * 
 * dest/path/res_bin.c
 * #include "res_bin.h"
 *
 * _Aling(16) const uint8_t res_bin_myfile[RES_BIN_MYFILE_SIZE] = {
 *    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 
 *    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 
 *    0x04, 0x03, 0x00, 0x00, 0x00, 0x58, 0x47, 0x6C, 0xED, 0x00, 0x00, 0x29, 
 *    0xA8, 0x7A, 0x54, 0x58, 0x74, 0x52, 0x61, 0x77, 0x20, 0x70, 0x72, 0x6F, 
 *    0x64, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x47, 0x49, 0x4D, 0x50, 0x57, 
 *    0x81, 0x0E, 0x17, 0x00, 0x00, 0x00, 0x23, 0x49, 0x44, 0x41, 0x54, 0x08, 
 *    0xD7, 0x63, 0x30, 0x06, 0x02, 0x06, 0x13, 0x67, 0x17, 0x20, 0xE1, 0xE2, 
 *    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
 * };
 *
 * You can extract binary data from a unique file too:
 *  bintoc -s pngs/path/file.bin -d dest/path
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <inttypes.h>
#include <ctype.h>

#define MAX_FILES               512	    /* Enough?? */
#define MAX_FILE_NAME_LENGTH    128     /* Max length for file names */
#define MAX_PATH_LENGTH         1024    /* Max length for paths */

#define PARAMS_ERROR            0   /* Error en procesado de parámetros */
#define PARAMS_STOP             1   /* Procesado de parámetros ok, finalizar */
#define PARAMS_CONTINUE         2   /* Procesado de parámetros ok, procesar */

const char version_text [] =
    "bintoc v0.01\n"
    "A binary to C language resource converter\n"
    "Coded by: Juan Ángel Moreno Fernández (@_tapule) 2021\n"
    "Github: https://github.com/tapule/mddev\n";

const char help_text [] =
    "Usage: bintoc [options]\n"
    "\n"
    "Options:\n"
    "  -v, --version       Show version information and exit\n"
    "  -h, --help          Show this help message and exit\n"
    "  -s <path>|<file>    Use a directory path to look for files to convert\n"
    "                      or the unique file path to convert\n"
    "                      Current directory will be used as default\n"
    "  -d <path>           Use a path to save generated C source files\n"
    "                      The current directory will be used as default\n"
    "  -n <name>           Use name as prefix for files, defines, vars, etc\n"
    "                      If it is not specified, \"bins\" will be used as\n"
    "                      default for multiple files. Source file name itself\n"
    "                      will be used if there is only one source file\n"
    "  -t <u8|u16|u32>     Set the data type to use in the conversion\n"
    "     <s8|s16|s32>     uint8_t will be used as default data type\n"
    "  -ma <integer>       Set a memory alignment to use in the conversion\n"
    "  -sa <integer>       Set a data size alignment for the converted data\n";

/* Stores the input parameters */
typedef struct params_t
{
    char *src_path;         /* Folder with the source files */
    char *dest_path;        /* Destination folder for the generated .h and .c */
    char *dest_name;        /* Base name prefix for the generated files */
    char *data_type;        /* Destination data type name, default uint8_t */
    uint8_t type_size;      /* Destination type size in bytes, default 1 */
    int32_t memory_align;   /* Memory alignment size in bytes, default none */
    int32_t size_align;     /* Size alignment in bytes, default none */
} params_t;

/* Stores files's data */
typedef struct file_t
{
    char file[MAX_FILE_NAME_LENGTH];           /* Original file name */
    char name[MAX_FILE_NAME_LENGTH];           /* Name without the extension */
    char size_define[MAX_FILE_NAME_LENGTH];    /* Size constant define name  */
    uint8_t *data;                             /* Data storage */
    uint16_t size;                             /* Data size in selected type */
} file_t;

/* Global storage for the parsed files */
file_t files[MAX_FILES];

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
        /* Source path where files to convert are */
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
        /* Data type for the generated .h and .c files */
        else if (strcmp(argv[i], "-t") == 0)
        {
            if (i < argc - 1)
            {
                if (!strcmp(argv[i + 1], "u8"))
                {
                    params->data_type = "uint8_t";
                    params->type_size = 1;
                }
                else if (!strcmp(argv[i + 1], "u16"))
                {
                    params->data_type = "uint16_t";
                    params->type_size = 2;
                }
                else if (!strcmp(argv[i + 1], "u32"))
                {
                    params->data_type = "uint32_t";
                    params->type_size = 4;
                }
                else if (!strcmp(argv[i + 1], "s8"))
                {
                    params->data_type = "int8_t";
                    params->type_size = 1;
                }
                else if (!strcmp(argv[i + 1], "s16"))
                {
                    params->data_type = "int16_t";
                    params->type_size = 2;
                }
                else if (!strcmp(argv[i + 1], "s32"))
                {
                    params->data_type = "int32_t";
                    params->type_size = 4;
                }
                else
                {
                    fprintf(stderr, "%s: unknown argument %s for this option: '%s'\n",
                        argv[0], argv[i+1], argv[i]);
                    return PARAMS_ERROR;                    
                }
                ++i;
            }
            else
            {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n",
                        argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        }
        /* Memory alignment to use in the conversion */
        else if (strcmp(argv[i], "-ma") == 0)
        {
            if (i < argc - 1)
            {
                params->memory_align = (int32_t) strtoimax(argv[i + 1], NULL, 0);
                if (params->memory_align < 0)
                {
                    params->memory_align = 0;
                }
                ++i;
            }
            else
            {
                fprintf(stderr, "%s: an argument is needed for this option: '%s'\n",
                        argv[0], argv[i]);
                return PARAMS_ERROR;
            }
        }    
        /* Size alignment to use in the conversion */
        else if (strcmp(argv[i], "-sa") == 0)
        {
            if (i < argc - 1)
            {
                params->size_align = (int32_t) strtoimax(argv[i + 1], NULL, 0);
                if (params->size_align < 0)
                {
                    params->size_align = 0;
                }
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
 * @brief Processes a file and converts it to binary aligned data
 * 
 * @param path File path
 * @param file File to process
 * @param type_size The size in bytes of our data type
 * @param size_align The file size alignment needed in the binary data
 * @param file_index Index in the files array to store the data
 * @return true if everythig was correct, false otherwise
 */
bool file_process(const char* path, const char *file, const uint8_t type_size,
                  const uint32_t size_align, const uint32_t file_index)
{
    char file_path[MAX_PATH_LENGTH];
    FILE *p_file;
    uint32_t file_size;
    uint32_t data_size;
    char *file_ext;

    /* Builds the complete file path */
    strcpy(file_path, path);
    strcat(file_path, "/");
    strcat(file_path, file);
    printf("File %s\n", file_path);

    p_file = fopen(file_path, "rb");
    if (!p_file)
    {
        printf("\tError reading file: %s\n", file_path);
        return false;
    }

    /* Get the file size */
    fseek(p_file, 0, SEEK_END);
    file_size = ftell(p_file);
    fseek(p_file, 0, SEEK_SET);
    data_size = file_size;

    /* Align the output size to selected type's size */
    if (type_size > 1)
    {
        data_size = (data_size + (type_size - 1));
        data_size = data_size / type_size;
        data_size = data_size * type_size;
    }
    /* Align the output size to the selected size of alignment */
    if (size_align > 0)
    {
        data_size = (data_size + (size_align - 1));
        data_size = data_size / size_align;
        data_size = data_size * size_align;
    }

    files[file_index].size = data_size / type_size;
    files[file_index].data = malloc(data_size);
    memset(files[file_index].data, 0, data_size);
    fread(files[file_index].data, 1, data_size, p_file);

    /* Save the file original name */
    strcpy(files[file_index].file, file);

    /* Save the file name without the extension in the file struct */
    strcpy(files[file_index].name, file);
    file_ext = strrchr(files[file_index].name, '.');
    if (file_ext)
    {
        *file_ext = '\0';
    }

    return true;
}

/**
 * @brief Builds the C header file for the generated files
 * 
 * @param path Destination path for the .h file
 * @param name Base name for the .h file (name + .h)
 * @param data_type The data type name to use en the written data
 * @param use_prefix Indicate if a prefix should be used for vars, etc.
 * @param file_count Number of files to process from the global files storage
 * @return true if everythig was correct, false otherwise
 */
bool build_header_file(const char *path, const char *name, const char *data_type,
                       const bool use_prefix, const uint32_t file_count)
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
        printf("\tError building C header: file %s can't be created\n", buff);
        return false;
    }

    /* An information message */
    fprintf(h_file, "/* Generated with bintoc v0.01                           */\n");
    fprintf(h_file, "/* A binary to C language resource converter             */\n");
    fprintf(h_file, "/* Github: https://github.com/tapule/mddev               */\n\n");

    /* Header include guard */
    strcpy(buff, name);
    strtoupper(buff);
    strcat(buff, "_H");
    fprintf(h_file, "#ifndef %s\n", buff);
    fprintf(h_file, "#define %s\n\n", buff);
    fprintf(h_file, "#include <stdint.h>\n\n");

    /* Files size defines */
    for (i = 0; i < file_count; ++i)
    {
        files[i].size_define[0] = '\0';
        if (use_prefix)
        {
            strcpy(files[i].size_define, name);
            strcat(files[i].size_define, "_");
        }
        strcat(files[i].size_define, files[i].name);
        strcat(files[i].size_define, "_SIZE");
        strtoupper(files[i].size_define);
        fprintf(h_file, "#define %s    %d\n", files[i].size_define,
                files[i].size);
    }
    fprintf(h_file, "\n");

    /* File declarations */   
    for (i = 0; i < file_count; ++i)
    {
        buff[0] = '\0';
        if (use_prefix)
        {
            strcpy(buff, name);
            strcat(buff, "_");
        }
        strcat(buff, files[i].name);
        fprintf(h_file, "extern const %s %s[%s];\n", data_type, buff,
                files[i].size_define);
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
 * @brief Builds the C source file for the extracted files
 * 
 * @param path Destinatio path for the .c file
 * @param name Base name for the .c file (name + .c)
 * @param data_type The data type name to use en the written data
 * @param type_size The size in bytes of our data type
 * @param memory_align The memory alignment to export in the .c file
 * @param use_prefix Indicate if a prefix should be used for files, vars, etc.
 * @param file_count Number of files to process from the global files storage
 * @return true if everythig was correct, false otherwise
 */
bool build_source_file(const char *path, const char *name, const char *data_type,
                       const uint8_t type_size, const int32_t memory_align,
                       const bool use_prefix, const uint32_t file_count)
{
    FILE *c_file;
    char buff[1024];
    uint32_t i;
    uint32_t j;
    uint32_t k;
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
    line_feed = 12 - ((type_size / 2) * 3);

    /* File definitions writting */
    for (i = 0; i < file_count; ++i)
    {
        buff[0] = '\0';
        if (use_prefix)
        {
            strcpy(buff, name);
            strcat(buff, "_");
        }
        strcat(buff, files[i].name);
        /* Add alignment if there was any */
        if (memory_align > 1)
        {
            fprintf(c_file, "_Align(%d) ", memory_align);
        }
        fprintf(c_file, "const %s %s[%s] = {", data_type, buff,
                files[i].size_define);
        /* We have the file size aligned to the data type size */
        for (j = 0; j < files[i].size; ++j)
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
            /* Write the value byte by byte */
            fprintf(c_file, "0x");
            for (k = 0; k < type_size; ++k)
            {
                fprintf(c_file, "%02X", files[i].data[(j * type_size) + k]);
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
    uint32_t file_index = 0;
    DIR *dir;
    char *file_name;
    struct dirent *dir_entry;
    uint8_t params_status;

    /* Set default values here */
    params.src_path = ".";
    params.dest_path = ".";
    params.data_type = "uint8_t";
    params.type_size = 1;
    params.memory_align = 1;
    params.size_align = 1;

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

    /* Checks that type size and size alignment are compatibles */
    if (params.type_size > 1 && params.size_align > 1)
    {
        if (params.size_align % params.type_size)
        {
            fprintf(stderr, "Error: Incompatible type size and size alignment\n");
            return EXIT_FAILURE;
        }
    }

    /* First try to open source path as a directory */
    dir = opendir(params.src_path);
    if (dir != NULL)
    {
        printf(version_text);
        printf("\nReading files...\n");
        while ((dir_entry = readdir(dir)) != NULL)
        {
            /* Checks max allowed files */
            if (file_index >= MAX_FILES)
            {
                closedir(dir);
                fprintf(stderr, "Error: More than %d files in the source directory\n", MAX_FILES);
                return EXIT_FAILURE;
            }            
            /* Process only regular files */
            if (dir_entry->d_type == DT_REG)
            {
                if (file_process(params.src_path, dir_entry->d_name, 
                                 params.type_size, params.size_align, file_index))
                {
                    printf("\tFile to binary: %s -> %s\n", dir_entry->d_name, 
                        files[file_index].name);
                    ++file_index;
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
        if (file_process(params.src_path, file_name, params.type_size,
                         params.size_align, file_index))
        {
            printf("\tFile to binary: %s -> %s\n", file_name, 
                files[file_index].name);
            ++file_index;
        }
    }

    printf("%d files readed.\n", file_index);

    /* Files already processed, continue with .c and .h building */
    if (file_index > 0)
    {
        /* By default use BASE_NAME as prefix for files, defines, vars, etc */
        bool use_prefix = true;

        /* Adjust the destination base name if it was not specified */
        if (!params.dest_name)
        {
            /* Only one file, use its name as base name and no prefix */
            if (file_index == 1)
            {
                params.dest_name = files[0].name;
                use_prefix = false;
            }
            /* More than one file, use "bins" as base name */
            else
            {
                params.dest_name = "bins";
            }
        }

        printf("Building C header file...\n");
        build_header_file(params.dest_path, params.dest_name, params.data_type,
                          use_prefix, file_index);
        printf("Building C source file...\n");
        build_source_file(params.dest_path, params.dest_name, params.data_type,
                          params.type_size, params.memory_align, use_prefix,
                          file_index);
        printf("Done.\n");
    }

    return EXIT_SUCCESS;
}
