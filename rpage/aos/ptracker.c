/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#include "rpage/aos/inc.prl"
#include <time.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>
#include <hardware/custom.h>
#include <graphics/gfxmacros.h>

#include "rpage/aos/sound.h"
#include "rpage/aos/ptreplay.h"
#include "rpage/aos/ptreplay_protos.h"
#include "rpage/aos/ptreplay_pragmas.h"

#include "rpage/aos/bitmap.h"
#include "rpage/aos/io.h"

#include "ext/tinfl.h"
#include "rpage/aos/shrinkler.h"
#include "rpage/aos/doynax.h"

#include "rpage/frwk.h"
#include "rpage/aos/ptracker.h"
#include "rpage/err.h"

/* Music */
struct Library *PTReplayBase = NULL;
struct Module *protracker_mod_playing = NULL;
UBYTE *protracker_mod_data = NULL;
ULONG protracker_mod_size = 0;
BYTE protracker_SigBit;
ULONG protracker_SigMask;

short protracker_mod_volume = 0;
short protracker_fade_min_volume = 0;
short protracker_fade_max_volume = 0;
short protracker_fade_speed = 0;
BOOL protracker_auto_stop = TRUE;

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

	protracker_fade_speed = 0;
	protracker_fade_min_volume = 0;
	protracker_fade_max_volume = 64;
	protracker_mod_volume = protracker_fade_max_volume;

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

void load_protacker_music(char *filename, int filesize)
{
	if (filesize < 0)
	{
		filesize = file_get_size(filename);
		printf("load_protacker_music(), guessed module file size : %d\n", filesize);
	}

	protracker_mod_data = load_raw_to_mem((UBYTE *)filename, (ULONG)filesize, TRUE);
	protracker_mod_size = filesize;
	printf("load_protacker_music(%s) loaded at %x.\n", filename, (unsigned int)protracker_mod_data);
}

void save_protracker_music(char *filename)
{
	rpage_file file;
	if (protracker_mod_data != NULL)
	{
		file = rpage_file_open(filename, MODE_OPEN_OR_CREATE);
		rpage_file_write(file, protracker_mod_data, protracker_mod_size);
		rpage_file_close(file);
	}
}

void load_imploded_protracker_music(char *filename)
{
	int unpacked_block_size, packed_block_size, i;
	BYTE *packed_block, *unpacked_block, *smpl_list_ptr, *smpl_ptr_save;
	BPTR fileHandle;
	char tag[4];
	char *spl_files[32];
	UWORD mod_size;	

	if ((fileHandle = Open(filename, MODE_OLDFILE)))
	{
		Read(fileHandle, &tag, 4);
		if (strncmp(tag, "IMPK", 4) == 0)
		{
			Read(fileHandle, &tag, 4);
			if (strncmp(tag, "SIZE", 4) == 0)
			{
				Read(fileHandle, &unpacked_block_size, 4);
				unpacked_block = AllocMem(unpacked_block_size, MEMF_CHIP);

				Read(fileHandle, &tag, 4);
				if (strncmp(tag, "MINZ", 4) == 0)
				{
					Read(fileHandle, &tag, 4);
					if (strncmp(tag, "SIZE", 4) == 0)
					{
						Read(fileHandle, &packed_block_size, 4);
						packed_block = AllocMem(packed_block_size, 0L);
						Read(fileHandle, packed_block, packed_block_size);
#ifdef DEBUG_MACROS            
						printf("!!!!MINIZ block size: %d\n", packed_block_size);
#endif
						tinfl_decompress_mem_to_mem(unpacked_block, unpacked_block_size, packed_block, packed_block_size, 1);
						FreeMem(packed_block, packed_block_size);

						// look for the "SMPL" tag
						smpl_list_ptr = unpacked_block;
						while((int)(smpl_list_ptr - unpacked_block) < unpacked_block_size && strncmp(smpl_list_ptr, "SMPL", 4))
							smpl_list_ptr++;

						smpl_ptr_save = smpl_list_ptr;
							
						// get the sample filenames
						for(i = 0; i < 32; i++)
						{
							smpl_list_ptr += 4;
							if (smpl_list_ptr[0] | smpl_list_ptr[1] | smpl_list_ptr[2] | smpl_list_ptr[3])
							{
								spl_files[i] = (char *)calloc(5, sizeof(char));
								strncpy(spl_files[i], smpl_list_ptr, 4);
								printf("Sample #%d: '%s.pak'.\n", i, spl_files[i]);
							}
							else
							{
								spl_files[i] = NULL;
							}
						}

						// back to the memory adress
						// where the samples should be located 
						smpl_list_ptr  = smpl_ptr_save;

						for(i = 0; i < 32; i++)
						{
							SoundInfo *sfx;

							// load each sample
							if (spl_files[i])
							{
								char *_file;
								_file = asset_build_device_path(spl_files[i], ".pak");
								sfx = LoadPackedSound(_file, game_get_unpacking_buffer(), smpl_list_ptr);
								smpl_list_ptr += sfx->FileLength;
								printf("Sample #%d: '%s.pak, %X bytes'.\n", i, spl_files[i], sfx->FileLength);
								RemoveSoundStruct(sfx);
							}
						}

						protracker_mod_data = unpacked_block;
						protracker_mod_size = unpacked_block_size;
					}
					else
					{
						printf(err_no_size_found);
					}
				}
			}
		}
	}						
}

