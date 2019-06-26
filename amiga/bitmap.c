/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
/*
		Misc bitmap routines
*/

#include "rpage/amiga/includes.prl"
#include "rpage/amiga/debug.h"
#include "external/tinfl.h"
#include "rpage/amiga/shrinkler.h"
#include "rpage/frwk.h"

extern struct DosLibrary *DOSBase;
extern struct GfxBase *GfxBase;

struct BitMap *allocate_new_bitmap(short width, short height, short depth)
{
	short i, size;
	struct BitMap *new_bitmap;
	PLANEPTR plane_prt;

	size = RASSIZE(width, height);
	new_bitmap = (struct BitMap *)AllocMem((LONG)sizeof(struct BitMap), MEMF_CLEAR);
	InitBitMap(new_bitmap, depth, width, height);

	plane_prt = (PLANEPTR)AllocMem(size * depth, MEMF_CHIP | MEMF_CLEAR);
	for (i = 0; i < depth; i++)
	{
		new_bitmap->Planes[i] = plane_prt + (i * size);
#ifdef DEBUG_MACROS
		printf(", allocated plane #%d", i);
#endif
	}

	return new_bitmap;
}

BOOL load_pak_img_to_bitmap(struct BitMap **bitmap, UWORD **palette, BYTE *packed_block, UBYTE *name)
{
	ULONG size;
	BPTR fileHandle;
	char tag[4];
	UWORD w, h, d, pal_size, packed_block_size;
	PLANEPTR plane_prt;

	UWORD i;
	// UBYTE *read_ptr;

#ifdef DEBUG_MACROS
	printf("load_pak_img_to_bitmap(): '%s',", name);
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
			/* Read image geometry */
			Read(fileHandle, &w, 2); /* image width in pixels */
			Read(fileHandle, &h, 2); /* image height in pixels */
			Read(fileHandle, &d, 2); /* image depth */
			size = RASSIZE(w, h);	/* size of a single bitplane, in bytes */
			pal_size = 1 << d;
#ifdef DEBUG_MACROS
			printf("w=%d, h=%d, d=%d, palette_size=%d, plane_size=%d,", w, h, d, pal_size, size);
#endif
			/* Read color palette (if available) */
			Read(fileHandle, &tag, 4);
			if (strncmp(tag, "PAL4", 4) != 0)
			{
				// printf("cannot found tag 'PAL4'!\n");
			}
			else
			{
				if (palette != NULL)
				{
					for (i = 0; i < pal_size; i++)
						Read(fileHandle, &((*palette)[i]), 2);
				}
				else
					Seek(fileHandle, pal_size * 2, OFFSET_CURRENT);
				
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
		// char _err_msg[128];
		// memset(_err_msg, 0, 128);
		// strcpy(_err_msg, "Cannot load '");
		// strcat(_err_msg, name); // fixme!
		// strcat(_err_msg, "'!");
		// rpage_system_alert(_err_msg);
		printf("load_pak_img_to_bitmap() : cannot open file '%s'!\n", name);
		return FALSE;
	}

	return TRUE;
}

