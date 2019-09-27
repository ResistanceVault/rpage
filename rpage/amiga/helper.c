/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#include "rpage/amiga/includes.prl"

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct Custom far custom;

UBYTE is_cpu_a_020(void) {
	// printf("Kickstart v%d, ", SysBase->LibNode.lib_Version);

	if (SysBase->AttnFlags && AFF_68020) {
	// 	printf("CPU is a 68020 or higher.\n");
		return 1;
	}

	// printf("CPU is a 68000 or 68010.\n");
	return 0;
}
#endif