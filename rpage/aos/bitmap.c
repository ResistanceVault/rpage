/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
/*
		Misc bitmap routines
*/

#include "rpage/aos/inc.prl"
#include "rpage/aos/debug.h"
#include "rpage/aos/color.h"
#include "ext/tinfl.h"
#include "ext/aos/shrinkler.h"
#include "ext/aos/nrv2.h"
#include "rpage/frwk.h"
#include "rpage/err.h"

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

#ifdef DEBUG_MACROS
	printf("allocate_new_bitmap(), size = %d", size * depth);
#endif
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

BOOL load_pak_img_to_bitmap(struct BitMap **bitmap, amiga_color **palette, BYTE *packed_block, UBYTE *name)
{
	BPTR fileHandle;
	char tag[4];
	UWORD w, h, d, pal_size, packed_block_size;
	// PLANEPTR plane_prt;

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
			printf(err_no_impk_found);
		}
		else
		{
			/* Read image geometry */
			ULONG size;
			Read(fileHandle, &w, 2); /* image width in pixels */
			Read(fileHandle, &h, 2); /* image height in pixels */
			Read(fileHandle, &d, 2); /* image depth */
			size = RASSIZE(w, h);	/* size of a single bitplane, in bytes */
			pal_size = 1 << d;
#ifdef DEBUG_MACROS
			printf("w=%d, h=%d, d=%d, palette_size=%d, plane_size=%d,", w, h, d, pal_size, (int)size);
#endif
			/* Read color palette (if available) */
			Read(fileHandle, &tag, 4);
			if (strncmp(tag, "PAL4", 4) == 0)
			{
#ifdef DEBUG_MACROS
				printf("PAL4\n");
#endif
				if (palette != NULL)
				{
					color444 fcolor; // Color from file
					for (i = 0; i < pal_size; i++)
					{
						Read(fileHandle, &fcolor, 2);
#ifdef DEBUG_MACROS
						printf("%X,", fcolor);
#endif
					#ifdef VGA_CAPABLE
						(*palette)[i] = rgb4_to_rgb8(fcolor); // from RGB444 to RGB888
					#else
						(*palette)[i] = fcolor; // from RGB444 to RGB444
					#endif
					}
					// printf("\n");
				}
				else
					Seek(fileHandle, pal_size * 2, OFFSET_CURRENT);
				
			}
			else
			{
				if (strncmp(tag, "PAL8", 4) == 0)
				{
#ifdef DEBUG_MACROS
					printf("PAL8\n");
#endif
					if (palette != NULL)
					{
						color888 fcolor; // Color from file
						for (i = 0; i < pal_size; i++)
						{
							Read(fileHandle, &fcolor, 4);
#ifdef DEBUG_MACROS
							printf("%X,", fcolor);
#endif
						#ifdef VGA_CAPABLE
							(*palette)[i] = fcolor; // from RGB888 to RGB888
						#else
							(*palette)[i] = rgb8_to_rgb4(fcolor); // from RGB888to RGB444
						#endif
						}
						// printf("\n");
					}
					else
						Seek(fileHandle, pal_size * 4, OFFSET_CURRENT);
					
				}				
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
					// printf("!!!!MINIZ block size: %d\n", packed_block_size);
					tinfl_decompress_mem_to_mem((**bitmap).Planes[0], size * d, packed_block, packed_block_size, 1);
#ifdef DEBUG_MACROS
					printf(", loaded packed plane #%d", i);
#endif
				}
				else
					printf(err_no_size_found);
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
					printf(err_no_size_found);
			}
			else if (strncmp(tag, "NRV2", 4) == 0)
			{
#ifdef DEBUG_MACROS
				printf(", found 'nrv2x' data!");
#endif
				Read(fileHandle, &tag, 4);
				if (strncmp(tag, "SIZE", 4) == 0)
				{
					UBYTE *Destination;
					Read(fileHandle, &packed_block_size, 2);
#ifdef DEBUG_MACROS
					printf(", nrv2x block size: %d", packed_block_size);
#endif
					Read(fileHandle, packed_block, packed_block_size);
					Destination = (**bitmap).Planes[0];
					nrv2s_unpack(packed_block, Destination);
#ifdef DEBUG_MACROS
					printf(", loaded packed plane #%d", i);
#endif
				}
				else
					printf(err_no_size_found);
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
		printf(err_cannot_open_file, name);
		return FALSE;
	}

	return TRUE;
}

