/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifndef _UTILS_HEADER_
#define _UTILS_HEADER_
#include <string.h>
#include <stdlib.h>
#ifdef LATTICE
#include <exec/types.h>
#endif
#include "rpage/coroutine.h"

#ifndef LATTICE
#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef UBYTE
typedef unsigned char UBYTE;
#endif

#ifndef ULONG
typedef unsigned long ULONG;
#endif

#ifndef UWORD
typedef unsigned short UWORD;
#endif

#ifndef BOOL
typedef int BOOL;
#endif
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif

typedef struct
{
    short x, y;
} vec2;

typedef struct
{
    int x, y;
} vec2fp;

typedef struct
{
    vec2 p0, p1, p2, p3;
} poly;

typedef struct
{
    short sx, sy, ex, ey;
} rect;

int range_adjust(int val, int in_lower, int in_upper, int out_lower, int out_upper);
int clamp(int x, int in_lower, int in_upper);
BOOL point_within_rect(vec2 *pt, rect *r);
BOOL point_within_quad(vec2 *pt, poly *pl);
BOOL point_within_polygon(vec2 *pt, vec2 *pt_list, unsigned short n_pt);
char* citoa(int num, char* str, int base);
short str_find_delimiter(short start, char *str);
int qsqr(int i);
#endif
