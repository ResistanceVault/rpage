#ifdef LATTICE
/*   
		 EASY-SOUND   V2.00   1990-09-23   ANDERS BJERIN
		 ADPCM Decoder by Kalms
*/

/* Include some important header files: */
#include "rpage/aos/inc.prl"
#include <exec/types.h>
#include <exec/memory.h>
#include <devices/audio.h>
#include <stdio.h>
#include <string.h>

#include "ext/tinfl.h"
#include "rpage/aos/shrinkler.h"
#include "rpage/aos/doynax.h"

#include "rpage/aos/sound.h"
#include "rpage/err.h"

#define CLOCK_CONSTANT 3579545
#define MUSIC_PRIORITY 0

extern struct DosLibrary *DOSBase;

/* An IOAudio pointer to each sound channel: */
struct IOAudio *IOA[4] = {NULL, NULL, NULL, NULL};

/* Intel ADPCM step variation table */
static int indexTable[16] = {
		-1,
		-1,
		-1,
		-1,
		2,
		4,
		6,
		8,
		-1,
		-1,
		-1,
		-1,
		2,
		4,
		6,
		8,
};

static int stepsizeTable[89] = {
		7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
		19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
		50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
		130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
		337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
		876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
		2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
		5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
		15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767};

static int indexTable68000[16];
static int stepsizeTable68000[89 * 16];

/// Load a packer sound file. <br>
/// The sound is encoded either in raw or mdpcm. <br>
/// The file is compressed either in tinfl, doynamite68k or shrinkler.
/// File structure:
/// SMPK (4 bytes) 'Sample Packer' Header
/// SIZE (4 bytes) 'Size information block' header
/// ____ (4 bytes) Size of the original uncompressed & decoded sample
/// ADPC|GLI2|8SVX (4 bytes) Encoder type. ADPC = ADPCM, GLI2 = MDPCM, 8SVX = Amiga 8bits IFF sample.
/// SIZE (4 bytes) 'Size information block' header
/// ____ (4 bytes) Size of the encoded sample
/// MINZ|SHRK|D68K (4 bytes) Compressor type. MINZ = Tinfl Miniz, SHRK = Shrinkler, D68K = Doynamite68k
/// SIZE (4 bytes) 'Size information block' header
/// ____ (4 bytes) Size of the compressed block
SoundInfo *LoadPackedSound(char *filename, BYTE *packed_block, BYTE *unpacked_block)
{
	ULONG encoded_block_size = 0, unpacked_block_size = 0, packed_block_size = 0;
	ULONG frequency = 8000;
	BPTR fileHandle;
	char encoder_tag[4], compressor_tag[4], tag[4];
	UWORD mod_size;
	BYTE /* *unpacked_block, */ *encoded_block;
	SoundInfo *sound;

	// printf("LoadPackedSound(%s)\n", filename);

	if ((fileHandle = Open(filename, MODE_OLDFILE)))
	{
		Read(fileHandle, &tag, 4);
		if (strncmp(tag, "SMPK", 4) == 0)
		{
			// read the sample length
			Read(fileHandle, &tag, 4); // SIZE
			if (strncmp(tag, "SIZE", 4) == 0)
			{
				Read(fileHandle, &unpacked_block_size, 4);
				// printf("unpacked_block_size = %d\n", unpacked_block_size);

				// read the replay frequency
				Read(fileHandle, &tag, 4); // FREQ
				Read(fileHandle, &frequency, 4);

				// read the encoder name
				Read(fileHandle, &encoder_tag, 4);
				// printf("encoder found : %c%c%c%c.\n", encoder_tag[0],encoder_tag[1],encoder_tag[2],encoder_tag[3]);

				Read(fileHandle, &tag, 4);
				if (strncmp(tag, "SIZE", 4) == 0)
				{
					Read(fileHandle, &encoded_block_size, 4);
					// printf("encoded_block_size = %d\n", encoded_block_size);

					// read the compressor name
					Read(fileHandle, &compressor_tag, 4);
					// printf("compressor found : %c%c%c%c.\n", compressor_tag[0],compressor_tag[1],compressor_tag[2],compressor_tag[3]);
					Read(fileHandle, &tag, 4);
					if (strncmp(tag, "SIZE", 4) == 0)
					{
						Read(fileHandle, &packed_block_size, 4);
						// printf("packed_block_size = %d\n", packed_block_size);
						Read(fileHandle, packed_block, packed_block_size);
					}
					else
						printf(err_no_size_found); // compressor
				}
				else
					printf(err_no_size_found); // encoder
			}
			else
				printf(err_no_size_found); // original sample

			if ((encoded_block_size > 0) && (unpacked_block_size > 0) && (packed_block_size > 0))
			{
				if (unpacked_block == NULL)
					unpacked_block = AllocMem(unpacked_block_size, MEMF_CHIP);
				encoded_block = (UBYTE *)calloc(encoded_block_size, sizeof(UBYTE));

				if (strncmp(compressor_tag, "MINZ", 4) == 0)
					tinfl_decompress_mem_to_mem(encoded_block, encoded_block_size, packed_block, packed_block_size, 1);
				else if (strncmp(compressor_tag, "SHRK", 4) == 0)
					ShrinklerDecompress(packed_block, encoded_block, NULL, NULL);
				else if (strncmp(compressor_tag, "D68K", 4) == 0)
					doynaxdepack(packed_block, encoded_block);

				if (strncmp(encoder_tag, "ADPC", 4) == 0)
				{
					CodecState c_st;
					adpcm_decode(&c_st, encoded_block, unpacked_block_size, unpacked_block);
				}
				else if (strncmp(encoder_tag, "GLI2", 4) == 0)
				{
					printf("Gligli MDPCM not supported yet!\n");
				}
				else if (strncmp(encoder_tag, "8SVX", 4) == 0)
				{
					memcpy(unpacked_block, encoded_block, unpacked_block_size);
				}

				sound = (SoundInfo *)calloc(1, sizeof(SoundInfo));
				sound->SoundBuffer = unpacked_block;
				sound->FileLength = unpacked_block_size;
				sound->RecordRate = frequency;

				free(encoded_block);

				return sound;
			}
		}
		else
		{
			printf("!Not a SMPK File!\n");
		}
	}

	return NULL;
}

