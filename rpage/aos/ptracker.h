/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#ifndef AUDIO_ROUTINES
#define AUDIO_ROUTINES

#include "rpage/aos/ptreplay.h"
#include "rpage/aos/ptreplay_protos.h"
#include "rpage/aos/ptreplay_pragmas.h"

BOOL init_protracker_player(void);
void uninit_protracker_player(void);
void load_imploded_protracker_music(char *filename, UBYTE *unpacking_sample_buffer, char *asset_path);
void load_packed_protacker_music(char *filename);
void save_protracker_music(char *filename);
void load_protacker_music(char *filename, int filesize);
void save_protracker_music(char *filename);
void unload_protacker_music(void);
void play_protracker_music(void);
void protracker_set_fade_speed(short fade_speed);
void protracker_update_state(void);


#endif // #ifndef AUDIO_ROUTINES
#endif