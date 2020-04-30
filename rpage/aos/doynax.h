/* Doynamite 68k ASM decompression routine by Oxyron.
   Lempel-Ziv decompressor by Johan "Doynax" Forslöf.
   See: https://github.com/AxisOxy/Planet-Rocklobster/tree/master/tools/doynamite68k
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

#define DOYNAX_ADDR_PAD(A) ((A * 5) / 100)
#define DOYNAX_ALLOC_PAD(A) (A + (2 * DOYNAX_ADDR_PAD(A)))

extern __asm void doynaxdepack(	register __a0 UBYTE *Source, /* Input buffer to be decompressed */
								register __a1 UBYTE *Destination); /* Output buffer. */ 

 #endif