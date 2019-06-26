/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#include "frwk/amiga/includes.prl"
#include "frwk/amiga/debug.h"

void free_mem_checked(void *memoryBlock, ULONG byteSize) {
	ULONG _mem_check_0, _mem_check_1;

#ifdef DEBUG_MACROS
	_mem_check_0 = AvailMem(MEMF_ANY);
#endif
	FreeMem(memoryBlock, byteSize);

#ifdef DEBUG_MACROS
	_mem_check_1 = AvailMem(MEMF_ANY);
	if (_mem_check_1 < _mem_check_0)
		printf("FreeMem() byteSize %d, freed %d.\n", byteSize, _mem_check_1 - _mem_check_0);
	else
		printf("FreeMem() OK!\n");
#endif
}
#endif