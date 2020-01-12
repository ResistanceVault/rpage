/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#ifndef SCREEN_SIZE
#define SCREEN_SIZE

// #define VGA_ENABLED
#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 200
#define DISPL_WIDTH (DEFAULT_WIDTH)
#define DISPL_HEIGHT (DEFAULT_HEIGHT)
#define WIDTH (DEFAULT_WIDTH)
#define HEIGHT (DEFAULT_HEIGHT)
#ifdef VGA_ENABLED
#define DEPTH    7
#else
#define DEPTH    5
#endif
#define COLOURS  (1 << DEPTH)

#endif // #ifndef SCREEN_SIZE
#endif