/* Declare the functions we are going to use: */
SoundInfo *PrepareSound(STRPTR file);
BOOL PlaySound(SoundInfo *info, UWORD volume, UBYTE channel,
							 WORD delta_rate, UWORD repeat);
void StopSound(UBYTE channel);
void RemoveSound();

BOOL PrepareIOA(UWORD period, UWORD volume, UWORD cycles, UBYTE channel,
								SoundInfo *info);
UWORD LoadSound(STRPTR filename, SoundInfo *info);
ULONG GetSize(STRPTR filename);
ULONG SizeIFF(STRPTR filename);
UWORD ReadIFF(STRPTR filename, SoundInfo *info);
BOOL MoveTo(STRPTR check_string, FILE *file_ptr);

/// PrepareSound() loads a sampled sound file (IFF or FutureSound) into  <br>
/// a buffer that is automatically allocated. All information about the  <br>
/// sound (record rate, length, buffersize etc) is put into an SoundInfo <br>
/// structure. If PrepareSound() has successfully prepared the sound it  <br>
/// returns a pointer to a SoundInfo structure, otherwise it returns     <br>
/// NULL.                                                                <br>
///                                                                      <br>
/// Synopsis: pointer = PrepareSound( filename );                        <br>
/// pointer:  (CPTR) Actually a pointer to a SoundInfo structure, but    <br>
///           since we do not want to confuse the user, we simply use a  <br>
///           normal memory pointer.                                     <br>
/// filename: (STRPTR) Pointer to a string containing the name of the    <br>
///           sound file. For example�"df0:Explosion.snd".               <br>

