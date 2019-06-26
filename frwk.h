/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifndef _FRAMEWORK_HEADER_
#define _FRAMEWORK_HEADER_

#include "rpage/utils.h"

#ifdef LATTICE
#include <exec/types.h>
#include <graphics/gfx.h>
#include "rpage/amiga/color.h"

typedef struct BitMap rpage_bitmap;
typedef unsigned short rpage_palette;
#define PLATFORM_DRAW_TWICE(_EXPR_){rpage_video_flip_buffers(); _EXPR_; rpage_video_present_screen(); rpage_video_flip_buffers(); _EXPR_; rpage_video_present_screen(); }
#endif

#ifdef WIN32
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <Windows.h>
typedef SDL_Texture rpage_bitmap;
typedef SDL_Color rpage_palette;
#define PLATFORM_DRAW_TWICE(_EXPR_);
#endif



/* Platform-agnostic game interface */

enum rpage_screen_modes
{
    mode_lowres,
    mode_medres,
    mode_hires
};

void rpage_init(void);
void rpage_uninit(void);

BYTE rpage_set_process_priority(BYTE new_priority);
void rpage_system_alert(char *alert_message);
void rpage_system_flash(void);
void rpage_free_memory_block(BYTE *block_ptr, UWORD block_size);
ULONG  rpage_get_avail_video_memory(void);

ULONG rpage_get_clock(void);

void rpage_video_open(int screen_mode);
void rpage_video_close(void);
void rpage_video_clear(void);
void rpage_video_set_font(char *font_filename, short font_size);
void rpage_video_vsync(void);
void rpage_video_wait_dma(void);
void rpage_video_flip_buffers(void);
void rpage_video_present_screen(void);
void rpage_video_sync_buffers(void);

void rpage_video_blt_bmp(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y);
void rpage_video_blt_bmp_mask(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *mask_bitmap);
void rpage_video_blt_bmp_clip(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rect *clipping_rect);
void rpage_video_blt_bmp_clip_mask(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *mask_bitmap, rect *clipping_rect);
void rpage_video_blt_bmp_clip_mask_bt(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *mask_bitmap, rect *clipping_rect, UBYTE bit_mask);
void rpage_video_set_palette(rpage_palette *palette, short palette_size);
void rpage_video_draw_rect(rect *r, short color_index);
void rpage_fill_rect(rect *r, short color);
void rpage_fill_rect_clip(rect *r, short color, rect *clipping_rect);
void rpage_video_set_pixel(short x, short y, short color_index);
void rpage_video_draw_text(char *str, short x, short y, short color_index);

ULONG rpage_calculate_bitmap_bytesize(short width, short height, short depth);
rpage_bitmap *rpage_new_bitmap(short width, short height, short depth);
BOOL rpage_load_pak_into_bitmap(rpage_bitmap **bitmap, rpage_palette **palette, BYTE *packed_buffer, char *filename);
short rpage_bitmap_get_width(rpage_bitmap *bitmap);
short rpage_bitmap_get_height(rpage_bitmap *bitmap);
BOOL rpage_load_pak_to_new_bitmap(rpage_bitmap **new_bitmap, rpage_palette **new_palette, BYTE *packed_buffer, char *bitmap_filename);
void rpage_free_bitmap(rpage_bitmap *bitmap);

BOOL rpage_input_init(void);
void rpage_input_update(void);
void rpage_mouse_button_flush(void);
void rpage_mouse_show(void);
void rpage_mouse_wait(void);
void rpage_mouse_hide(void);
void rpage_mouse_get_values(short *button, vec2 *mouse_coords);
void rpage_mouse_get_prev_values(short *button, vec2 *mouse_coords);
BOOL rpage_mouse_button_left_is_down(void);
BOOL rpage_mouse_button_right_is_down(void);
BOOL rpage_mouse_button_left_was_down(void);
BOOL rpage_mouse_button_right_was_down(void);

#endif