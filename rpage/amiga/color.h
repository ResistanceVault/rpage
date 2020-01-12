/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#ifndef COLOR_ROUTINES
#define COLOR_ROUTINES

#include "rpage/amiga/screen_size.h"
#include "rpage/utils.h"
#include "rpage/amiga/inc.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>

#define FADE_STEP_DEFAULT 16

typedef UWORD color444;
typedef ULONG color888;

#ifdef VGA_CAPABLE
typedef color888 amiga_color;
#else
typedef color444 amiga_color;
#endif

color444 components_to_rgb4(UWORD r, UWORD g, UWORD b);
UWORD color_to_depth(UWORD colors);
UWORD mix_rgb4_colors(color444 A, color444 B, USHORT n);
void mix_rgb4_palette_to_black(struct ViewPort *vp, color444 *pal, UWORD pal_size, UWORD fade);
// void mix_rgb4_palette_to_black_as_rgb8(struct ViewPort *vp, color444 *pal, UWORD pal_size, color888 rgb8color, UWORD fade);
// void mix_rgb4_palettes(struct ViewPort *vp, amiga_color *source_palette, amiga_color *dest_palette, UWORD pal_size, UWORD fade);
color888 rgb4_to_rgb8(color444 A);
color444 rgb8_to_rgb4(color888 A);
color888 mix_rgb8_colors(color888 A, color888 B, UWORD n);
color888 add_rgb8_colors(color888 A, color888 B);
color888 divide_rgb8_colors(color888 A, UWORD n);
void set_palette_to_black(struct ViewPort *vp, UWORD first_color, UWORD last_color);
void set_palette_to_grey(struct ViewPort *vp, UWORD first_color, UWORD last_color);
void set_palette(struct ViewPort *vp, amiga_color **palette, UWORD first_color, UWORD last_color);
// void fadein_rgb4_palette(struct ViewPort *current_viewport, color444 *current_palette, UWORD pal_size, unsigned short steps);
// void fadeout_rgb4_palette(struct ViewPort *current_viewport, color444 *current_palette, UWORD pal_size, unsigned short steps);
// void fade_rgb4_palettes(struct ViewPort *current_viewport, color444 *source_palette, color444 *dest_palette, UWORD pal_size, unsigned short steps);

#endif
#endif