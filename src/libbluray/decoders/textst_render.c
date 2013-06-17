/*
 * This file is part of libbluray
 * Copyright (C) 2013  Petri Hintukainen <phintuka@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "util/macro.h"
#include "util/logging.h"
#include "libbluray/bluray.h"          /* bd_char_code_e */

#include <stdint.h>

#ifdef HAVE_FT2
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

#include "textst_render.h"

/*
 *
 */

#define TEXTST_ERROR(...) BD_DEBUG(DBG_GC | DBG_CRIT, __VA_ARGS__)
#define TEXTST_TRACE(...) BD_DEBUG(DBG_GC,            __VA_ARGS__)

/*
 * data
 */

struct textst_render {
#ifdef HAVE_FT2
  FT_Library     ft_lib;

  unsigned       font_count;
  FT_Face       *face;

  bd_char_code_e char_code;
#endif
};

/*
 * init / free
 */

TEXTST_RENDER *textst_render_init(void)
{
#ifdef HAVE_FT2
    TEXTST_RENDER *p = calloc(1, sizeof(TEXTST_RENDER));

    if (!FT_Init_FreeType(&p->ft_lib)) {
        return p;
    }

    X_FREE(p);
    TEXTST_ERROR("Loading FreeType2 failed\n");
#else
    TEXTST_ERROR("TextST font support not compiled in\n");
#endif
    return NULL;
}

void textst_render_free(TEXTST_RENDER **pp)
{
    if (pp && *pp) {
#ifdef HAVE_FT2
        TEXTST_RENDER *p = *pp;

        if (p->ft_lib) {
            /* free fonts */
            unsigned ii;
            for (ii = 0; ii < p->font_count; ii++) {
                if (p->face[ii]) {
                    FT_Done_Face(p->face[ii]);
                }
            }

            FT_Done_FreeType(p->ft_lib);
        }
#endif
        X_FREE(*pp);
    }
}

/*
 * settings
 */

int textst_render_add_font(TEXTST_RENDER *p, const char *file)
{
#ifdef HAVE_FT2
    p->face = realloc(p->face, sizeof(*(p->face)) * (p->font_count + 1));

    if (FT_New_Face(p->ft_lib, file, -1, NULL)) {
        TEXTST_ERROR("Unsupport font file format (%s)\n", file);
        return -1;
    }

    if (!FT_New_Face(p->ft_lib, file, 0, &p->face[p->font_count])) {
        p->font_count++;
        return 0;
    }

    TEXTST_ERROR("Loading font %s failed\n", file);
#endif
    return -1;
}

int textst_render_set_char_code(TEXTST_RENDER *p, int char_code)
{
    p->char_code = (bd_char_code_e)char_code;
    if (p->char_code != BLURAY_TEXT_CHAR_CODE_UTF8) {
        TEXTST_ERROR("WARNING: unsupported TextST coding type %d\n", char_code);
        return -1;
    }

    return 0;
}

/*
 * rendering
 */

static int _utf8_char_size(const uint8_t *s)
{
    if ((s[0] & 0xE0) == 0xC0 &&
        (s[1] & 0xC0) == 0x80) {
        return 2;
    }
    if ((s[0] & 0xF0) == 0xE0 &&
        (s[1] & 0xC0) == 0x80 &&
        (s[2] & 0xC0) == 0x80) {
        return 3;
    }
    if ((s[0] & 0xF8) == 0xF0 &&
        (s[1] & 0xC0) == 0x80 &&
        (s[2] & 0xC0) == 0x80 &&
        (s[3] & 0xC0) == 0x80) {
        return 4;
    }
    return 1;
}

static unsigned _utf8_char_get(const uint8_t *s, int char_size)
{
    if (!char_size) {
        char_size = _utf8_char_size(s);
    }

    switch (char_size) {
        case 2: return ((s[0] & 0x1F) <<  6) | ((s[1] & 0x3F));
        case 3: return ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) <<  6) | ((s[2] & 0x3F));
        case 4: return ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | ((s[3] & 0x3F));
        default: ;
    }
    return s[0];
}

static int _draw_string(FT_Face face, const uint8_t *string, int length,
                        uint8_t *mem, int x, int y, int width, int height, int stride,
                        int color)
{
#ifdef HAVE_FT2
    unsigned char_code;
    int      ii, jj, kk;

    if (length <= 0) {
        return -1;
    }

    for (ii = 0; ii < length; ii++) {
        /*if (p->char_code == BLURAY_TEXT_CHAR_CODE_UTF8) {*/
            int char_size = _utf8_char_size(string + ii);
            char_code = _utf8_char_get(string + ii, char_size);
            ii += char_size - 1;
        /*}*/

        if (FT_Load_Char(face, char_code, FT_LOAD_RENDER /*| FT_LOAD_MONOCHROME*/) == 0) {
            for (jj = 0; jj < face->glyph->bitmap.rows; jj++) {
                for (kk = 0; kk < face->glyph->bitmap.width; kk++) {
                    uint8_t pixel = face->glyph->bitmap.buffer[jj * face->glyph->bitmap.pitch + kk];
                    if (pixel & 0x80) {
                        int xpos = x + face->glyph->bitmap_left + kk;
                        int ypos = y - face->glyph->bitmap_top + jj;
                        if (xpos >= 0 && xpos < width && ypos >= 0 && ypos < height) {
                            mem[xpos + ypos * stride] = color;
                        }
                    }
                }
            }
            x += face->glyph->metrics.horiAdvance >> 6;
        }
    }
#endif /* HAVE_FT2 */
    return x;
}