SoundInfo *PrepareSound(STRPTR file)
{
	/* Declare a pointer to a SoundInfo structure: */
	SoundInfo *info;

	/* Allocate memory for a SoundInfo structure: (The memory can be of */
	/* any type, and should be cleared.                                 */
	info = (SoundInfo *)AllocMem(sizeof(SoundInfo),
																			MEMF_PUBLIC | MEMF_CLEAR);

	if (info)
	{
		/* The memory have been successfully allocated. */

		/* Get the size of the file, and store it in the SoundInfo struct.: */
		if (info->FileLength = GetSize(file))
		{
			/* Allocate enough memory for the sampled sound, and store a */
			/* pointer to the buffer in the SoundInfo structure:         */
			info->SoundBuffer =
					(BYTE *)AllocMem(info->FileLength, MEMF_CHIP | MEMF_CLEAR);

			if (info->SoundBuffer)
			{
				/* The memory have been successfully allocated. */

				/* Load the sound, and store the record rate in the SoundInfo  */
				/* structure. If the sound could not be loaded, 0 is returned: */
				if (info->RecordRate = LoadSound(file, info))
				{
					/* OK! The sound has successfully been loaded. */

					/* Old FutureSound files were saved in kHz. If the record rate */
					/* is less than one hundered, we know it is an old FutureSound */
					/* file, and simply multiply the rate with one thousand:       */
					/* Astrofra : removed suspicious patch inherited from the past century
					 */
					/* if( info->RecordRate < 100 )
						info->RecordRate *= 1000;
					*/

					/* Return a pointer to the SoundInfo structure. (We return a */
					/* normal memory pointer.)                                   */
					return (info); // return( (CPTR) info ); /* OK! */
				}
				else
				{
					/* ERROR! We could not load the sound! */

					/* Deallocate the memory for the sound buffer: */
					FreeMem(info->SoundBuffer, info->FileLength);
				}
			}
		}
		/* Deallocate the memory the SoundInfo structure: */
		FreeMem(info, sizeof(SoundInfo));
	}

	/* We have not been able to prepare the sound. All allocated memory */
	/* have been deallocated, and we return NULL.                       */
	return (NULL); /* ERROR! */
}

/* PlaySound()                                                          */
/* PlaySound() plays one already prepared sound effect. You can decide  */
/* what volume, which channel should, what rate, and how many times the */
/* sound should be played.                                              */
/*                                                                      */
/* Synopsis: ok = PlaySound( pointer, volume, channel, drate, times );  */
/* ok:       (BOOL) If the sound was played successfully TRUE is        */
/*           returned, else FALSE.                                      */
/* pointer:  (CPTR) Actually a pointer to a SoundInfo structure. This   */
/*           pointer was returned by PrepareSound().                    */
/* volume:   (UWORD) Volume, 0 to 64.                                   */
/* channel:  (UBYTE) Which channel should be used. (LEFT0, RIGHT0,      */
/*           RIGHT1 or LEFT1)                                           */
/* drate:    (WORD) Delta rate. When the sound is prepared, the record  */
/*           rate is automatically stored in the SoundInfo structure,   */
/*           so if you do not want to change the rate, write 0.         */
/* times:    (UWORD) How many times the sound should be played. If you  */
/*           want to play the sound forever, write 0. (To stop a sound  */
/*           call the function StopSound().)                            */

BOOL PlaySound(SoundInfo *info, UWORD volume, UBYTE channel,
							 WORD delta_rate, UWORD repeat)
{
	/* Before we may play the sound, we must make sure that the sound is */
	/* not already being played. We will therefore call the function     */
	/* StopSound(), in order to stop the sound if it is playing:         */
	StopSound(channel);

	/* Call the PrepareIOA() function that will declare and initialize an */
	/* IOAudio structure:                                                 */
	if (PrepareIOA(CLOCK_CONSTANT / info->RecordRate + delta_rate, volume, repeat,
								 channel, info))
	{
		/* We will now start playing the sound: */
		BeginIO((struct IORequest *)IOA[channel]);
		return (TRUE); /* OK! */
	}
	else
		return (FALSE); /* ERROR! */
}

/* StopSound()                                                         */
/* StopSound() will stop the specified audio channel from continuing   */
/* to play the sound. It will also close all devices and ports that    */
/* have been opened, and deallocate some memory that have been         */
/* allocated.                                                          */
/*                                                                     */
/* Synopsis: StopSound( channel );                                     */
/* channel:  (UBYTE) The audio channel that should be stopped. (LEFT0, */
/*           LEFT1, RIGHT0 or RIGHT1.)                                 */

