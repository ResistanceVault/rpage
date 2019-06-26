/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#ifndef COLOR_ROUTINES
#define COLOR_ROUTINES

#include "rpage/utils.h"
#include "rpage/amiga/includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>

#define FADE_STEP_DEFAULT 16

UWORD color_to_depth(UWORD colors);
UWORD mix_rgb4_colors(UWORD A, UWORD B, USHORT n);
void mix_rgb4_palette_to_black(struct ViewPort *vp, UWORD *pal, UWORD pal_size, UWORD fade);
void mix_rgb4_palette_to_black_as_rgb8(struct ViewPort *vp, UWORD *pal, UWORD pal_size, ULONG rgb8color, UWORD fade);
ULONG rgb4_to_rgb8(UWORD A);
UWORD rgb8_to_rgb4(ULONG A);
ULONG mix_rgb8_colors(ULONG A, ULONG B, UWORD n);
ULONG add_rgb8_colors(ULONG A, ULONG B);
ULONG divide_rgb8_colors(ULONG A, UWORD n);
void set_palette_to_black(struct ViewPort *vp, UWORD first_color, UWORD last_color);
void set_palette_to_grey(struct ViewPort *vp, UWORD first_color, UWORD last_color);
void set_palette(struct ViewPort *vp, UWORD **palette, UWORD first_color, UWORD last_color);
void fadein_rgb4_palette(struct ViewPort *current_viewport, PALETTEPTR current_palette, UWORD pal_size, unsigned short steps);
void fadeout_rgb4_palette(struct ViewPort *current_viewport, PALETTEPTR current_palette, UWORD pal_size, unsigned short steps);
void fade_rgb4_palettes(struct ViewPort *current_viewport, PALETTEPTR source_palette, PALETTEPTR dest_palette, UWORD pal_size, unsigned short steps);
void mix_rgb4_palettes(struct ViewPort *vp, PALETTEPTR source_palette, PALETTEPTR dest_palette, UWORD pal_size, UWORD fade);

#endif
#endif