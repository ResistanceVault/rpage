#ifdef LATTICE
#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include "frwk/amiga/screen_size.h"
#include "frwk/amiga/color.h"
#include "frwk/amiga/screen.h"
#include "frwk/amiga/debug.h"

// #ifdef LATTICE
// int CXBRK(void)    { return(0); }  /* Disable Lattice CTRL/C handling */
// int chkabort(void) { return(0); }  /* really */
// #endif

extern struct Library *IntuitionBase;
extern struct Library *GfxBase;

void __inline WaitVBL(buffered_screen *screen)
{
	// struct Node wait;  /* We cannot use the task's node here as that is
	//                       used to queue the task in Wait() */
	// wait.ln_Name = (char *)FindTask(NULL);
	// SetSignal(0, SIGF_SINGLE);
	// Disable();
	// AddTail((struct List *)&(((struct GfxBase *)GfxBase)->TOF_WaitQ), (struct Node *)&wait);
	// Wait(SIGF_SINGLE);
	// Remove((struct Node *)&wait);
	// Enable();
	WaitBOVP(&(screen->screen->ViewPort));
}

USHORT getLogicalBitmapIndex(buffered_screen *screen)
{
	return (USHORT)((screen->physical)^1);
}

USHORT getPhysicalBitmapIndex(buffered_screen *screen)
{
	return screen->physical;
}

void flipBuffers(buffered_screen *screen)
{
	/* Swap the physical and logical bitmaps */
	screen->physical = (USHORT)(screen->physical)^1;
	screen->screen->RastPort.BitMap	= screen->bitmaps[screen->physical];
	screen->screen->ViewPort.RasInfo->BitMap = screen->bitmaps[screen->physical];
#ifdef DEBUG_MACROS	
	printf("flipBuffers() : physical screen: %d\n", screen->physical);
	printf("flipBuffers() : logical screen: %d\n", getLogicalBitmapIndex(screen));
#endif	
}

void presentScreen(buffered_screen *screen)
{
	/* Update the physical display to match the recently updated bitmap. */
	MakeScreen(screen->screen);
	RethinkDisplay();
#ifdef DEBUG_MACROS	
	printf("presentScreen()\n");
#endif	
}

void synchronizeBuffers(buffered_screen *screen)
{
	BltBitMap(screen->bitmaps[screen->physical], 0, 0, screen->bitmaps[getLogicalBitmapIndex(screen)], 0, 0, WIDTH, HEIGHT, 0xC0, 0xFF, NULL);
	WaitBlit();
}

buffered_screen *openMainScreen(void)
{
	return openMainScreenCustom(WIDTH, HEIGHT, 32, DBUFFER_ENABLED);
}

buffered_screen *openMainScreenCustom(USHORT _width, USHORT _height, USHORT _colors, BOOL _dbuffer)
{
	int i;
	static buffered_screen bscreen;
	struct NewScreen new_screen;
	struct NewWindow new_window;

	screen_geometry this_screen;

	this_screen.width = _width;
	this_screen.height = _height;
	this_screen.depth = color_to_depth(_colors);
	this_screen.view_modes = SPRITES;

#ifdef DEBUG_MACROS
	printf("openMainScreenCustom(%d, %d, %d, %d), depth = %d\n", _width, _height, _colors, _dbuffer, this_screen.depth);
#endif

	bscreen.physical = 0; /* the physical screen (front buffer) has index 0. */

	for(i = 0; i < (_dbuffer?2:1); i++)
		bscreen.bitmaps[i] = setupBitMap(this_screen.depth, this_screen.width, this_screen.height);

#ifdef DEBUG_MACROS
	printf("Allocated %d bitmaps!\n", i);
#endif	

	if (bscreen.bitmaps[i-1] != NULL)
	{
		new_screen.LeftEdge = 0;
		new_screen.TopEdge = 0;
		new_screen.Width = this_screen.width;
		new_screen.Height = this_screen.height;
		new_screen.Depth = this_screen.depth;
		new_screen.DetailPen = 0;
		new_screen.BlockPen = 1;
		new_screen.ViewModes = this_screen.view_modes;
		new_screen.BlockPen = 0;
		new_screen.DetailPen = 0;

		if (_width >= 640)
		{
			new_screen.ViewModes |= HIRES;
#ifdef DEBUG_MACROS			
			printf("openMainScreenCustom(), HIRES.\n");
#endif
		}

		if (_height >= 512)
		{
			new_screen.ViewModes |= LACE;
#ifdef DEBUG_MACROS
			printf("openMainScreenCustom(), INTERLACE.\n");
#endif
		}

		new_screen.Type = CUSTOMSCREEN | CUSTOMBITMAP | SCREENQUIET;
		new_screen.Font = NULL;
		new_screen.DefaultTitle = NULL;
		new_screen.Gadgets = NULL;
		new_screen.CustomBitMap = bscreen.bitmaps[bscreen.physical];

		bscreen.screen = OpenScreen(&new_screen);
		if (bscreen.screen != NULL)
		{
			ShowTitle(bscreen.screen, FALSE);
			/* Blacken the palette */
			// set_palette_to_black(&(screen->ViewPort), 0, _colors);
			set_palette_to_grey(&(bscreen.screen->ViewPort), 0, _colors);

			WaitTOF();

			/* Create a fullscreen window */
			new_window.LeftEdge = 0;
			new_window.TopEdge = 0;
			new_window.DetailPen = 0;
			new_window.BlockPen = 0;
			new_window.Title = NULL;
			new_window.Width = this_screen.width;
			new_window.Height = this_screen.height;
			new_window.BlockPen = 0;
			new_window.DetailPen = 0;
			new_window.IDCMPFlags = IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY;
			new_window.Flags = WFLG_ACTIVATE | WFLG_OTHER_REFRESH | WFLG_RMBTRAP | WFLG_REPORTMOUSE | WFLG_WINDOWACTIVE | WFLG_GIMMEZEROZERO | WFLG_BORDERLESS | WFLG_NOCAREREFRESH;
			new_window.Screen = bscreen.screen;
			new_window.Type = CUSTOMSCREEN;
			new_window.FirstGadget = NULL;
			new_window.CheckMark = NULL;
			new_window.BitMap = NULL;

			bscreen.window = OpenWindow(&new_window);
			if (bscreen.window == NULL)
			{
				printf("openMainScreenCustom(), OpenWindow() failed!\n");
				closeMainScreen(&bscreen);
				return NULL;
			}

			bscreen.screen->FirstWindow = bscreen.window;

			ScreenToFront(bscreen.screen);
			return &bscreen;
		}
		else
		{
			for(i = 0; i  < (_dbuffer?2:1); i++)
				freeBitMap(bscreen.bitmaps[i], this_screen.depth, this_screen.width, this_screen.height);
			printf("openMainScreenCustom(), OpenScreen() failed!\n");
			return NULL;
		}
	}
	else
	{
		printf("openMainScreenCustom(), setupBitMaps() failed!\n");
		return NULL;
	}
}