void StopSound(UBYTE channel)
{
	/* Check if the IOAudio structure exist: */
	if (IOA[channel])
	{
		/* 1. Stop the sound: */
		AbortIO((struct IORequest *)IOA[channel]);

		/* 2. If there exist a Sound Device, close it: */
		if (IOA[channel]->ioa_Request.io_Device)
			CloseDevice((struct IORequest *)IOA[channel]);

		/* 3. If there exist a Message Port, delete it: */
		if (IOA[channel]->ioa_Request.io_Message.mn_ReplyPort)
			DeletePort(IOA[channel]->ioa_Request.io_Message.mn_ReplyPort);

		FreeMem(IOA[channel], sizeof(struct IOAudio));
		IOA[channel] = NULL;
	}
}

/* RemoveSound()                                                        */
/* RemoveSound() will stop playing the sound, and deallocate all memory */
/* that was allocated by the PrepareSound() function. Before your       */
/* program terminates, all sound that has been prepared, MUST be        */
/* removed.                                                             */
/*                                                                      */
/* IMPORTANT! The each channel that is currently playing the sound must */
/* be stopped! (Use the StopSound() function.)                          */

/* Synopsis: RemoveSound( pointer );                                    */
/* pointer:  (CPTR) Actually a pointer to a SoundInfo structure.        */

void RemoveSound(SoundInfo *info)
{
	/* IMPORTANT! The sound must have been */
	/* stopped before you may remove it!!! */

	/* Have we allocated a SoundInfo structure? */
	if (info != NULL)
	{
		/* Deallocate the sound buffer: */
		FreeMem(info->SoundBuffer, info->FileLength);

		/* Deallocate the SoundInfo structure: */
		free(info);
		// FreeMem(info, sizeof(SoundInfo));
		// info = NULL;
	}
}

void RemoveSoundStruct(SoundInfo *info)
{
	/* IMPORTANT! The sound must have been */
	/* stopped before you may remove it!!! */
	/* Have we allocated a SoundInfo structure? */
	if (info)
	{
		/* Deallocate the SoundInfo structure: */
		// FreeMem(info, sizeof(SoundInfo));
		free(info);
		// info = NULL;
	}
}

/* PrepareIOA()                                                           */
/* PrepareIOA() allocates and initializes an IOAudio structure.           */
/*                                                                        */
/* Synopsis: ok = PrepareIOA( period, volume, cycles, channel, pointer ); */
/*                                                                        */
/* ok:       (BOOL) If the IOAudio structure was allocated and            */
/*           initialized successfully, TRUE is returned, else FALSE.      */
/* period:   (UWORD) Period time.                                         */
/* volume:   (UWORD) Volume, 0 to 64.                                     */
/* cycles:   (UWORD) How many times the sound should be played.           */
/*           (0 : forever)                                                */
/* channel:  (UBYTE) Which channel should be used. (LEFT0, RIGHT0,        */
/*           RIGHT1 or LEFT1)                                             */
/* pointer:  (CPTR) Actually a pointer to a SoundInfo structure.          */

