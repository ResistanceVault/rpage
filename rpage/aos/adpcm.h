#ifndef __MT_ADPCM__
#define __MT_ADPCM__

#include <exec/types.h>
#define SWAP(x) x
#define SWAPW(x) x
#define attrib(x)

#define MAKE_ID(a,b,c,d)  SWAP((  (a)<<24 |  (b)<<16 | (c)<<8 | (d) ))


static int Matrix[3][8] =
{
	{0x3800, 0x5600, 0, 0, 0, 0, 0, 0},
	{0x399a, 0x3a9f, 0x4d14, 0x6607, 0, 0, 0, 0},
	{0x3556, 0x3556, 0x399A, 0x3A9F, 0x4200, 0x4D14, 0x6607, 0x6607},
};

static int bitmask[9] =
{
	0, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
};

// struct ADPCMHeader
// {
// 	UBYTE Identifier[6];
// 	ULONG Frequency;
// }; // attrib(packed);

// struct Voice8Header {
//     ULONG oneShotHiSamples, /* # samples in the high octave 1-shot part */
//           repeatHiSamples,  /* # samples in the high octave repeat part */
//           samplesPerHiCycle;/* # samples/cycle in high octave, else 0   */
//     UWORD samplesPerSec;    /* data sampling rate                       */
//     UBYTE ctOctave,         /* # octaves of waveforms                   */
//           sCompression;     /* data compression technique used          */
//     LONG volume;           /* playback volume from 0 to Unity (full
//                              * volume). Map this value into the output
//                              * hardware's dynamic range.                */
// }; // attrib(packed) attrib(packed);

// struct SVXHeader
// {
// 	ULONG FORM;
// 	ULONG fSize;
// 	ULONG SVX, VHDR;
// 	ULONG vSize;
// 	struct Voice8Header v8h;
// 	ULONG ANNO;
// 	ULONG aSize;
// 	UBYTE aData[20];
// 	ULONG BODY;
// 	ULONG bSize;
// }; // attrib(packed) attrib(packed);

// struct WAVHeaderFmt
// {
// 	UWORD	AudioFormat;
// 	UWORD	NumOfChan;
// 	ULONG	SamplesPerSec;
// 	ULONG	bytesPerSec;
// 	UWORD	blockAlign;
// 	UWORD	bitsPerSample;
// }; // attrib(packed) attrib(packed);

// struct WAVHeader
// {
// 	ULONG	RIFF;
// 	ULONG	ChunkSize;
// 	ULONG	WAVE;
// 	ULONG	fmt;
// 	ULONG	Subchunk1Size;
// 	struct WAVHeaderFmt whf;
// 	ULONG	Subchunk2ID;
// 	ULONG	Subchunk2Size;
// }; // attrib(packed) attrib(packed); 

#endif /* __MT_ADPCM__ */
