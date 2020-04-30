/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#ifndef BITMAP_ROUTINES
#define BITMAP_ROUTINES

/*
		Misc bitmap routines headers
*/

#include "rpage/aos/inc.prl"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos/dos.h>
#include <exec/types.h>             /* The Amiga data types file. */
#include <exec/memory.h>             /* The Amiga data types file. */
#include <exec/libraries.h>
#include <intuition/intuition.h>    /* Intuition data strucutres, etc. */
#include <libraries/dos.h>          /* Official return codes defined here */
#include <devices/keyboard.h>

#include <clib/exec_protos.h>       /* Exec function prototypes           */
#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>       /* Exec function prototypes */
#include <clib/intuition_protos.h>  /* Intuition function prototypes */

#include "rpage/aos/debug.h"

/*
	Image loading
*/
BOOL load_pak_img_to_bitmap(struct BitMap **bitmap, UWORD **palette, BYTE *packed_block, UBYTE *name);
BOOL load_pak_img_to_new_bitmap(struct BitMap **new_bitmap, UWORD **new_palette, BYTE *packed_block, UBYTE *name);

/*
	Image loading
	Legacy RAW format (exported from Cloanto PPaint)
*/
struct BitMap *allocate_new_bitmap(short width, short height, short depth);
void free_allocated_bitmap(struct BitMap *allocated_bitmap);
PLANEPTR load_raw_to_mem(UBYTE *name, ULONG size, BOOL allocate_into_chipmem);
void clear_bitmap(struct BitMap *bitmap);

/*  
	Simple bitblit
*/
#define BLIT_BITMAP_S(SRC_BITMAP, DEST_BITMAP, WIDTH, HEIGHT, X, Y) BltBitMap(SRC_BITMAP, 0, 0, \
						DEST_BITMAP, X, Y,  \
						WIDTH, HEIGHT,      \
						0xC0, 0xFF, NULL);

#endif // #ifndef BITMAP_ROUTINES
#endif