BOOL load_pak_img_to_new_bitmap(struct BitMap **new_bitmap, amiga_color **new_palette, BYTE *packed_block, UBYTE *name)
{
	BPTR fileHandle;
	char tag[4];
	UWORD w, h, d, pal_size, packed_block_size;
	BOOL self_alloc_unpack_buffer = FALSE;
	// BYTE *packed_block;
	// PLANEPTR plane_prt;

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
			printf(err_no_impk_found);
		}
		else
		{
			/* Read image geometry */
			ULONG size;
			Read(fileHandle, &w, 2); /* image width in pixels */
			Read(fileHandle, &h, 2); /* image height in pixels */
			Read(fileHandle, &d, 2); /* image depth */
			size = RASSIZE(w, h);	/* size of a single bitplane, in bytes */
			pal_size = 1 << d;
#ifdef DEBUG_MACROS
			printf("w=%d, h=%d, d=%d, palette_size=%d, plane_size=%d,", w, h, d, pal_size, (int)size);
#endif
			/* Read color palette (if available) */
			Read(fileHandle, &tag, 4);
			if (strncmp(tag, "PAL4", 4) == 0)
			{
				if (new_palette != NULL)
				{
					color444 fcolor; // Color from file
					*new_palette = (amiga_color *)AllocMem(pal_size * sizeof(amiga_color), 0L);
					for (i = 0; i < pal_size; i++)
					{
						Read(fileHandle, &fcolor, 2);
					#ifdef VGA_CAPABLE
						(*new_palette)[i] = rgb4_to_rgb8(fcolor); // from RGB444 to RGB888
					#else
						(*new_palette)[i] = fcolor; // from RGB444 to RGB444
					#endif						
					}
				}
				else
					Seek(fileHandle, pal_size * 2, OFFSET_CURRENT);
			}
			else
			{
				if (strncmp(tag, "PAL8", 4) == 0)
				{
					if (new_palette != NULL)
					{
						color888 fcolor; // Color from file
						*new_palette = (amiga_color *)AllocMem(pal_size * sizeof(amiga_color), 0L);
						for (i = 0; i < pal_size; i++)
						{
							Read(fileHandle, &fcolor, 4);
						#ifdef VGA_CAPABLE
							(*new_palette)[i] = fcolor; // from RGB888 to RGB888
						#else
							(*new_palette)[i] = rgb8_to_rgb4(fcolor); // from RGB888to RGB444
						#endif
						}
					}
					else
						Seek(fileHandle, pal_size * 4, OFFSET_CURRENT);
				}
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
					// printf("!!!!MINIZ block size: %d\n", packed_block_size);
					tinfl_decompress_mem_to_mem((**new_bitmap).Planes[0], size * d, packed_block, packed_block_size, 1);
					if (self_alloc_unpack_buffer && packed_block != NULL)
					{
						FreeMem(packed_block, packed_block_size);
						packed_block = NULL;
					}
#ifdef DEBUG_MACROS
					printf(", loaded packed plane #%d", i);
#endif
				}
				else
					printf(err_no_size_found);
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
					if (self_alloc_unpack_buffer && packed_block != NULL)
					{
						FreeMem(packed_block, packed_block_size);
						packed_block = NULL;
					}
#ifdef DEBUG_MACROS
					printf(", loaded packed plane #%d", i);
#endif
				}
				else
					printf(err_no_size_found);
			}
			else if (strncmp(tag, "NRV2", 4) == 0)
			{
#ifdef DEBUG_MACROS
				printf(", found 'nrv2x' data!");
#endif
				Read(fileHandle, &tag, 4);
				if (strncmp(tag, "SIZE", 4) == 0)
				{
					UBYTE *Destination;

					Read(fileHandle, &packed_block_size, 2);
					if (self_alloc_unpack_buffer)
						packed_block = AllocMem(packed_block_size * sizeof(BYTE), MEMF_CLEAR);
#ifdef DEBUG_MACROS
					printf(", nrv2x block size: %d", packed_block_size);
#endif
					Read(fileHandle, packed_block, packed_block_size);
					Destination = (**new_bitmap).Planes[0];
					nrv2s_unpack(packed_block, Destination);
					if (self_alloc_unpack_buffer && packed_block != NULL)
					{
						FreeMem(packed_block, packed_block_size);
						packed_block = NULL;
					}
#ifdef DEBUG_MACROS
					printf(", loaded packed plane #%d", i);
#endif
				}
				else
					printf(err_no_size_found);
			}					
#ifdef DEBUG_MACROS
			printf("\n");
#endif
			return TRUE;
		}
	}
	else
	{
		printf(err_cannot_open_file, name);
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
		printf(err_cannot_open_file, name);
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
	if (allocated_bitmap)
	{
		UWORD i;

#ifdef DEBUG_MACROS		
		printf("free_allocated_bitmap() allocated_bitmap = %x\n", (int)allocated_bitmap);
		printf("allocated_bitmap, BytesPerRow = %d, Rows = %d, Depth = %d, pad = %d\n",
		      (*allocated_bitmap).BytesPerRow,
		      (*allocated_bitmap).Rows,
		      (*allocated_bitmap).Depth,
		      (int)(*allocated_bitmap).pad);
#endif
		if (allocated_bitmap->Planes[0] != NULL)
			FreeMem(allocated_bitmap->Planes[0], RASSIZE(allocated_bitmap->BytesPerRow << 3, allocated_bitmap->Rows) * allocated_bitmap->Depth);
		else
			printf("free_allocated_bitmap() error, plane ptr should not be NULL!\n");		

		for (i = 0; i < allocated_bitmap->Depth; i++)
		{
#ifdef DEBUG_MACROS				
			printf("FreeMem() plane[%i], adr = %x, block_len = %i\n", i, allocated_bitmap->Planes[i], (int)block_len);
#endif
			if (allocated_bitmap->Planes[i] != NULL)
				allocated_bitmap->Planes[i] = NULL;
		}

		if (allocated_bitmap != NULL)
		{
			FreeMem(allocated_bitmap, (LONG)sizeof(struct BitMap));
			allocated_bitmap = NULL;
		}
	}
}

#endif