void closeMainScreen(buffered_screen *screen)
{
	int i;
	screen_geometry this_screen;
#ifdef DEBUG_MACROS
	printf("closeMainScreen()\n");
#endif
	if (screen->screen->FirstWindow != NULL)
		CloseWindow(screen->screen->FirstWindow);

	if (screen->screen != NULL)
	{
		struct BitMap *_bmp[2];
		for(i = 0; i  < ((!(screen->double_buffer_enabled))?1:2); i++)
			_bmp[i] = &(screen->screen->BitMap);

		WaitBlit();
		CloseScreen(screen->screen);

		for(i = 0; i  < ((!(screen->double_buffer_enabled))?1:2); i++)
		{
			this_screen.depth = _bmp[i]->Depth;
			this_screen.width = _bmp[i]->BytesPerRow << 3;
			this_screen.height = _bmp[i]->Rows;
			freeBitMap(_bmp[i], this_screen.depth, this_screen.width, this_screen.height);
		}

		for(i = 0; i  < ((!(screen->double_buffer_enabled))?1:2); i++)
			screen->bitmaps[i] = NULL;
		screen->screen = NULL;
		screen->window = NULL;
	}
	else
		printf("closeMainScreen(): new_screen is NULL!\n");
}

/*
	setupBitMap(): allocate the bit maps for a screen.
*/
struct BitMap *setupBitMap(LONG depth, LONG width, LONG height)
{
	/* static */ struct BitMap *main_bitmap = NULL;

	main_bitmap = (struct BitMap *)AllocMem((LONG)sizeof(struct BitMap), MEMF_CLEAR);
	if (main_bitmap != NULL)
	{
		InitBitMap(main_bitmap, depth, width, height);

		if (setupPlanes(main_bitmap, depth, width, height) != NULL)
			return (main_bitmap);

		freePlanes(main_bitmap, depth, width, height);
	}

	free_mem_checked(main_bitmap, (LONG)sizeof(struct BitMap));
	return NULL;
}

/*
	freeBitMap(): free up the memory allocated by setupBitMap().
*/
VOID freeBitMap(struct BitMap *main_bitmaps, LONG depth, LONG width, LONG height)
{
	freePlanes(main_bitmaps, depth, width, height);
	// free_mem_checked(main_bitmaps, (LONG)sizeof(struct BitMap));
}

/*
	setupPlanes(): allocate the bit planes for a screen bit map.
*/
LONG setupPlanes(struct BitMap *bitMap, LONG depth, LONG width, LONG height)
{
	SHORT plane_num;

	for (plane_num = 0; plane_num < depth; plane_num++)
	{
		bitMap->Planes[plane_num] = (PLANEPTR)AllocRaster(width, height);

		if (bitMap->Planes[plane_num] != NULL)
			BltClear(bitMap->Planes[plane_num], (width / 8) * height, 1);
		else
		{
			freePlanes(bitMap, depth, width, height);
			return (NULL);
		}
	}
	return (TRUE);
}

/*
	freePlanes(): free up the memory allocated by setupPlanes().
*/
VOID freePlanes(struct BitMap *bitMap, LONG depth, LONG width, LONG height)
{
	SHORT plane_num;

	for (plane_num = 0; plane_num < depth; plane_num++)
	{
		if (bitMap->Planes[plane_num] != NULL)
			FreeRaster(bitMap->Planes[plane_num], width, height);
	}
}
#endif