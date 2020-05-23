/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef WIN32
#ifndef SCREEN_SIZE
#define SCREEN_SIZE

#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 200
#define DISPL_WIDTH (DEFAULT_WIDTH * 2)
#define DISPL_HEIGHT (DEFAULT_HEIGHT * 2)
#define SCREEN_WIDTH (DEFAULT_WIDTH)
#define SCREEN_HEIGHT (DEFAULT_HEIGHT)
#define SCREEN_DEPTH    8
#define COLOURS  (1 << SCREEN_DEPTH)

#endif // #ifndef SCREEN_SIZE
#endif