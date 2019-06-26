/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#include "frwk/amiga/includes.prl"
#include "frwk/amiga/color.h"
#include "frwk/utils.h"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>

extern struct GfxBase *GfxBase;

UWORD color_to_depth(UWORD colors) {
	UWORD i = 1;
	while ((1 << i) < colors)
		i++;

	return i;
}

UWORD mix_rgb4_colors(UWORD A, UWORD B, USHORT n) {
	UWORD r, g, b, x, y, z;

	if (n == 0)
		return A;

	if (n >= 255)
		return B;

	x = (B & 0x0f00) >> 8;
	y = (B & 0x00f0) >> 4;
	z = B & 0x000f;

	x *= n;
	y *= n;
	z *= n;

	n = 255 - n;

	r = (A & 0x0f00) >> 8;
	g = (A & 0x00f0) >> 4;
	b = A & 0x000f;

	r *= n;
	g *= n;
	b *= n;

	r += x;
	g += y;
	b += z;

	r /= 255;
	g /= 255;
	b /= 255;

	if (r > 0xf)
		r = 0xf;
	if (g > 0xf)
		g = 0xf;
	if (b > 0xf)
		b = 0xf;

	r = r & 0xf;
	g = g & 0xf;
	b = b & 0xf;

	return (UWORD)((r << 8) | (g << 4) | b);
}

ULONG rgb4_to_rgb8(UWORD A) {
	ULONG r, g, b;
	r = ((ULONG)(A & 0x0f00)) << 12;
	g = (A & 0x00f0) << 8;
	b = (A & 0x000f) << 4;

	return (ULONG)(r | g | b);
}

UWORD rgb8_to_rgb4(ULONG A) {
	UWORD r, g, b;
	r = (A & 0xF00000) >> 12; // ((ULONG)(A & 0x0f00)) << 12;
	g = (A & 0x00F000) >> 8;
	b = (A & 0x00F0) >> 4;

	return (UWORD)(r | g | b);
}

ULONG add_rgb8_colors(ULONG A, ULONG B) {
	ULONG r, g, b, x, y, z;

	x = (B & 0xff0000) >> 16;
	y = (B & 0x00ff00) >> 8;
	z = B & 0x000ff;

	r = (A & 0xff0000) >> 16;
	g = (A & 0x00ff00) >> 8;
	b = A & 0x0000ff;

	r += x;
	g += y;
	b += z;

	if (r > 0xFF)
		r = 0xFF;
	if (g > 0xFF)
		g = 0xFF;
	if (b > 0xFF)
		b = 0xFF;

	return (r << 16) | (g << 8) | b;
}

ULONG divide_rgb8_colors(ULONG A, UWORD n) {
	ULONG r, g, b;

	if (n == 0)
		return A;

	r = (A & 0xff0000) >> 16;
	g = (A & 0x00ff00) >> 8;
	b = A & 0x0000ff;

	r /= n;
	g /= n;
	b /= n;

	return (r << 16) | (g << 8) | b;
}

ULONG mix_rgb8_colors(ULONG A, ULONG B, UWORD n) {
	ULONG r, g, b, x, y, z;

	if (n == 0)
		return A;

	if (n >= 255)
		return B;

	x = (B & 0xff0000) >> 16;
	y = (B & 0x00ff00) >> 8;
	z = B & 0x000ff;

	x *= n;
	y *= n;
	z *= n;

	n = 255 - n;

	r = (A & 0xff0000) >> 16;
	g = (A & 0x00ff00) >> 8;
	b = A & 0x0000ff;

	r *= n;
	g *= n;
	b *= n;

	r += x;
	g += y;
	b += z;

	r >>= 8;
	g >>= 8;
	b >>= 8;

	if (r > 0xff)
		r = 0xff;
	if (g > 0xff)
		g = 0xff;
	if (b > 0xff)
		b = 0xff;

	r = r & 0xff;
	g = g & 0xff;
	b = b & 0xff;

	return (r << 16) | (g << 8) | b;
}

