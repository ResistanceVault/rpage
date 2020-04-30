/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef WIN32
#include "rpage/frwk.h"
#include "rpage/win32/screen_size.h"
#include "rpage/utils.h"
#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>
#include <sysinfoapi.h>

/*
Graphic assets
*/
#include "rpage/aos/screen_size.h"

short scr_x_offset = 0, scr_y_offset = 0;
SDL_Window *main_window = NULL;
SDL_Renderer *main_renderer = NULL;
SDL_Event input_events;

/* platform interface Win32 implementation */

void rpage_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		rpage_system_alert("Could NOT open the Intuition library!");
		rpage_uninit();
		exit(0);
	}

	// atexit(SDL_Quit);
}

void rpage_uninit(void)
{
	SDL_Quit();
}

/*
	SYSTEM (resources, memory, multitasking...)
	-------------------------------------------
*/

BYTE rpage_set_process_priority(BYTE new_priority)
{
	// Not implemented!
    return 0;
}

void rpage_free_memory_block(BYTE *block_ptr, UWORD block_size)
{
}

ULONG  rpage_get_avail_video_memory(void)
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return (ULONG)status.ullTotalPhys;
}

void rpage_system_alert(char *alert_message)
{
	printf("%s\n", alert_message);
}

void rpage_system_flash(void)
{
}

ULONG rpage_get_clock(void)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
    return st.wMilliseconds;
}

/*
	VIDEO (video framebuffer access)
	--------------------------------
*/

void rpage_video_open(int screen_mode)
{
	if (main_window == NULL)
	{
		main_window = SDL_CreateWindow("Athanor 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPL_WIDTH, DISPL_HEIGHT, 0);

		if (main_window)
		{
			SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "nearest");
			main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
			SDL_RenderSetLogicalSize(main_renderer, WIDTH, HEIGHT);
			SDL_SetRenderDrawColor(main_renderer, 255, 0, 255, 255);
		}
		else
		{
			printf("Couldn't create a surface: %s\n", SDL_GetError());
			exit(0);
		}
	}
	else
	{
		rpage_system_alert("A screen is already open!");
		exit(0);
	}
}

void rpage_video_vsync(void)
{
	SDL_RenderPresent(main_renderer);
}

void rpage_video_clear(void)
{
	SDL_RenderClear(main_renderer);
}

void rpage_video_blt_bmp(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y)
{
	SDL_Rect src, dst;
	src.x = source_x;
	src.y = source_y;
	src.w = width;
	src.h = height;
	dst.x = x;
	dst.y = y;
	dst.w = width;
	dst.h = height;

	SDL_RenderCopy(main_renderer, source_bitmap, &src, &dst);
}

void rpage_video_blt_bmp_mask(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *mask_bitmap)
{
}

void rpage_video_set_palette(rpage_palette *palette, short palette_size)
{
}

void rpage_video_draw_rect(rect *r, short color)
{
}

void rpage_video_set_pixel(short x, short y, short color)
{

}

void rpage_video_set_font(char *font_filename, short font_size)
{
}

void rpage_video_draw_text(char *str, short x, short y, short color)
{
	printf("rpage_video_draw_text(%s)\n", str);
}

void rpage_video_close(void)
{
	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);
	main_renderer = NULL;
	main_window = NULL;
}

/*
	BITMAP (direct bitmap access)
	-----------------------------
*/

ULONG rpage_bitmap_calculate_bytesize(short width, short height, short depth)
{
	return (width * height);
}

rpage_bitmap *rpage_bitmap_new(short width, short height, short depth)
{
	return (rpage_bitmap *)SDL_CreateTexture(main_renderer, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STATIC, width, height);
}

BOOL rpage_load_pak_into_bitmap(rpage_bitmap **bitmap, rpage_palette **palette, BYTE *packed_buffer, char *bitmap_filename)
{
	(*bitmap) = IMG_LoadTexture(main_renderer, bitmap_filename);
	if ((*bitmap) == NULL)
	{
		rpage_system_alert("Cannot load bitmap!");
		return FALSE;
	}
	return TRUE;
}

short rpage_bitmap_get_width(rpage_bitmap *bitmap)
{
	int w;
	SDL_QueryTexture((SDL_Texture *)bitmap, NULL, NULL, &w, NULL);
	return w;
}

short rpage_bitmap_get_height(rpage_bitmap *bitmap)
{
	int h;
	SDL_QueryTexture((SDL_Texture *)bitmap, NULL, NULL, NULL, &h);
	return h;
}

BOOL rpage_load_pak_to_new_bitmap(rpage_bitmap **new_bitmap, rpage_palette **new_palette, BYTE *packed_buffer, char *bitmap_filename)
{
    return FALSE;
}

void rpage_bitmap_free(rpage_bitmap *bitmap)
{
	SDL_DestroyTexture((SDL_Texture *)bitmap);
}

BOOL rpage_input_init(void)
{
	// In SDL2, the mouse & keyboard don't need a specific subsystem to be initialized.
	return TRUE;
}

void rpage_input_update(void)
{
	SDL_PollEvent(&input_events);
}

void rpage_mouse_button_flush(void)
{
}

BOOL rpage_mouse_button_left_is_down(void)
{
    return FALSE;
}

BOOL rpage_mouse_button_right_is_down(void)
{
    return FALSE;
}

BOOL rpage_mouse_button_left_was_down(void)
{
    return FALSE;
}

BOOL rpage_mouse_button_right_was_down(void)
{
    return FALSE;
}

void rpage_mouse_get_values(short *button, vec2 *mouse_coords)
{
}

void rpage_mouse_show(void)
{
	SDL_ShowCursor(SDL_ENABLE);
}

void rpage_mouse_wait(void)
{
}

void rpage_mouse_hide(void)
{
	SDL_ShowCursor(SDL_DISABLE);
}

#endif