int textst_render(TEXTST_RENDER *p,
                  TEXTST_BITMAP *bmp,
                  const BD_TEXTST_REGION_STYLE *style,
                  const BD_TEXTST_DIALOG_REGION *region)
{
    FT_Face                face;
    BD_TEXTST_REGION_STYLE s;   /* current style settings */

    /* settings can be changed and reset with inline codes. Use local copy. */
    memcpy(&s, style, sizeof(s));

    /* fonts loaded ? */
    if (p->font_count < 1) {
        TEXTST_ERROR("textst_render: no fonts loaded\n");
        return -1;
    }

    /* TODO: */
    if (s.text_flow != BD_TEXTST_FLOW_LEFT_RIGHT) {
        TEXTST_ERROR("textst_render: unsupported text flow type %d\n", s.text_flow);
    }
    if (bmp->argb) {
        TEXTST_ERROR("textst_render: ARGB output not implemented\n");
        return -1;
    }
    if (s.font_style.bold) {
        TEXTST_ERROR("textst_render: unsupported style: bold\n");
    }
    if (s.font_style.italic) {
        TEXTST_ERROR("textst_render: unsupported style: italic\n");
    }
    if (s.font_style.outline_border) {
        TEXTST_ERROR("textst_render: unsupported style: outline\n");
    }

    /* select font */
    if (s.font_id_ref >= p->font_count || !p->face[s.font_id_ref]) {
        TEXTST_ERROR("textst_Render: incorrect font index %d\n", s.font_id_ref);
        face = p->face[0];
    } else {
        face = p->face[s.font_id_ref];
    }
    FT_Set_Char_Size(face, 0, s.font_size << 6, 0, 0);

    /* */

    unsigned  ee;
    uint8_t  *ptr = (uint8_t*)region->elem;
    int       xpos = 0;
    int       ypos = 0;

    /* */

    /* vertical alignment */
    switch (s.text_valign) {
        case BD_TEXTST_VALIGN_TOP:
            break;
        case BD_TEXTST_VALIGN_BOTTOM:
            ypos = s.text_box.height - region->line_count * s.line_space;
TEXTST_ERROR("adjust lines: %d px to bottom\n", ypos);
            break;
        case BD_TEXTST_VALIGN_MIDDLE:
            ypos = (s.text_box.height - region->line_count * s.line_space) / 2;
TEXTST_ERROR("adjust lines: %d px to middle\n", ypos);
            break;
        default:
            TEXTST_ERROR("textst_render: unsupported vertical align %d\n", s.text_halign);
            break;
    }
    ypos += (face->size->metrics.ascender >> 6) + 1;

    /* horizontal alignment (per line) */
    if (s.text_halign != BD_TEXTST_HALIGN_LEFT) {
        TEXTST_ERROR("textst_render: unsupported horizontal align %d\n", s.text_halign);
    }

    /* TODO: need to get max height for all fonts in line */
    /* TODO: styles: see ex. vlc/modules/text_renderer/freetype.c ; function GetGlyph() */

    for (ee = 0; ee < region->elem_count; ee++) {
        BD_TEXTST_DATA *elem = (BD_TEXTST_DATA*)ptr;
        switch (elem->type) {
            case BD_TEXTST_DATA_STRING:
                xpos = _draw_string(face, elem->data.text.string, elem->data.text.length,
                                    bmp->mem, xpos, ypos, bmp->width, bmp->height, bmp->stride, s.font_color);
                ptr += elem->data.text.length;
                break;

            case BD_TEXTST_DATA_NEWLINE:
                ypos += style->line_space;
                xpos = 0;
                break;

            case BD_TEXTST_DATA_FONT_ID:
                if (elem->data.font_id_ref >= p->font_count || !p->face[elem->data.font_id_ref]) {
                    TEXTST_ERROR("textst_Render: incorrect font index %d\n", elem->data.font_id_ref);
                } else {
                    TEXTST_ERROR("Font change not implemented\n");
                    //s.font_id_ref = elem->data.font_idx;
                    //face = p->face[s.font_id_ref];
                    //FT_Set_Char_Size(face, 0, s.font_size << 6, 0, 0);
                }
                break;

            case BD_TEXTST_DATA_FONT_STYLE:
                TEXTST_ERROR("Font style change not implemented\n");
                break;

            case BD_TEXTST_DATA_FONT_SIZE:
                TEXTST_ERROR("Font size change not implemented\n");
                break;

            case BD_TEXTST_DATA_FONT_COLOR:
                s.font_color = elem->data.font_color;
                break;

            case BD_TEXTST_DATA_RESET_STYLE:
                memcpy(&s, style, sizeof(s));
                FT_Set_Char_Size(face, 0, s.font_size << 6, 0, 0);
                break;

            default:
                TEXTST_ERROR("Unknown control code %d\n", elem->type);
                break;
        }
        ptr += sizeof(BD_TEXTST_DATA);
    }

    return 0;
}
