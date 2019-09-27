/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#include "rpage/amiga/includes.prl"
#include <time.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>
#include <hardware/custom.h>
#include <graphics/gfxmacros.h>

#include "rpage/amiga/ptreplay.h"
#include "rpage/amiga/ptreplay_protos.h"
#include "rpage/amiga/ptreplay_pragmas.h"

#include "rpage/amiga/bitmap.h"

#include "rpage/frwk.h"
#include "rpage/amiga/ptracker.h"

/* Music */
struct Library *PTReplayBase = NULL;
struct Module *protracker_mod_playing = NULL;
UBYTE *protracker_mod_data = NULL;
ULONG protracker_mod_size = 0;

// extern struct IntuitionBase *IntuitionBase;
// extern struct GfxBase *GfxBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct Custom far custom;

BOOL init_protracker_player(void)
{
	if (SysBase->LibNode.lib_Version >= 36)
		if (!AssignPath("Libs","Libs"))
			printf("/!\\Cannot assign local Libs: folder. The Ptreplay library might not load properly!\n");

	if (!(PTReplayBase = OpenLibrary((UBYTE *)"ptreplay.library", 0)))
	{
		rpage_system_alert("Cannot open ptreplay.library!");
		PTReplayBase = NULL;
		return FALSE;
	}

	return TRUE;
}

void uninit_protracker_player(void)
{
	unload_protacker_music();

	if (PTReplayBase)
	{
		CloseLibrary(PTReplayBase);
		PTReplayBase = NULL;
	}
}

void load_protacker_music(char *filename, ULONG filesize)
{
	protracker_mod_data = load_raw_to_mem((UBYTE *)filename, filesize, TRUE);
	protracker_mod_size = filesize;
	printf("load_protacker_music(%s) loaded at %x.\n", filename, (unsigned int)protracker_mod_data);
}

void unload_protacker_music(void)
{
	if (protracker_mod_data != NULL && protracker_mod_playing != NULL)
	{
		/*	Stop music */
		PTStop(protracker_mod_playing);

		/*	Free allocated memory */
		PTFreeMod(protracker_mod_playing);
		FreeMem(protracker_mod_data, protracker_mod_size);

		protracker_mod_playing = NULL;
		protracker_mod_data = NULL;
	}
}

void play_protracker_music(void)
{
	if (PTReplayBase)
		if (protracker_mod_data != NULL && protracker_mod_playing == NULL)
		{
			protracker_mod_playing = PTSetupMod((APTR)protracker_mod_data);
			PTPlay(protracker_mod_playing);
		}
	else
		rpage_system_alert("Ptreplay.library was not open!");
}

#endif