/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#include "rpage/aos/inc.prl"
#include "rpage/aos/color.h"
#include "rpage/utils.h"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>

#include "rpage/aos/color.h"
#include "rpage/utils.h"

extern struct GfxBase *GfxBase;

UWORD __inline components_to_rgb8(UWORD r, UWORD g, UWORD b)
{
	if (r > 0xff)
		r = 0xff;
	if (g > 0xff)
		g = 0xff;
	if (b > 0xff)
		b = 0xff;

	r = r & 0xff;
	g = g & 0xff;
	b = b & 0xff;

	return (UWORD)((r << 16) | (g << 8) | b);
}


UWORD __inline components_to_rgb4(UWORD r, UWORD g, UWORD b)
{
	r >>= 4;
	g >>= 4;
	b >>= 4;

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

UWORD color_to_depth(UWORD colors)
{
	UWORD i = 1;
	while ((1 << i) < colors)
		i++;

	return i;
}

UWORD darken_rgb4_colors(UWORD A, USHORT n)
{
	WORD r, g, b, x, y, z;

	if (n == 0)
		return A;

	if (n >= 255)
		return 0x000;

	// n = 255 - n;
	n = n >> 4;

	r = (A & 0x0f00) >> 8;
	g = (A & 0x00f0) >> 4;
	b = A & 0x000f;

	r = max(0, r - n);
	g = max(0, g - n);
	b = max(0, b - n);

	return (UWORD)((r << 8) | (g << 4) | b);
}

UWORD mix_rgb4_colors(UWORD A, UWORD B, USHORT n)
{
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

	r >>= 8;
	g >>= 8;
	b >>= 8;

	if (r > 0xf)
		r = 0xf;
	if (g > 0xf)
		g = 0xf;
	if (b > 0xf)
		b = 0xf;

	// r = r & 0xf;
	// g = g & 0xf;
	// b = b & 0xf;

	return (UWORD)((r << 8) | (g << 4) | b);
}

ULONG rgb4_to_rgb8(UWORD A)
{
	ULONG r, g, b;
	r = ((ULONG)(A & 0x0f00)) << 12;
	g = (A & 0x00f0) << 8;
	b = (A & 0x000f) << 4;

	return (ULONG)(r | g | b);
}

UWORD rgb8_to_rgb4(ULONG A)
{
	UWORD r, g, b;
	r = (A & 0xF00000) >> 12; // ((ULONG)(A & 0x0f00)) << 12;
	g = (A & 0x00F000) >> 8;
	b = (A & 0x00F0) >> 4;

	return (UWORD)(r | g | b);
}

ULONG add_rgb8_colors(ULONG A, ULONG B)
{
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

ULONG divide_rgb8_colors(ULONG A, UWORD n)
{
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

ULONG mix_rgb8_colors(ULONG A, ULONG B, UWORD n)
{
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

// void mix_rgb4_palettes(struct ViewPort *vp, amiga_color *source_palette, amiga_color *dest_palette, UWORD pal_size,
// 					   UWORD fade)
// {
// 	UBYTE i;

// 	for (i = 0; i < pal_size; i++)
// 	{
// 		UWORD col = mix_rgb4_colors(source_palette[i], dest_palette[i], fade);
// 		SetRGB4(vp, i, (col & 0x0f00) >> 8, (col & 0x00f0) >> 4, col & 0x000f);
// 	}
// }

void mix_rgb4_palette_to_black(struct ViewPort *vp, color444 *pal, UWORD pal_size,
							   UWORD fade)
{
	UBYTE i;

	for (i = 0; i < pal_size; i++)
	{
		UWORD col = mix_rgb4_colors(pal[i], 0x000, fade);
		SetRGB4(vp, i, (col & 0x0f00) >> 8, (col & 0x00f0) >> 4, col & 0x000f);
	}
}

// void mix_rgb4_palette_to_black_as_rgb8(struct ViewPort *vp, UWORD *pal, UWORD pal_size,
// 									   ULONG rgb8color, UWORD fade)
// {
// 	UBYTE i;

// 	for (i = 0; i < pal_size; i++)
// 	{
// 		UWORD col = mix_rgb8_colors(rgb4_to_rgb8(pal[i]), rgb8color, fade);
// 		col = rgb8_to_rgb4(col);
// 		SetRGB4(vp, i, (col & 0x0f00) >> 8, (col & 0x00f0) >> 4, col & 0x000f);
// 	}
// }

void set_palette_to_black(struct ViewPort *vp, UWORD first_color, UWORD last_color)
{
	short loop;
	for (loop = first_color; loop <= last_color; loop++)
		SetRGB4(vp, loop, 0x0, 0x0, 0x0);
}

void set_palette_to_grey(struct ViewPort *vp, UWORD first_color, UWORD last_color)
{
	int loop;
	for (loop = first_color; loop <= last_color; loop++)
	{
		unsigned int luma;
#ifdef VGA_CAPABLE
		luma = range_adjust(loop, first_color, last_color, 0, 255);
		SetRGB32(vp, loop, luma << 24, luma << 24, luma << 24);
#else
		luma = range_adjust(loop, first_color, last_color, 0, 15);
		SetRGB4(vp, loop, luma, luma, luma);
#endif
	}
}

void set_palette(struct ViewPort *vp, UWORD **palette, UWORD first_color, UWORD last_color)
{
	if (first_color > 0)
	{
		// Slow mode
		short loop;
		for (loop = first_color; loop <= last_color; loop++)
		{
	#ifdef VGA_CAPABLE
			unsigned int r, g, b;
			r = ((*palette)[loop] >> 16) & 0xff;
			g = ((*palette)[loop] >> 8) & 0xff;
			b = (*palette)[loop] & 0xff;
			SetRGB32(vp, loop, r << 24, g << 24, b << 24);
	#else
			UWORD r, g, b;
			r = ((*palette)[loop] >> 8) & 0xf;
			g = ((*palette)[loop] >> 4) & 0xf;
			b = (*palette)[loop] & 0xf;
			SetRGB4(vp, loop, r, g, b);
	#endif
		}
	}
	else
	{
		// Fast mode
	#ifdef VGA_CAPABLE
		LoadRGB32(vp, (ULONG *)*palette, last_color);
	#else
		LoadRGB4(vp, (UWORD *)*palette, last_color);
	#endif
	}
}

// void fadein_rgb4_palette(struct ViewPort *current_viewport, color444 *current_palette, UWORD pal_size, unsigned short steps)
// {
// 	unsigned short j;
// 	for (j = 0; j < steps; j++)
// 	{ /* fade in */
// 		WaitTOF();
// 		mix_rgb4_palette_to_black(current_viewport, current_palette, pal_size, ((steps - 1 - j) * 255) / steps);
// 	}
// }

// void fadeout_rgb4_palette(struct ViewPort *current_viewport, color444 *current_palette, UWORD pal_size, unsigned short steps)
// {
// 	unsigned short j;
// 	for (j = 0; j < steps; j++)
// 	{ /* fade out */
// 		WaitTOF();
// 		mix_rgb4_palette_to_black(current_viewport, current_palette, pal_size, 255 - (((steps - 1 - j) * 255) / steps));
// 	}
// }

// void fade_rgb4_palettes(struct ViewPort *current_viewport, color444 *source_palette, color444 *dest_palette, UWORD pal_size, unsigned short steps)
// {
// 	unsigned short i, j;
// 	UWORD col;

// 	for (j = 0; j < steps; j++)
// 	{ /* fade step by step */
// 		WaitTOF();
// 		for (i = 0; i < pal_size; i++)
// 		{
// 			col = mix_rgb4_colors(dest_palette[i], source_palette[i], ((steps - 1 - j) * 255) / steps);
// 			SetRGB4(current_viewport, i, (col & 0x0f00) >> 8, (col & 0x00f0) >> 4, col & 0x000f);
// 		}
// 	}
// }
#endif