void load_packed_protacker_music(char *filename)
{
	int unpacked_block_size, packed_block_size;
	BPTR fileHandle;
	char tag[4];
	UWORD mod_size;
	BYTE *packed_block, *unpacked_block;

	if ((fileHandle = Open(filename, MODE_OLDFILE)))
	{
		Read(fileHandle, &tag, 4);
		if (strncmp(tag, "PTPK", 4) == 0)
		{
			// read original module size
			Read(fileHandle, &unpacked_block_size, 4);
			unpacked_block = AllocMem(unpacked_block_size, MEMF_CHIP);
	
			Read(fileHandle, &tag, 4);
			if (strncmp(tag, "MINZ", 4) == 0)
			{
				Read(fileHandle, &tag, 4);
				if (strncmp(tag, "SIZE", 4) == 0)
				{
					Read(fileHandle, &packed_block_size, 4);
					packed_block = AllocMem(packed_block_size, 0L);
					Read(fileHandle, packed_block, packed_block_size);
#ifdef DEBUG_MACROS            
					printf("!!!!MINIZ block size: %d\n", packed_block_size);
#endif
					tinfl_decompress_mem_to_mem(unpacked_block, unpacked_block_size, packed_block, packed_block_size, 1);
					FreeMem(packed_block, packed_block_size);
				}
				else
					printf(err_no_size_found);
			}
			else if (strncmp(tag, "SHRK", 4) == 0)
			{
				Read(fileHandle, &tag, 4);
				if (strncmp(tag, "SIZE", 4) == 0)
				{
					Read(fileHandle, &packed_block_size, 4);
					packed_block = AllocMem(packed_block_size, 0L);
					Read(fileHandle, packed_block, packed_block_size);
#ifdef DEBUG_MACROS            
					printf("!!!!SHRK block size: %d\n", packed_block_size);
#endif
					ShrinklerDecompress(packed_block, unpacked_block, NULL, NULL);
					FreeMem(packed_block, packed_block_size);
				}
				else
					printf(err_no_size_found);
			}
			else if (strncmp(tag, "D68K", 4) == 0)
			{
				Read(fileHandle, &tag, 4);
				if (strncmp(tag, "SIZE", 4) == 0)
				{
					Read(fileHandle, &packed_block_size, 4);
					packed_block = AllocMem(DOYNAX_ALLOC_PAD(packed_block_size), 0L);
					Read(fileHandle, packed_block, packed_block_size);
#ifdef DEBUG_MACROS            
					printf("!!!!D68K block size: %d\n", packed_block_size);
#endif
					doynaxdepack(packed_block, unpacked_block);
					FreeMem(packed_block, packed_block_size);
				}
				else
					printf(err_no_size_found);
			}
			else
			{
				printf(err_unknown_tag, tag);
				FreeMem(unpacked_block, unpacked_block_size);
				unpacked_block = NULL;
				unpacked_block_size = 0;
			}
			
		}
		else
		{
			printf("!Not a Protracker Packed File!\n");
		}
		
	}

	protracker_mod_data = unpacked_block;
	protracker_mod_size = unpacked_block_size;
}

void unload_protacker_music(void)
{
	if (protracker_mod_data != NULL && protracker_mod_playing != NULL)
	{
		//	Stop music
		PTStop(protracker_mod_playing);

		// Free signals
		FreeSignal(protracker_SigBit);

		//	Free allocated memory
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
			if((protracker_SigBit=AllocSignal(-1)) != -1)
			{
				protracker_mod_playing = PTSetupMod((APTR)protracker_mod_data);
				PTInstallBits(protracker_mod_playing, protracker_SigBit, -1, -1, -1);
				PTSetVolume(protracker_mod_playing, protracker_mod_volume);
				PTPlay(protracker_mod_playing);
			}
			else
				printf("play_protracker_music() Couldn't allocate signal!\n");
			
		}
	else
		rpage_system_alert("Ptreplay.library was not open!");
}

void protracker_update_state(void)
{
	if (PTReplayBase)
		if (protracker_mod_data != NULL)
		{
			if (protracker_fade_speed != 0)
			{
				protracker_mod_volume = min(protracker_fade_max_volume, max(protracker_fade_min_volume, protracker_mod_volume + protracker_fade_speed));
				PTSetVolume(protracker_mod_playing, protracker_mod_volume);
				if (protracker_auto_stop && protracker_mod_volume <= 0)
					unload_protacker_music();
			}
		}
}

void protracker_set_fade_speed(short fade_speed)
{
	protracker_fade_speed = fade_speed;
}

#endif