void mix_rgb4_palettes(struct ViewPort *vp, PALETTEPTR source_palette, PALETTEPTR dest_palette, UWORD pal_size,
										 UWORD fade) {
	UBYTE i;
	UWORD col;

	for (i = 0; i < pal_size; i++) {
		col = mix_rgb4_colors(source_palette[i], dest_palette[i], fade);
		SetRGB4(vp, i, (col & 0x0f00) >> 8, (col & 0x00f0) >> 4, col & 0x000f);
	}
}

void mix_rgb4_palette_to_black(struct ViewPort *vp, UWORD *pal, UWORD pal_size,
										 UWORD fade) {
	UBYTE i;
	UWORD col;

	for (i = 0; i < pal_size; i++) {
		col = mix_rgb4_colors(pal[i], 0x000, fade);
		SetRGB4(vp, i, (col & 0x0f00) >> 8, (col & 0x00f0) >> 4, col & 0x000f);
	}
}

void mix_rgb4_palette_to_black_as_rgb8(struct ViewPort *vp, UWORD *pal, UWORD pal_size,
																ULONG rgb8color, UWORD fade) {
	UBYTE i;
	UWORD col;

	for (i = 0; i < pal_size; i++) {
		col = mix_rgb8_colors(rgb4_to_rgb8(pal[i]), rgb8color, fade);
		col = rgb8_to_rgb4(col);
		SetRGB4(vp, i, (col & 0x0f00) >> 8, (col & 0x00f0) >> 4, col & 0x000f);
	}
}

void set_palette_to_black(struct ViewPort *vp, UWORD first_color, UWORD last_color) {
	short loop;
	for (loop = first_color; loop <= last_color; loop++)
		SetRGB4(vp, loop, 0x0, 0x0, 0x0);
}

void set_palette_to_grey(struct ViewPort *vp, UWORD first_color, UWORD last_color) {
	int loop;
	int luma;
	for (loop = first_color; loop <= last_color; loop++)
	{
		luma = range_adjust(loop, first_color, last_color, 0, 15);
		SetRGB4(vp, loop, luma, luma, luma);
	}
}

void set_palette(struct ViewPort *vp, UWORD **palette, UWORD first_color, UWORD last_color) {
	short loop, r, g, b;
	for (loop = first_color; loop <= last_color; loop++) {
		r = ((*palette)[loop] >> 8) & 0xf;
		g = ((*palette)[loop] >> 4) & 0xf;
		b = (*palette)[loop] & 0xf;
		SetRGB4(vp, loop,r, g, b);
	}
}

void fadein_rgb4_palette(struct ViewPort *current_viewport, PALETTEPTR current_palette, UWORD pal_size, unsigned short steps){
	unsigned short j;
	for(j = 0; j < steps; j++) { /* fade in */
		WaitTOF();
		mix_rgb4_palette_to_black(current_viewport, current_palette, pal_size, ((steps - 1 - j) * 255) / steps);	
	}	
}

void fadeout_rgb4_palette(struct ViewPort *current_viewport, PALETTEPTR current_palette, UWORD pal_size, unsigned short steps){
	unsigned short j;
	for(j = 0; j < steps; j++) { /* fade out */
		WaitTOF();
		mix_rgb4_palette_to_black(current_viewport, current_palette, pal_size, 255 - (((steps - 1 - j) * 255) / steps));	
	}	
}

void fade_rgb4_palettes(struct ViewPort *current_viewport, PALETTEPTR source_palette, PALETTEPTR dest_palette, UWORD pal_size, unsigned short steps){
	unsigned short i,j;
	UWORD col;

	for(j = 0; j < steps; j++) { /* fade step by step */
		WaitTOF();
		for (i = 0; i < pal_size; i++) {
			col = mix_rgb4_colors(dest_palette[i], source_palette[i], ((steps - 1 - j) * 255) / steps);
			SetRGB4(current_viewport, i, (col & 0x0f00) >> 8, (col & 0x00f0) >> 4, col & 0x000f);
		}	
	}	
}
#endif
