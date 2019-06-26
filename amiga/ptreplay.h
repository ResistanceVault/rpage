/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
/* ptreplay.h -- definition of ptreplay.library structures */

#ifndef	PTREPLAY_BASE_H 
#define PTREPLAY_BASE_H

#ifndef	EXEC_TYPES_H
#include <exec/types.h>
#endif

#define PTREPLAYNAME "ptreplay.library"

struct Module
{
    STRPTR mod_Name;
/* The rest is private for now, but more details will be released later. */
};

#endif
#endif