BOOL PrepareIOA(UWORD period, UWORD volume, UWORD cycles, UBYTE channel,
								SoundInfo *info)
{
	UBYTE ch;

	/* Declare a pointer to a MsgPort structure: */
	struct MsgPort *port;

	/* Allocate space for an IOAudio structure: */
	IOA[channel] = (struct IOAudio *)AllocMem(sizeof(struct IOAudio),
																						MEMF_PUBLIC | MEMF_CLEAR);

	/* Could we allocate enough memory? */
	if (IOA[channel])
	{
		/* Create Message port: */
		if ((port = (struct MsgPort *)CreatePort("Sound Port", 0)) == NULL)
		{
			/* ERROR! Could not create message port! */
			/* Deallocate the IOAudio structure: */
			FreeMem(IOA[channel], sizeof(struct IOAudio));
			IOA[channel] = NULL;

			return (FALSE); /* ERROR! */
		}
		else
		{
			/* Port created successfully! */
			/* Initialize the IOAudion structure: */

			/* Priority: */
			IOA[channel]->ioa_Request.io_Message.mn_Node.ln_Pri = MUSIC_PRIORITY;

			/* Port: */
			IOA[channel]->ioa_Request.io_Message.mn_ReplyPort = port;

			/* Channel: */
			ch = 1 << channel;
			IOA[channel]->ioa_Data = &ch;

			/* Length: */
			IOA[channel]->ioa_Length = sizeof(UBYTE);

			/* Open Audio Device: */
			if (OpenDevice(AUDIONAME, 0, (struct IORequest *)IOA[channel], 0))
			{
				/* ERROR! Could not open the device! */
				/* Delete Sound Port: */
				DeletePort(port);

				/* Deallocate the IOAudio structure: */
				FreeMem(IOA[channel], sizeof(struct IOAudio));
				IOA[channel] = NULL;

				return (FALSE); /* ERROR! */
			}
			else
			{
				/* Device opened successfully! */
				/* Initialize the rest of the IOAudio structure: */
				IOA[channel]->ioa_Request.io_Flags = ADIOF_PERVOL;
				IOA[channel]->ioa_Request.io_Command = CMD_WRITE;
				IOA[channel]->ioa_Period = period;
				IOA[channel]->ioa_Volume = volume;
				IOA[channel]->ioa_Cycles = cycles;

				/* The Audio Chip can of some strange reason not play sampled  */
				/* sound that is longer than 131KB. So if the sound is too long, */
				/* we simply cut it off:                                        */
				if (info->FileLength > 131000)
					IOA[channel]->ioa_Length = 131000;
				else
					IOA[channel]->ioa_Length = info->FileLength;

				// printf("PrepareIOA() ioa_Length = %d\n", IOA[ channel ]->ioa_Length);

				IOA[channel]->ioa_Data = info->SoundBuffer;

				return (TRUE); /* OK! */
			}
		}
	}
	return (FALSE); /* ERROR! */
}

/* LoadSound()                                                         */
/* LoadSound() will load sampled sound that was either saved in IFF or */
/* FutureSound format.                                                 */
/*                                                                     */
/* Synopsis: rate = LoadSound( filename, pointer );                    */
/* rate:     (UWORD) The record rate is returned if the sound was      */
/*           successfully loaded, else 0.                              */
/* filename: (STRPTR) Pointer to a string containing the name of the   */
/*           sound file. For example�"df0:Explosion.snd".              */
/* pointer:  (CPTR) Actually a pointer to a SoundInfo structure.       */

UWORD LoadSound(STRPTR filename, SoundInfo *info)
{
	FILE *file_ptr;		 /* Pointer to a file. */
	ULONG length;			 /* Data Length. */
	UWORD record_rate; /* Record rate. */

	/* Check if it is an IFF File: */
	if (SizeIFF(filename))
	{
		/* Yes, it is an IFF file. Read it: */
		return (ReadIFF(filename, info));
	}
	else
	{
		/* No, then it is probably a FutureSound file. */
		/* Open the file so we can read it:            */
		if ((file_ptr = fopen(filename, "r")) == 0)
			return (0); /* ERROR! Could not open the file! */

		/* Read the data length: */
		if (fread((char *)&length, sizeof(ULONG), 1, file_ptr) == 0)
		{
			/* ERROR! Could not read the data length! */
			/* Close the file, and return zero:       */
			fclose(file_ptr);
			return (0);
		}

		/* Read the record rate: */
		if (fread((char *)&record_rate, sizeof(UWORD), 1, file_ptr) == 0)
		{
			/* ERROR! Could not read the record rate! */
			/* Close the file, and return zero:       */
			fclose(file_ptr);
			return (0);
		}

		/* Read the sampled sound data into the buffer: */
		if (fread((char *)info->SoundBuffer, length, 1, file_ptr) == 0)
		{
			/* ERROR! Could not read the data!  */
			/* Close the file, and return zero: */
			fclose(file_ptr);
			return (0);
		}

		/* Close the file: */
		fclose(file_ptr);

		/* Return the record rate: */
		// printf("LoadSound() record_rate = %d\n", record_rate);
		return (record_rate);
	}
}

/* GetSize()                                                         */
/* GetSize() returns the size of the file which was saved in either  */
/* IFF or FutureSound format.                                        */
/*                                                                   */
/* Synopsis: length = GetSize( filename );                           */
/* length:   (ULONG) Data length.                                    */
/* filename: (STRPTR) Pointer to a string containing the name of the */
/*           sound file. For example�"df0:Explosion.snd".            */

