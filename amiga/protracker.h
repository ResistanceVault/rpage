/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#ifndef AUDIO_ROUTINES
#define AUDIO_ROUTINES

#include "frwk/amiga/ptreplay.h"
#include "frwk/amiga/ptreplay_protos.h"
#include "frwk/amiga/ptreplay_pragmas.h"

BOOL init_protracker_player(void);
void uninit_protracker_player(void);
void load_protacker_music(char *filename, ULONG filesize);
void unload_protacker_music(void);
void play_protracker_music(void);

#endif // #ifndef AUDIO_ROUTINES
#endif