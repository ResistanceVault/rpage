/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#ifndef SCREEN_ROUTINES
#define SCREEN_ROUTINES

#include "rpage/amiga/includes.prl"
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>

typedef struct{
	USHORT width;
	USHORT height;
	UBYTE depth;
	unsigned int view_modes;
} screen_geometry;

typedef struct{
	struct BitMap *bitmaps[2];
	struct Screen *screen;
	struct Window *window;
	BOOL double_buffer_enabled;
	USHORT physical;
} buffered_screen;

#define DBUFFER_DISABLED FALSE
#define DBUFFER_ENABLED TRUE

/* Prototypes for our functions */

void WaitVBL(buffered_screen *screen);
void disableScreen(void);
void enableScreen(void);
buffered_screen *openMainScreenCustom(USHORT _width, USHORT _height, USHORT _colors, BOOL _dbuffer);
buffered_screen *openMainScreen(void);
void closeMainScreen(buffered_screen *main_screen);
struct BitMap *setupBitMap(LONG, LONG, LONG);
void freeBitMap(struct BitMap *,LONG, LONG, LONG);
LONG setupPlanes(struct BitMap *, LONG, LONG, LONG);
void freePlanes(struct BitMap *, LONG, LONG, LONG);
USHORT getLogicalBitmapIndex(buffered_screen *screen);
USHORT getPhysicalBitmapIndex(buffered_screen *screen);
void flipBuffers(buffered_screen *screen);
void presentScreen(buffered_screen *screen);
void synchronizeBuffers(buffered_screen *screen);

#endif
#endif