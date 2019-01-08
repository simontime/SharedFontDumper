#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <switch.h>
#include <ft2build.h>

#include FT_FREETYPE_H

#define MEMSIZE 0x1100000

#define RED RGBA8_MAXALPHA(0xff, 0, 0)
#define GREEN RGBA8_MAXALPHA(0, 0xff, 0)
#define WHITE RGBA8_MAXALPHA(0xff, 0xff, 0xff)
#define SKYBLUE RGBA8_MAXALPHA(0xa0, 0xff, 0xff)

#define DIRNAME "fonts"
#define SUCCESS "Successfully dumped shared fonts!\n\nPress + to exit."
#define IOERROR "Error: Failed to open file for writing.\n"

void draw_glyph(FT_Bitmap *bitmap, u32 x, u32 y, u32 color);
void draw_text(u32 x, u32 y, u32 color, const char *str);
void text_init();
void text_update();
void text_exit();