ULONG GetSize(STRPTR filename)
{
	FILE *file_ptr; /* Pointer to a file. */
	ULONG length;		/* Data length. */

	/* Check if it is an IFF File: */
	if ((length = SizeIFF(filename)) == 0)
	{
		/* No, then it is probably a FutureSound file. */
		/* Open the file so we can read it:            */
		if ((file_ptr = fopen(filename, "r")) == 0)
			return (0); /* ERROR! Could not open the file! */

		/* Read the data length: */
		if (fread((char *)&length, sizeof(ULONG), 1, file_ptr) == 0)
		{
			/* ERROR! Could not read the data length! */
			/* Close the file, and return zero:       */
			fclose(file_ptr);
			return (0);
		}

		/* Close the file: */
		fclose(file_ptr);
	}
	// printf("GetSize() length = %d\n", length);
	return (length);
}

/* SizeIFF()                                                         */
/* SizeIFF() returns the size of an IFF file, or zero if something   */
/* went wrong (for example, It was not an IFF file).                 */
/*                                                                   */
/* Synopsis: length = SizeIFF( filename );                           */
/* length:   (ULONG) Data length.                                    */
/* filename: (STRPTR) Pointer to a string containing the name of the */
/*           IFF file. For example�"df0:Explosion.snd".              */

ULONG SizeIFF(STRPTR filename)
{
	FILE *file_ptr;								/* Pointer to a file. */
	STRPTR empty_string = "    "; /* Four spaces. */
	LONG dummy;										/* A dummy variable. */
	Voice8Header Header;					/* Voice8Header structure. */

	/* Try to open the file: */
	if (file_ptr = fopen(filename, "r"))
	{
		fread((char *)empty_string, 4, 1, file_ptr);
		if (strcmp(empty_string, "FORM") == 0)
		{
			/* Read twice: */
			fread((char *)empty_string, 4, 1, file_ptr);
			fread((char *)empty_string, 4, 1, file_ptr);

			/* Check if it is a "8SVX" file, or not: */
			if (strcmp(empty_string, "8SVX") == 0)
			{
				MoveTo("VHDR", file_ptr);
				fread((char *)&dummy, sizeof(LONG), 1, file_ptr);
				fread((char *)&Header, sizeof(Header), 1, file_ptr);

				/* Close the file, and return the length: */
				fclose(file_ptr);

				// printf("SizeIFF() oneShotHiSamples + repeatHiSamples = %d\n",
				// Header.oneShotHiSamples + Header.repeatHiSamples);
				return (Header.oneShotHiSamples + Header.repeatHiSamples);
			}
		}
		/* Close the file: */
		fclose(file_ptr);
	}
	/* Return zero: (ERROR) */
	return (0);
}

/* ReadIFF()                                                           */
/* ReadIFF() reads an IFF file into the buffer, and returns the record */
/* rate.                                                               */
/*                                                                     */
/* Synopsis: rate = ReadIFF( filename, pointer );                      */
/* rate:     (UWORD) The record rate is returned if the sound was      */
/*           successfully loaded, else 0.                              */
/* filename: (STRPTR) Pointer to a string containing the name of the   */
/*           sound file. For example�"df0:Explosion.snd".              */
/* pointer:  (CPTR) Actually a pointer to a SoundInfo structure.       */

UWORD ReadIFF(STRPTR filename, SoundInfo *info)
{
	FILE *file_ptr;								/* Pointer to a file. */
	STRPTR empty_string = "    "; /* Four spaces. */
	LONG dummy;										/* A dummy variable. */
	Voice8Header Header;					/* Voice8Header structure. */

	/* Try to open the file: */
	if (file_ptr = fopen(filename, "r"))
	{
		fread((char *)empty_string, 4, 1, file_ptr);
		if (strcmp(empty_string, "FORM") == 0)
		{
			/* Read twice: */
			fread((char *)empty_string, 4, 1, file_ptr);
			fread((char *)empty_string, 4, 1, file_ptr);

			/* Check if it is a "8SVX" file, or not: */
			if (strcmp(empty_string, "8SVX") == 0)
			{
				MoveTo("VHDR", file_ptr);
				fread((char *)&dummy, sizeof(LONG), 1, file_ptr);
				fread((char *)&Header, sizeof(Header), 1, file_ptr);

				MoveTo("BODY", file_ptr);
				fread((char *)&dummy, sizeof(LONG), 1, file_ptr);
				fread((char *)info->SoundBuffer,
							Header.oneShotHiSamples + Header.repeatHiSamples, 1, file_ptr);

				/* Close the file, and return the record rate: */
				fclose(file_ptr);

				// printf("ReadIFF() samplesPerSec = %d\n", Header.samplesPerSec);
				return (Header.samplesPerSec);
			}
		}
		/* Close the file: */
		fclose(file_ptr);
	}
	/* Return zero: (ERROR) */
	return (0);
}

