/*  \mainpage My Personal Index Page
    Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifndef _FRAMEWORK_HEADER_
#define _FRAMEWORK_HEADER_

#include "rpage/utils.h"

#ifdef LATTICE
#include <exec/types.h>
#include <graphics/gfx.h>
#include "rpage/amiga/color.h"
#include "rpage/amiga/screen_size.h"

typedef BPTR rpage_file;
typedef struct BitMap rpage_bitmap;
#ifdef VGA_ENABLED
typedef unsigned long rpage_palette;
#else
typedef unsigned short rpage_palette;
#endif
typedef struct SimpleSprite rpage_hardware_sprite;

#define MAX_HARDWARE_SPRITES 8
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

/// Initialize the framework.<br>
/// Open the required system libraries (Graphics, Intuition, DiskFont on the Amiga side), reset the global timer and get the current task ID. 
void rpage_init(void);
/// Un-initialize the framework, close the system libraries.
void rpage_uninit(void);

/// Set the multitasking priority of the current task.<BR>
/// On the Amiga side, this function takes -127 to +127 as a parameter, the higher the more CPU time is allocated to the task.
BYTE rpage_set_process_priority(BYTE new_priority);
/// Opens a GURU MEDITATION message.
void rpage_system_alert(char *alert_message);
/// Use the system function BELL/RING/FLASH to send a visual/audio alert.
void rpage_system_flash(void);
/// Wrapper to the system-specific memory deallocator.
void rpage_free_memory_block(BYTE *block_ptr, UWORD block_size);
/// Return how many free memory is available to store the graphics data (aka Chipram on the Amiga side).
ULONG  rpage_get_avail_video_memory(void);
/// Return the largest free memory block available to store the graphics data (aka Chipram on the Amiga side).
ULONG  rpage_get_avail_largest_video_memory(void);
/// Return how many general purpose free memory is available (aka Fastram/Slowram on the Amiga side).
ULONG  rpage_get_avail_non_video_memory(void);

/// Get the elapsed time, in milliseconds, since ::rpage_init was invoked.
ULONG rpage_get_clock(void);

/// Open the main video output by allocating a blank screen framebuffer.<br>
/// The screenmode is defined by ::rpage_screen_modes. The current enum is tied to the regular Amiga OCS specifications.
void rpage_video_open(int screen_mode);
/// Close the video output, deallocate the screen framebuffer.
void rpage_video_close(void);
/// Get bit depth of the screen
UWORD rpage_video_get_depth(void);
/// Clear the screen.
void rpage_video_clear(void);
/// Set the default font file/size for every next call to ::rpage_video_draw_text
void rpage_video_set_font(char *font_filename, short font_size);
/// Wait for the vertical blank.
void rpage_video_vsync(void);
/// Wait for the completion of all ongoing DMA transfert (including the blitter operation, on the Amiga side).
void rpage_video_wait_dma(void);
/// Tell R-PAGE to redirect every next graphic drawing operations to the logical buffer.<br>
/// If the function is called again, all the drawing operations will be performed in the physical buffer again.<br>
/// This is how double buffering is handled by R-PAGE.
void rpage_video_flip_buffers(void);
/// Tell R-PAGE to swap the logical and the physical buffers. Everything that was drawn into the logical buffer will become visible.
void rpage_video_present_screen(void);
/// Swap logical & physical palettes
void rpage_video_present_palette(void);
/// Hard copy the content of the physical buffer to the logical buffer. On the Amiga side This function may use the Blitter.
void rpage_video_sync_buffers(void);
/// Bring the game screen to top (Amiga only)
void rpage_video_screen_to_front(void);
/// Send the game screen to the back (Amiga only)
void rpage_video_screen_to_back(void);

/// Blit a bitmap into the screen.
void rpage_video_blt_bmp(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y);
/// Blit a bitmap into the screen.<br>
/// A bitmap mask is used to merge the source into the destination. The bitmap mask should be 1bit wide.<br>
void rpage_video_blt_bmp_mask(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *mask_bitmap);
/// Blit a bitmap into the screen.<br>
/// * A 2D clipping operation is done, based on the clipping ::rect provided
void rpage_video_blt_bmp_clip(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rect *clipping_rect);
/// Blit a bitmap into the screen.<br>
/// A bitmap mask is used to merge the source into the destination. The bitmap mask should be 1bit wide.<br>
/// * A 2D clipping operation is done, based on the clipping ::rect provided
void rpage_video_blt_bmp_clip_mask(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *mask_bitmap, rect *clipping_rect);
/// Blit a bitmap into the screen.<br>
/// * A 2D clipping operation is done, based on the clipping ::rect provided
/// * A bitmask is applied to every pixel during the blit.
void rpage_video_blt_bmp_clip_mask_bt(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *mask_bitmap, rect *clipping_rect, UBYTE bit_mask);
/// Set the current palette of the screen.<br>
/// The color format is RGB444.
void rpage_video_set_palette(rpage_palette *palette, short palette_size);
void rpage_video_set_palette_to_black(short first_color, short last_color);
void rpage_video_set_palette_to_grey(short first_color, short last_color);
/// Draw an empty polygon (quad only) to the screen.<br>
/// * Color index range is (0,31) on the Amiga side.
void rpage_video_draw_polygon(poly *p, short color);
/// Draw an empty rect to the screen.<br>
/// * Color index range is (0,31) on the Amiga side.
void rpage_video_draw_rect(rect *r, short color_index);
/// Draw a filled rect to the screen.<br>
/// * Color index range is (0,31) on the Amiga side.
/// * A 2D clipping operation is done, based on the clipping ::rect provided
void rpage_video_fill_rect(rect *r, short color);
void rpage_video_fill_rect_clip(rect *r, short color, rect *clipping_rect);
/// Draw a pixel to the screen.
void rpage_video_set_pixel(short x, short y, short color_index);
/// Get the color of a pixel on screen
short rpage_video_get_pixel(short x, short y);
/// Draw a text string to the screen.<br>
/// * The font must be defined prior to this operation, see ::rpage_video_set_font
void rpage_video_draw_text(char *str, short x, short y, short color_index);
/// Get text width in pixels
short rpage_video_get_text_width(char *str);
/// Draw a tileset-based image to the current screen.
void rpage_video_draw_tileset(rpage_bitmap *tileset_bitmap, UBYTE *tileset, rect *tile_rect, short tileset_width);
void rpage_video_save_to_bitmap(rpage_bitmap *dest_bitmap, short source_x, short source_y, short width, short height);

/// Draw a tileset-based image into a bitmap.
void rpage_bitmap_draw_tileset(rpage_bitmap *dest_bitmap, rpage_bitmap *tileset_bitmap, UBYTE *tileset, rect *tile_rect, short tileset_width);
/// Blit the destination bitmap into the source bitmap.
void rpage_bitmap_blit(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *dest_bitmap);

/// Return the size in bytes of a ::rpage_bitmap.
ULONG rpage_bitmap_calculate_bytesize(short width, short height, short depth);
/// Allocate a new ::rpage_bitmap and return its address in video memory.
rpage_bitmap *rpage_bitmap_new(short width, short height, short depth);
/// Load a .PAK bitmap file into an existing bitmap.<br>
/// * The ::rpage_bitmap and ::rpage_palette must be allocated before calling this function.<br>
/// * the packed_buffer must be allocated before calling this function.<br>
BOOL rpage_load_pak_into_bitmap(rpage_bitmap **bitmap, rpage_palette **palette, BYTE *packed_buffer, char *filename);
/// Return the width in pixels of a ::rpage_bitmap
short rpage_bitmap_get_width(rpage_bitmap *bitmap);
/// Return the height in pixels of a ::rpage_bitmap
short rpage_bitmap_get_height(rpage_bitmap *bitmap);
/// Return the bitwise depth of a ::rpage_bitmap
short rpage_bitmap_get_depth(rpage_bitmap *bitmap);
/// Clear the current bitmap (fill it with 0, aka index 0 color)
void rpage_bitmap_clear(rpage_bitmap *bitmap);
/// Load a .PAK bitmap file into a new bitmap, self allocated by the function.<br>
/// * The ::rpage_bitmap and ::rpage_palette will be automatically allocated by the function.<br>
/// * If packed_buffer equals ::NULL, it will be automatically allocated by the function. Otherwise, R-PAGE will assume it contains the address of an already allocated block of memory.<br>
BOOL rpage_load_pak_to_new_bitmap(rpage_bitmap **new_bitmap, rpage_palette **new_palette, BYTE *packed_buffer, char *bitmap_filename);
/// Free the memory allocated by a ::rpage_bitmap.<br>
/// The possiblity related ::rpage_palette is not automatically freed.
void rpage_bitmap_free(rpage_bitmap *bitmap);

/// Refresh the image/position of an hardware sprite
void rpage_move_sprite(short sprite_index, rpage_hardware_sprite *sprite, vec2 *position);
void rpage_remove_sprite(short sprite_index);
BOOL rpage_sprite_is_enabled(short sprite_index);

/// Initialize the input system.<br>
/// On the Amiga side, the input system needs a Window to be created. This is done automatically by R-PAGE when calling ::rpage_video_open. As a consequence, the input won't work if no screen was created first.
BOOL rpage_input_init(void);
/// Pull the mouse/keyboard update from the input system.
void rpage_input_update(void);
/// Enable or disable the input pooling (Enabled by default)
void rpage_input_enable(BOOL enabled);
/// Flush the latest mouse update from the input system.<br>
/// This maybe required on a multitasking system.
void rpage_mouse_button_flush(void);
/// Show the mouse cursor.
void rpage_mouse_show(void);
/// Change the look of the mouse cursor to warn the end-user that the application is currently busy.
void rpage_mouse_wait(void);
/// Hide the mouse cursor.
void rpage_mouse_hide(void);
///
void rpage_mouse_set_bitmap(UWORD *sprite_data, vec2 *hotspot);
/// Read the current mouse coordinates and button states.
void rpage_mouse_get_values(short *button, vec2 *mouse_coords);
/// Read the previous mouse coordinates and button states.<br>
/// This might be useful when you need to calculate the delta/velocity of the mouse, or check is the mouse buttons where previously pressed. 
void rpage_mouse_get_prev_values(short *button, vec2 *mouse_coords);
/// Test if the left mouse button is currently pressed.
BOOL rpage_mouse_button_left_is_down(void);
/// Test if the right mouse button is currently pressed.
BOOL rpage_mouse_button_right_is_down(void);
/// Test if the left mouse button was pressed but isn't anymore.
BOOL rpage_mouse_button_left_was_down(void);
/// Test if the right mouse button was pressed but isn't anymore.
BOOL rpage_mouse_button_right_was_down(void);
/// 
unsigned short rpage_keyboard_rawkey(void);

#endif