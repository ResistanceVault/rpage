
/* ptreplay.h -- definition of ptreplay.library structures */

#ifndef	LIBRARIES_PTREPLAY_H 
#define LIBRARIES_PTREPLAY_H

#ifndef	EXEC_TYPES_H
#include <exec/types.h>
#endif

#define PTREPLAYNAME "ptreplay.library"

struct Module
{
    STRPTR mod_Name;
/* The rest is private for now, but more details may be released later. */
};

/* This structure is returned by GetSample function */
struct PTSample
{
	UBYTE Name[22];		/* Null terminated string with samplename */
	UWORD Length;		/* Sample length in words */
	UBYTE FineTune;		/* FineTune of sample in lower 4 bits */
	UBYTE Volume;		/* Volume of sample */
	UWORD Repeat;		/* Repeat start in number of words */
	UWORD Replen;		/* Repeat length in number of words */
};

#endif /* LIBRARIES_PTREPLAY_H */
