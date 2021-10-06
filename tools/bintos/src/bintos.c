/* SPDX-License-Identifier: MIT */
/**
 * SGKD
 * Coded by: Stephane Dallongeville
 * Github: https://github.com/Stephane-D/SGDK
 * Modified version by: Juan Ángel Moreno Fernández (@_tapule) 2021
 * Github: https://github.com/tapule/mddev
 *
 * bintos
 *
 * A binary to m68k assembler resource converter
 *
 * Converts a binary file to a m68k assembler resource .s file
 *
 * Usage example: 
 * 
 * It extracts tilesets in "pngs/path/*.png" and generates the C source files
 * "base_name.h" and "base_name.c" in "dest/path" directory.
 * For each png file, tilesettool adds a define with its size in tiles and a
 * const uint32_t array containing the tileset data (one tile a row).
 * 
 * If -p parameter is not specified, the current directory will be used as
 * destination folder.
 * 
 * If "mytileset.png" is in "pngs/path" the previos example usage generates:
 * 
 * res_til.h
 * #ifndef RES_TIL_H
 * #define RES_TIL_H
 *
 * #include <stdint.h>
 *
 * #define RES_TIL_MYTILESET_SIZE    3
 *
 * extern const uint32_t res_til_mytileset[RES_TIL_MYTILESET_SIZE * 8];
 *
 * #endif // RES_TIL_H
 * 
 * res_til.c
 * #include "res_til.h"
 *
 * const uint32_t res_til_mytileset[RES_TIL_MYTILESET_SIZE * 8] = {
 *    0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 
 *    0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111211,
 *    0x11111221, 0x11111221, 0x11111222, 0x21111222, 0x21112221, 0x21112211, 0x21111111, 0x21111111
 * };
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>


static char *getFilename(char *pathname)
{
    char* fname = strrchr(pathname, '/');

    if (fname)
    {
        return fname + 1;
    }
    return pathname;
}

static char *getFileExtension(char *pathname)
{
    char* fname = strrchr(pathname, '.');

    if (fname)
    {
        return fname + 1;
    }
    return "";
}

static void removeExtension(char *pathname)
{
    char* fname = strrchr(pathname, '.');

    if (fname)
    {
        *fname = 0;
    }
}

int main(int argc, char **argv)
{
    int ii, jj;
    int len;
    int total;
    int align;
    int formatint;
    int formatintasm;
    int nullfill;
    char *format;
    char *formatasm;
    char *shortname;
    char *ext;
    char *FileName;
    char *FileNameOut;
    FILE *FileInput;
    FILE *FileOutput;
    char path[4096];
    unsigned char temp[4096];

    // default
    FileName = "";
    FileNameOut = "";
    format = "uint8_t";
    formatint = 1;
    formatasm = "w";
    formatintasm = 2;
    total = 0;
    align = 2;
    nullfill = 0;

    // parse parmeters
    for (ii = 1; ii < argc; ii++)
    {
        if (!strcmp(argv[ii], "-u8"))
        {
            format = "uint8_t";
            formatint = 1;
            formatasm = "w";
            formatintasm = 2;
        }
        else if (!strcmp(argv[ii], "-s8"))
        {
            format = "int8_t"; 
            formatint = 1;
            formatasm = "w";
            formatintasm = 2;
        }
        else if (!strcmp(argv[ii], "-u16"))
        {
            format = "uint16_t";
            formatint = 2;
            formatasm = "w";
            formatintasm = 2;
        }
        else if (!strcmp(argv[ii], "-s16"))
        {
            format = "int16_t";
            formatint = 2;
            formatasm = "w";
            formatintasm = 2;
        }
        else if (!strcmp(argv[ii], "-u32"))
        {
            format = "uint32_t";
            formatint = 4;
            formatasm = "l";
            formatintasm = 4;
        }
        else if (!strcmp(argv[ii], "-s32"))
        {
            format = "int32_t";
            formatint = 4;
            formatasm = "l";
            formatintasm = 4;
        }
        else if (!strcmp(argv[ii], "-align"))
        {
            ii++;
            align = strtoimax(argv[ii], NULL, 0);

            if (!align) align = 2;
        }
        else if (!strcmp(argv[ii], "-nullfill"))
        {
            ii++;
            nullfill = strtoimax(argv[ii], NULL, 0);
        }
        else if (!FileName[0])
        {
            FileName = argv[ii];
        }
        else if (!FileNameOut[0])
        {
            FileNameOut = argv[ii];
        }
    }

    if (!FileNameOut[0])
    {
        FileNameOut = strdup(FileName);
    }

    ext = getFileExtension(FileName);

    FileInput = fopen(FileName, "rb");

    if (!FileInput)
    {
        printf("Couldn't open input file %s\n", FileName);
        return 1;
    }

    // remove extension
    removeExtension(FileNameOut);
    // keep only file name
    shortname = getFilename(FileNameOut);

    // build output .s file
    strcpy(path, FileNameOut);
    strcat(path, ".s");
    FileOutput = fopen(path, "w");

    if (!FileOutput)
    {
        fclose(FileInput);
        printf("Couldn't open output file %s\n", path);
        return 1;
    }

    // force align on 2
    if (align < 2)
    {
        align = 2;
    }

    fprintf(FileOutput, ".section .rodata\n\n");

    fprintf(FileOutput, "    .align  %d\n\n", align);
    fprintf(FileOutput, "    .global %s\n", shortname);
    fprintf(FileOutput, "%s:\n", shortname);

    while (1)
    {
        // start by setting buffer to nullfill
        memset(temp, nullfill, sizeof(temp));

        len = fread(temp, 1, sizeof(temp), FileInput);
        total += len;

         // align length for size of units
        len = (len + (formatintasm - 1)) & ~(formatintasm - 1);

        if (len)
        {
            fprintf(FileOutput, "    dc.%s    ", formatasm);

            for (ii = 0; ii < (len / formatintasm); ii++)
            {
                if (ii)
                {
                    fprintf(FileOutput, ",");
                }

                fprintf(FileOutput, "0x");

                for (jj = 0; jj < formatintasm; jj++)
                {
                    fprintf(FileOutput, "%02X", temp[(ii * formatintasm) + jj]);
                }
            }
            fprintf(FileOutput, "\n");
        }

        if (len < 16)
        {
            break;
        }
    }

    fprintf(FileOutput, "\n");
    fclose(FileOutput);
    fclose(FileInput);

    // now make .h file
    strcpy(path, FileNameOut);
    strcat(path, ".h");
    FileOutput = fopen(path, "w");

    if (!FileOutput)
    {
        printf("Couldn't open output file %s\n", path);
        return 1;
    }

    for (ii = 0; ii < strlen(shortname); ii++)
    {
        path[ii] = toupper(shortname[ii]);
    }

    path[ii] = 0;

    /* An information message */
    fprintf(FileOutput, "/* Generated with bintos                               */\n");
    fprintf(FileOutput, "/* a binary to m68k assembler resource converter       */\n");
    fprintf(FileOutput, "/* Github: https://github.com/tapule/mddev             */\n\n");

    fprintf(FileOutput, "#ifndef %s_H\n", path);
    fprintf(FileOutput, "#define %s_H\n\n", path);
    fprintf(FileOutput, "#include <stdint.h>\n\n");
    fprintf(FileOutput, "#define %s_SIZE    %d\n\n", path, total / formatint);
    fprintf(FileOutput, "extern const %s %s[0x%X];\n\n", format, shortname, total / formatint);
    fprintf(FileOutput, "#endif /* %s_H */\n", path);

    fclose(FileOutput);
    return 0;
}