BOOL load_pak_img_to_new_bitmap(struct BitMap **new_bitmap, UWORD **new_palette, BYTE *packed_block, UBYTE *name)
{
	ULONG size;
	BPTR fileHandle;
	char tag[4];
	UWORD w, h, d, pal_size, packed_block_size;
	BOOL self_alloc_unpack_buffer = FALSE;
	// BYTE *packed_block;
	PLANEPTR plane_prt;

	UWORD i;
	// UBYTE *read_ptr;

	if (packed_block == NULL)
		self_alloc_unpack_buffer = TRUE;

#ifdef DEBUG_MACROS
	printf("load_pak_img_to_new_bitmap(): '%s',", name);
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
			/* Read image geometry */
			Read(fileHandle, &w, 2); /* image width in pixels */
			Read(fileHandle, &h, 2); /* image height in pixels */
			Read(fileHandle, &d, 2); /* image depth */
			size = RASSIZE(w, h);	/* size of a single bitplane, in bytes */
			pal_size = 1 << d;
#ifdef DEBUG_MACROS
			printf("w=%d, h=%d, d=%d, palette_size=%d, plane_size=%d,", w, h, d, pal_size, size);
#endif
			/* Read color palette (if available) */
			Read(fileHandle, &tag, 4);
			if (strncmp(tag, "PAL4", 4) != 0)
			{
				// printf("cannot found tag 'PAL4'!\n");
			}
			else
			{
				if (new_palette != NULL)
				{
					*new_palette = (UWORD *)AllocMem(pal_size * sizeof(UWORD), 0L);
					for (i = 0; i < pal_size; i++)
						Read(fileHandle, &((*new_palette)[i]), 2);
				}
				else
					Seek(fileHandle, pal_size * 2, OFFSET_CURRENT);
			}

			/* Allocate each plane */
			*new_bitmap = allocate_new_bitmap(w, h, d);

			Read(fileHandle, &tag, 4);
			if (strncmp(tag, "DATA", 4) == 0)
			{
#ifdef DEBUG_MACROS
				printf(", found raw 'DATA'!");
#endif
				Read(fileHandle, (**new_bitmap).Planes[0], size * d);
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
					if (self_alloc_unpack_buffer)
						packed_block = AllocMem(packed_block_size * sizeof(BYTE), MEMF_CLEAR);
#ifdef DEBUG_MACROS
					printf(", MINIZ block size: %d", packed_block_size);
#endif
					Read(fileHandle, packed_block, packed_block_size);
					tinfl_decompress_mem_to_mem((**new_bitmap).Planes[0], size * d, packed_block, packed_block_size, 1);
					if (self_alloc_unpack_buffer)
						FreeMem(packed_block, packed_block_size);
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
					if (self_alloc_unpack_buffer)
						packed_block = AllocMem(packed_block_size * sizeof(BYTE), MEMF_CLEAR);
#ifdef DEBUG_MACROS
					printf(", Shrinkler block size: %d", packed_block_size);
#endif
					Read(fileHandle, packed_block, packed_block_size);
					ShrinklerDecompress(packed_block, (**new_bitmap).Planes[0], NULL, NULL);
					if (self_alloc_unpack_buffer)
						FreeMem(packed_block, packed_block_size);
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
			return TRUE;
		}
	}
	else
	{
		printf("load_pak_img_to_new_bitmap() : cannot open file '%s'!\n", name);
	}

	return FALSE;
}

void clear_bitmap(struct BitMap *bitmap)
{
	int i;
	for (i = 0; i < bitmap->Depth; i++)
	{
		BltClear(bitmap->Planes[i], RASSIZE(bitmap->BytesPerRow << 3, bitmap->Rows), 0);
		WaitBlit();
	}
}

PLANEPTR load_raw_to_mem(UBYTE *name, ULONG size, BOOL allocate_into_chipmem)
{
	BPTR fileHandle;
	PLANEPTR mem;

	if (!(fileHandle = Open(name, MODE_OLDFILE)))
	{
		printf("Cannot open file %s\n", name);
		rpage_system_alert("load_raw_to_mem() : cannot open file!");
		return (NULL);
	}	

	if (!(mem = AllocMem(size, allocate_into_chipmem?MEMF_CHIP:0L)))
	{
		rpage_system_alert("load_raw_to_mem() : cannot AllocMem()!");
		Close(fileHandle);
		return (NULL);
	}	

	Read(fileHandle, mem, size);
	Close(fileHandle);

	return (mem);
}

void free_allocated_bitmap(struct BitMap *allocated_bitmap)
{
	UWORD i;
	ULONG block_len;

	if (allocated_bitmap)
	{
#ifdef DEBUG_MACROS		
		printf("free_allocated_bitmap() allocated_bitmap = %x\n", allocated_bitmap);
		printf("allocated_bitmap, BytesPerRow = %d, Rows = %d, Depth = %d, pad = %d\n",
		      (*allocated_bitmap).BytesPerRow,
		      (*allocated_bitmap).Rows,
		      (*allocated_bitmap).Depth,
		      (int)(*allocated_bitmap).pad);
#endif
		block_len =
			RASSIZE((*allocated_bitmap).BytesPerRow * 8, (*allocated_bitmap).Rows);
		for (i = 0; i < (*allocated_bitmap).Depth; i++)
		{
#ifdef DEBUG_MACROS				
			printf("FreeMem() plane[%i], block_len = %i\n", i, block_len);
#endif
			FreeMem((*allocated_bitmap).Planes[i],
						   block_len);
		}

		block_len = (LONG)sizeof(struct BitMap);
		FreeMem(allocated_bitmap, block_len);
	}
}

#endif