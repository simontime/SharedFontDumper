/* All code in this document was adapted from shchmue's Lockpick! */

#include "text.h"

static u32 framebuf_width = 0;
static u32 stride;
static u32 *framebuf;
static Framebuffer fb;
static FT_Library library;
static FT_Face face;

void draw_glyph(FT_Bitmap *bitmap, u32 x, u32 y, u32 color) {
    u32 framex, framey;
    u8* imageptr = bitmap->buffer;

    for (u32 tmpy = 0; tmpy < bitmap->rows; tmpy++) {
        for (u32 tmpx = 0; tmpx < bitmap->width; tmpx++) {
            framex = x + tmpx;
            framey = y + tmpy;

            framebuf[framey * framebuf_width + framex] = RGBA8_MAXALPHA(imageptr[tmpx], imageptr[tmpx], imageptr[tmpx]) & color;
        }

        imageptr += bitmap->pitch;
    }
}

void draw_text(u32 x, u32 y, u32 color, const char *str) {
    u32 tmpx = x;
    FT_UInt glyph_index;
    FT_GlyphSlot slot = face->glyph;

    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '\n') {
            tmpx = x;
            y += face->size->metrics.height >> 6;
            continue;
        }

        glyph_index = FT_Get_Char_Index(face, (FT_ULong)str[i]);
        
        if (R_FAILED(FT_Load_Glyph(face, glyph_index, FT_LOAD_COLOR) ||
            FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0))
            return;

        draw_glyph(&slot->bitmap, tmpx + slot->bitmap_left, y - slot->bitmap_top, color);

        tmpx += slot->advance.x >> 6;
        y += slot->advance.y >> 6;
    }
}

void text_init() {
    PlFontData font;
    plGetSharedFontByType(&font, PlSharedFontType_Standard);

    FT_Init_FreeType(&library);
    FT_New_Memory_Face(library, font.address, font.size, 0, &face);
    FT_Set_Char_Size(face, 0, 6*64, 300, 300);

    framebufferCreate(&fb, nwindowGetDefault(), 1280, 720, PIXEL_FORMAT_RGBA_8888, 2);
    framebufferMakeLinear(&fb);
    framebuf = (u32 *)framebufferBegin(&fb, &stride);
    framebuf_width = stride / sizeof(u32);
    memset(framebuf, 0, stride*720);
    framebufferEnd(&fb);
}

void text_update() {
	framebufferBegin(&fb, &stride);
	framebufferEnd(&fb);
}

void text_exit() {
    framebufferClose(&fb);

	FT_Done_Face(face);
    FT_Done_FreeType(library);
}
