/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#ifndef _INPUT_ROUTINES_
#define _INPUT_ROUTINES_

#include <exec/types.h>

#define PLATFORM_MOUSE_LEFT_BUTTON 1
#define PLATFORM_MOUSE_RIGHT_BUTTON (1 << 1)

UBYTE Keyboard(void);
void input_window_init(struct Window *window);
void input_update(short *button, short *x, short *y, unsigned short *rawkey);
#endif
#endif