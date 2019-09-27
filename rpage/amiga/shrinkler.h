/*	Shrinkler executable file compressor for Amiga

	Copyright 1999-2015 Aske Simon Christensen, with exceptions noted below.

	Permission is hereby granted to anyone obtaining a copy of this software
	package (including accompanying documentation) to compile, use, copy,
	modify, merge and/or distribute it, in whole or in part, subject to the
	following conditions:

	- Distribution in source code form must include a copy of this license.

	- Distribution in binary form must not be misattributed, i.e. you must
	  not claim (implicitly or explicitly) that you wrote it yourself.

	- Distribution of the decrunch headers (Header.S, MiniHeader.S,
	  OverlapHeader.S, and the .bin and .dat files generated from them) in
	  binary form as part of an Amiga executable is not restricted by this
	  license and does not require attribution.
	  In particular, output executables from Shrinkler (which contain code
	  from the decrunch headers) are to be considered original works of the
	  author(s) of the corresponding input executables.

	- The data decompression code (ShrinklerDecompress.S) is distributed
	  alongside the Shrinkler binaries in the official archives and has its
	  own license stated inside the file.

*/

#ifdef LATTICE
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/exec_pragmas.h>
#include <libraries/dos.h>
#include <dos/rdargs.h>
#include <utility/tagitem.h>
#include <devices/audio.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <string.h>
#include <hardware/cia.h>

extern __asm ULONG ShrinklerDecompress(	register __a0 UBYTE *Source, /* Input buffer to be decompressed */
										register __a1 UBYTE *Destination, /* Output buffer. */
										register __a2 UBYTE *Callback,
										register __a3 UBYTE *Callback_arg); 

 #endif