/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 Fran�ois Gutherz, Resistance.no
	Released under MIT License, see license.txt for details.
*/

#ifdef WIN32
/*
		Misc bitmap routines
*/

// #include "external/tinfl.h"
#include "rpage/frwk.h"

BOOL load_pak_img_to_bitmap(struct BitMap **bitmap, UWORD **palette, BYTE *packed_block, UBYTE *name)
{
	/*
	ULONG size;
	BPTR fileHandle;
	char tag[4];
	UWORD w, h, d, pal_size, packed_block_size;
	PLANEPTR plane_prt;

	UWORD i;
	// UBYTE *read_ptr;

#ifdef DEBUG_MACROS
	printf("load_pak_img_to_chipmem(): '%s',", name);
#endif
	if ((fileHandle = Open(name, MODE_OLDFILE)))
	{
		Read(fileHandle, &tag, 4);
		if (strncmp(tag, "IMPK", 4) != 0)
		{
			//printf("cannot found tag 'IMPK'!\n");
		}
		else
		{
			// Read image geometry 
			Read(fileHandle, &w, 2); // image width in pixels
			Read(fileHandle, &h, 2); // image height in pixels
			Read(fileHandle, &d, 2); // image depth
			size = RASSIZE(w, h);	// size of a single bitplane, in bytes
			pal_size = 1 << d;
#ifdef DEBUG_MACROS
			printf("w=%d, h=%d, d=%d, palette_size=%d, plane_size=%d,", w, h, d, pal_size, size);
#endif
			// Read color palette (if available)
			Read(fileHandle, &tag, 4);
			if (strncmp(tag, "PAL4", 4) != 0)
			{
				// printf("cannot found tag 'PAL4'!\n");
			}
			else
			{
				for (i = 0; i < pal_size; i++)
					Read(fileHandle, &((*palette)[i]), 2);
			}

			Read(fileHandle, &tag, 4);
			if (strncmp(tag, "DATA", 4) == 0)
			{
#ifdef DEBUG_MACROS
				printf(", found raw 'DATA'!");
#endif
				Read(fileHandle, (**bitmap).Planes[0], size * d);
#ifdef DEBUG_MACROS
				printf(", loaded plane #%d", i);
#endif
			}
			else if (strncmp(tag, "MINZ", 4) == 0)
			{
#ifdef DEBUG_MACROS
				printf(", found 'MINIZ' data!");
#endif
				Read(fileHandle, &tag, 4);
				if (strncmp(tag, "SIZE", 4) == 0)
				{
					Read(fileHandle, &packed_block_size, 2);
#ifdef DEBUG_MACROS
					printf(", MINIZ block size: %d", packed_block_size);
#endif
					Read(fileHandle, packed_block, packed_block_size);
					tinfl_decompress_mem_to_mem((**bitmap).Planes[0], size * d, packed_block, packed_block_size, 1);
#ifdef DEBUG_MACROS
					printf(", loaded packed plane #%d", i);
#endif
				}
				else
					printf(", no 'SIZE' tag found!");
			}
			else if (strncmp(tag, "SHRK", 4) == 0)
			{
#ifdef DEBUG_MACROS
				printf(", found 'Shrinkler' data!");
#endif
				Read(fileHandle, &tag, 4);
				if (strncmp(tag, "SIZE", 4) == 0)
				{
					Read(fileHandle, &packed_block_size, 2);
#ifdef DEBUG_MACROS
					printf(", Shrinkler block size: %d", packed_block_size);
#endif
					Read(fileHandle, packed_block, packed_block_size);
					ShrinklerDecompress(packed_block, (**bitmap).Planes[0], NULL, NULL);
#ifdef DEBUG_MACROS
					printf(", loaded packed plane #%d", i);
#endif
				}
				else
					printf(", no 'SIZE' tag found!");
			}
#ifdef DEBUG_MACROS
			printf("\n");
#endif
			// return FALSE;
		}
	}
	else
	{
		char _err_msg[128];
		memset(_err_msg, 0, 128);
		strcpy(_err_msg, "Cannot load '");
		strcat(_err_msg, name); // fixme!
		strcat(_err_msg, "'!");
		rpage_system_alert(_err_msg);
		printf("cannot open file '%s'!\n", name);
		return FALSE;
	}
	*/
	return TRUE;
}

#endif