/* MoveTo()                                                  */
/* MoveTo() walks through an IFF file, and looks for chunks. */
/*                                                           */
/* Synopsis: MoveTo( chunk, file_ptr );                      */
/* chunk:    (STRPTR) The chunk we want to get to.           */
/* file_ptr: (FILE *) Pointer to an already opened file.     */

BOOL MoveTo(STRPTR check_string, FILE *file_ptr)
{
	STRPTR empty_string = "    "; /* Four spaces. */
	int skip, loop;								/* How much data should be skiped. */
	LONG dummy;										/* A dummy variable. */

	/* As long as we have not reached the EOF, continue: */
	while (!feof(file_ptr))
	{
		fread((char *)empty_string, 4, 1, file_ptr);

		/* Have we found the right chunk? */
		if (strcmp(check_string, empty_string) == 0)
			return (0); /* YES! Return nothing. */

		/* Move foreward: */
		fread((char *)&skip, sizeof(LONG), 1, file_ptr);
		for (loop = 0; loop < skip; loop++)
			fread((char *)&dummy, 1, 1, file_ptr);
	}
}

/*
** Intel/DVI ADPCM coder/decoder.
** Adapted for the 68000 Motorola CPU by https://github.com/Kalmalyzer
**
** The algorithm for this coder was taken from the IMA Compatability Project
** proceedings, Vol 2, Number 2; May 1992.
**
** Version 1.2, 18-Dec-92.
*/

void adpcm_decoder_init(void)
{
	int i, delta;
	for (i = 0; i < 16; i++)
		indexTable68000[i] = indexTable[i] << 4;

	for (i = 0; i < 89; i++)
	{
		for (delta = 0; delta < 16; delta++)
		{
			int origPredictor = stepsizeTable[i];
			int predictor = origPredictor >> 3;
			if (delta & 4)
				predictor += origPredictor;
			if (delta & 2)
				predictor += origPredictor >> 1;
			if (delta & 1)
				predictor += origPredictor >> 2;
			if (delta & 8)
				predictor = -predictor;
			stepsizeTable68000[i * 16 + delta] = predictor;
		}
	}
}

void adpcm_decode(CodecState *state, UBYTE *input, int numSamples, UBYTE *output)
{
	UBYTE *inp;			 /* Input buffer pointer */
	UBYTE *outp;		 /* output buffer pointer */
	int delta;			 /* Current adpcm output value */
	int valpred;		 /* Predicted value */
	int vpdiff;			 /* Current change to valpred */
	int index;			 /* Current step change index */
	int inputbuffer; /* place to keep next 4-bit value */
	int bufferstep;	 /* toggle between inputbuffer/input */

	outp = output;
	inp = input;
	inputbuffer = *inp;

	valpred = state->valprev;
	index = state->index;

	bufferstep = 0;

	for (; numSamples > 0; numSamples--)
	{

		if (bufferstep)
		{
			delta = inputbuffer & 0xf;
		}
		else
		{
			inputbuffer = *inp++;
			delta = (inputbuffer >> 4) & 0xf;
		}
		bufferstep = !bufferstep;

		vpdiff = stepsizeTable68000[index | delta];
		index += indexTable68000[delta];
		if (index < 0)
			index = 0;
		if (index > (88 << 4))
			index = (88 << 4);

		valpred += vpdiff;

		if (valpred > 32767)
			valpred = 32767;
		else if (valpred < -32768)
			valpred = -32768;

		*outp++ = (valpred >> 9);
	}

	state->valprev = valpred;
	state->index = index;
}

#endif