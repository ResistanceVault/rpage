/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#include <stdlib.h>

// #include <exec/types.h>
#include "rpage/aos/inc.prl"

#include "rpage/frwk.h"
#include "ext/tinfl.h"
#include "rpage/aos/locale.h"
#include "ext/aos/shrinkler.h"
#include "ext/aos/nrv2.h"
#include "rpage/err.h"

extern struct DosLibrary *DOSBase;

char *locale_ext[3] = {"_fr.pak", "_en.pak", "_es.pak"};

// #define DEBUG_MACROS TRUE

/// Compressed Text loading
UBYTE *load_pak_locale_to_array(char *text_array[], UWORD array_size, char *filename)
{
    BPTR fileHandle;
	char tag[4], packer_tag[4];
    UWORD unpacked_block_size, packed_block_size;
    UBYTE *packed_block, *unpacked_block = NULL, *str_ptr;
    short i;
#ifdef DEBUG_MACROS
    printf("load_pak_locale_to_array(%s,%d);\n", filename, array_size);
#endif
    if ((fileHandle = Open(filename, MODE_OLDFILE)))
    {
		Read(fileHandle, &tag, 4);
		if (strncmp(tag, "TXPK", 4) != 0)
		{
			printf("cannot found tag 'TXPK'!\n");
		}
		else
		{
#ifdef DEBUG_MACROS            
            printf("found tag 'TXPK'!\n");
#endif
            // Get the original size of the block (before it was packed)
            Read(fileHandle, &unpacked_block_size, 2);
            unpacked_block = (UBYTE *)calloc(unpacked_block_size, sizeof(UBYTE));
#ifdef DEBUG_MACROS
            printf("unpacked_block_size = %d\n", unpacked_block_size);
#endif
            // Get the compression method
            Read(fileHandle, &packer_tag, 4);

            // Look for the packed size
            Read(fileHandle, &tag, 4);
            if (strncmp(tag, "SIZE", 4) == 0)
            {
                // Get the size of the block after it was packed
                Read(fileHandle, &packed_block_size, 2);
#ifdef DEBUG_MACROS            
                printf("packed_block_size = %d\n", packed_block_size);
#endif
                packed_block = (UBYTE *)calloc(packed_block_size, sizeof(UBYTE));
                Read(fileHandle, packed_block, packed_block_size);

                if (strncmp(packer_tag, "MINZ", 4) == 0)
                {
#ifdef DEBUG_MACROS
                    printf("Calling MINZ decoder!\n");
#endif
                    tinfl_decompress_mem_to_mem(unpacked_block, unpacked_block_size, packed_block, packed_block_size, 1);
                }
                else if (strncmp(packer_tag, "SHRK", 4) == 0)
                {
#ifdef DEBUG_MACROS
                    printf("Calling SHRK decoder!\n");
#endif
                    ShrinklerDecompress(packed_block, unpacked_block, NULL, NULL);
                }
                else if (strncmp(packer_tag, "NRV2", 4) == 0)
                {
#ifdef DEBUG_MACROS
                    printf("Calling NRV2X decoder!\n");
#endif
                    nrv2s_unpack(packed_block, unpacked_block);
                }                

                // Transfer the content to an array
                str_ptr = unpacked_block;
                for(i = 0; i < array_size; i++)
                {
                    UWORD str_len;
                    str_len = (*str_ptr) << 8 | (*(str_ptr + 1));
#ifdef DEBUG_MACROS
                    printf("string len = 0x%X/%d, ", str_len, str_len);
#endif
                    str_ptr += 2;
                    text_array[i] = str_ptr; // (char *)calloc(str_len, sizeof(char));
                    // strncpy(text_array[i], str_ptr, str_len);
#ifdef DEBUG_MACROS
                    printf("%s\n", text_array[i]);
#endif
                    str_ptr += str_len;
                }

                // Free the allocated memory
                free(packed_block);
                // free(unpacked_block);
            }
            else
            {
                printf(err_no_size_found);
                printf(", %s\n", tag);
            }
            
        }  
    }

    return unpacked_block;
}