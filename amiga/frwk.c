/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#include "rpage/frwk.h"

#include "rpage/amiga/includes.prl"
#include <time.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>
#include <hardware/custom.h>
#include <graphics/gfxmacros.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <math.h>
#include <time.h>

/*
Common
*/
#include "rpage/amiga/board.h"
#include "rpage/amiga/ptreplay.h"
#include "rpage/amiga/ptreplay_protos.h"
#include "rpage/amiga/ptreplay_pragmas.h"

/*
Routines
*/
#include "rpage/amiga/screen.h"
#include "rpage/amiga/bitmap.h"
#include "rpage/amiga/color.h"
#include "rpage/amiga/helper.h"
#include "rpage/amiga/protracker.h"
#include "rpage/amiga/input.h"
#include "rpage/amiga/time.h"

/*
Graphic assets
*/
#include "rpage/amiga/screen_size.h"
#include "rpage/amiga/mouse_pointer_data.h"
#include "rpage/amiga/debug.h"

struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct DiskfontBase *DiskfontBase;
extern struct Custom far custom;

struct Task *main_task = NULL;
BYTE oldPri;

struct View my_view;
struct View *my_old_view;

/* Main ViewPort */
buffered_screen *main_screen = NULL;
short scr_x_offset = 0, scr_y_offset = 0;

struct TextFont *main_font = NULL;

/* Global clock */
struct timeval startTime;

/* Input System */
short input_mouse_button;
short prev_input_mouse_button;

vec2 input_mouse_position;
vec2 prev_input_mouse_position;

short input_rawkey;

/* platform interface Amiga implementation */

void rpage_init(void)
{
    BYTE error_code;
#ifdef DEBUG_MACROS
    printf("rpage_init()\n");
#endif
    /* Open the Intuition library: */
    if (IntuitionBase == NULL)
    {
        IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
        if (!IntuitionBase)
        {
            rpage_system_alert("Could NOT open the Intuition library!");
            rpage_uninit();
            exit(0);
        }
    }
    else
    {
        rpage_system_alert("Platform already initialized!");
        exit(0);
    }

    /* Open the Graphics library: */
    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
    if (!GfxBase)
    {
        rpage_system_alert("Could NOT open the Graphics library!");
        rpage_uninit();
        exit(0);
    }

    /* Open the DiskFont library: */
    DiskfontBase = (struct DiskfontBase *)OpenLibrary("diskfont.library", 0);
    if (!DiskfontBase)
    {
        rpage_system_alert("Could NOT open the Diskfont library!");
        rpage_uninit();
        exit(0);
    }    

    main_task = FindTask(NULL);
    // oldPri = SetTaskPri(main_task, 16);

    /* Timestamp of the platform startup */
    init_timer_device();
    timer_device_get_system_time(&startTime);

    main_screen = NULL;
}

void rpage_uninit(void)
{
#ifdef DEBUG_MACROS
    printf("rpage_uninit()\n");
#endif
    if (main_task != NULL)
        SetTaskPri(main_task, oldPri);

    uninit_timer_device();

    /* Close the Graphics library: */
    if (GfxBase)
        CloseLibrary((struct Library *)GfxBase);

    /* C Close the Intuition library:  */
    if (IntuitionBase)
        CloseLibrary((struct Library *)IntuitionBase);

    IntuitionBase = NULL;
    GfxBase = NULL;
}

/*
    SYSTEM (resources, memory, multitasking...)
    -------------------------------------------
*/

BYTE rpage_set_process_priority(BYTE new_priority)
{
    return SetTaskPri(main_task, new_priority);
}

ULONG  rpage_get_avail_video_memory(void)
{
    return AvailMem(MEMF_CHIP);
}

void rpage_free_memory_block(BYTE *block_ptr, UWORD block_size)
{
    FreeMem(block_ptr, block_size);
}

void rpage_system_alert(char *alert_message)
{
    char guru_format_message[128];
    short margin_x; 
#ifdef DEBUG_MACROS
    printf("/!\\%s\n", alert_message);
#endif
    memset(guru_format_message, 0, 128);
    strcpy(guru_format_message, "   ");
    if (strlen(alert_message) > 76)
        alert_message[76] = 0x0;
    strcat(guru_format_message, alert_message);
    margin_x = ((640 - strlen(alert_message) * 8) / 2);
    guru_format_message[0] = (margin_x & 0xFF00) >> 8;
    guru_format_message[1] = margin_x & 0xFF;
    guru_format_message[2] = 0xF;
    DisplayAlert(RECOVERY_ALERT, guru_format_message, 32);
}

void rpage_system_flash(void)
{
    DisplayBeep(main_screen->screen);
}

ULONG rpage_get_clock(void)
{
    struct timeval endTime;
 
    timer_device_get_system_time(&endTime);
    SubTime(&endTime, &startTime);

    return (endTime.tv_secs * 1000 + endTime.tv_micro / 1000);
}

/*
    VIDEO (video framebuffer access)
    --------------------------------
*/

void rpage_video_open(int rpage_video_open)
{
#ifdef DEBUG_MACROS
    printf("rpage_video_open()\n");
#endif
    if (main_screen == NULL)
    {
        // if (rpage_video_open == mode_lowres)
            main_screen = openMainScreen();
        // else
        //     main_screen = openMainScreenCustom(320, 512, 32, FALSE); /* Double buffer is DISABLED */

        main_font = NULL;
    }
    else
    {
        rpage_system_alert("A screen is already open!");
        exit(0);
    }
}

void __inline rpage_video_wait_dma(void)
{
    WaitBlit();
}

void __inline rpage_video_vsync(void)
{
    WaitVBL(main_screen);
}

void __inline rpage_video_flip_buffers(void)
{
    flipBuffers(main_screen);
}

void __inline rpage_video_present_screen(void)
{
    presentScreen(main_screen);
}

void rpage_video_sync_buffers(void)
{
    synchronizeBuffers(main_screen);
}

void rpage_video_clear(void)
{
    short i;
    // SetRast(&(main_screen->RastPort), 0);
    // Move(&(main_screen->RastPort), 0, 0);
    // ClearScreen(&(main_screen->RastPort));
    if (main_screen != NULL)
    {
        for (i = 0; i < main_screen->screen->RastPort.BitMap->Depth; i++)
        {
            // memset(main_screen->RastPort.BitMap->Planes[i], 0x0, RASSIZE(main_screen->Width, main_screen->Height));
            BltClear(main_screen->screen->RastPort.BitMap->Planes[i], RASSIZE(main_screen->screen->Width, main_screen->screen->Height), 0);
            // WaitBlit();
        }
    }
    else
    {
        rpage_system_alert("No screen was found open!");
        exit(0);
    }
}

void __inline rpage_bitmap_blit(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *dest_bitmap)
{
    BltBitMap(source_bitmap, source_x, source_y, dest_bitmap, x, y, width, height, 0xC0, 0xFF, NULL);
    // WaitBlit();
}

void __inline rpage_video_blt_bmp(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y)
{
    BltBitMap(source_bitmap, source_x, source_y, main_screen->bitmaps[main_screen->physical], x, y, width, height, 0xC0, 0xFF, NULL);
    // WaitBlit();
}

void __inline rpage_video_blt_bmp_mask(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *mask_bitmap)
{
    BltMaskBitMapRastPort(source_bitmap, source_x, source_y, &(main_screen->screen->RastPort), x, y, width, height, (ABC|ABNC|ANBC), mask_bitmap->Planes[0]);
    // WaitBlit();
}

void __inline rpage_video_blt_bmp_clip(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rect *clipping_rect)
{
    short clip_x = 0, clip_y = 0;

    if (x < clipping_rect->sx)
    {
        clip_x = clipping_rect->sx - x;
        x += clip_x;
        source_x += clip_x;
        width -= clip_x;
    }
    else
    {
        if (x + width > clipping_rect->ex)
            width -= (x + width - clipping_rect->ex);
    }

    if (width <= 0)
        return;
    
    if (y < clipping_rect->sy)
    {
        clip_y = clipping_rect->sy - y;
        y += clip_y;
        source_y += clip_y;
        height -= clip_y;
    }
    else
    {
        if (y + height > clipping_rect->ey)
            height -= (y + height - clipping_rect->ey);
    }
    
    if (height <= 0)
        return;

    BltBitMap(source_bitmap, source_x, source_y, main_screen->bitmaps[main_screen->physical], x, y, width, height, 0xC0, 0xFF, NULL);
    // WaitBlit();
}

void __inline rpage_video_blt_bmp_clip_mask(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *mask_bitmap, rect *clipping_rect)
{
    short clip_x = 0, clip_y = 0;
    
    if (x < clipping_rect->sx)
    {
        clip_x = clipping_rect->sx - x;
        x += clip_x;
        source_x += clip_x;
        width -= clip_x;
    }
    else
    {
        if (x + width > clipping_rect->ex)
            width -= (x + width - clipping_rect->ex);
    }

    if (width <= 0)
        return;
    
    if (y < clipping_rect->sy)
    {
        clip_y = clipping_rect->sy - y;
        y += clip_y;
        source_y += clip_y;
        height -= clip_y;
    }
    else
    {
        if (y + height > clipping_rect->ey)
            height -= (y + height - clipping_rect->ey);
    }
    
    if (height <= 0)
        return;

    BltMaskBitMapRastPort(source_bitmap, source_x, source_y, &(main_screen->screen->RastPort), x, y, width, height, (ABC|ABNC|ANBC), mask_bitmap->Planes[0]);
    // WaitBlit();
}

void __inline rpage_video_blt_bmp_clip_mask_bt(rpage_bitmap *source_bitmap, short source_x, short source_y, short width, short height, short x, short y, rpage_bitmap *mask_bitmap, rect *clipping_rect, UBYTE bit_mask)
{
    short clip_x = 0, clip_y = 0;
    UBYTE tmp_mask;
    
    if (x < clipping_rect->sx)
    {
        clip_x = clipping_rect->sx - x;
        x += clip_x;
        source_x += clip_x;
        width -= clip_x;
    }
    else
    {
        if (x + width > clipping_rect->ex)
            width -= (x + width - clipping_rect->ex);
    }

    if (width <= 0)
        return;
    
    if (y < clipping_rect->sy)
    {
        clip_y = clipping_rect->sy - y;
        y += clip_y;
        source_y += clip_y;
        height -= clip_y;
    }
    else
    {
        if (y + height > clipping_rect->ey)
            height -= (y + height - clipping_rect->ey);
    }
    
    if (height <= 0)
        return;

    tmp_mask = main_screen->screen->RastPort.Mask;
    main_screen->screen->RastPort.Mask = bit_mask;
    BltMaskBitMapRastPort(source_bitmap, source_x, source_y, &(main_screen->screen->RastPort), x, y, width, height, (ABC|ABNC|ANBC), mask_bitmap->Planes[0]);
    main_screen->screen->RastPort.Mask = tmp_mask;
    // WaitBlit();
}

void __inline rpage_video_set_palette(PALETTEPTR palette, short palette_size)
{
    set_palette(&(main_screen->screen->ViewPort), &palette, 0, palette_size - 1);
}

void rpage_video_draw_tileset(rpage_bitmap *tileset_bitmap, UBYTE *tileset, rect *tile_rect, short tileset_width)
{
    UBYTE x , y, tile_idx, prev_tile_idx = 0xFF;
    unsigned short y_w;
    UBYTE tile_x, tile_y,  tileset_bitmap_width = rpage_bitmap_get_width(tileset_bitmap) >> 3;

    for(y = (UBYTE)(tile_rect->sy); y < tile_rect->ey; y++)
    {
        y_w = y * tileset_width;
        for(x = (UBYTE)(tile_rect->sx); x < tile_rect->ex; x++)
        {
            tile_idx = tileset[x + y_w];
            if (tile_idx > 0)
            {
                if (prev_tile_idx != tile_idx) /* let's avoid 2 divide ops. if possible */ 
                {
                    tile_x = (tile_idx%tileset_bitmap_width) << 3;
                    tile_y = (tile_idx/tileset_bitmap_width) << 3;
                }
                rpage_video_blt_bmp(tileset_bitmap, tile_x, tile_y, 8, 8, x << 3, y << 3);
            }
            prev_tile_idx = tile_idx;
        }
    }
}

void rpage_bitmap_draw_tileset(rpage_bitmap *dest_bitmap, rpage_bitmap *tileset_bitmap, UBYTE *tileset, rect *tile_rect, short tileset_width)
{
    UBYTE x , y, tile_idx, prev_tile_idx = 0xFF;
    unsigned short y_w;
    UBYTE tile_x, tile_y,  tileset_bitmap_width = rpage_bitmap_get_width(tileset_bitmap) >> 3;

    for(y = (UBYTE)(tile_rect->sy); y < tile_rect->ey; y++)
    {
        y_w = y * tileset_width;
        for(x = (UBYTE)(tile_rect->sx); x < tile_rect->ex; x++)
        {
            tile_idx = tileset[x + y_w];
            if (tile_idx > 0)
            {
                if (prev_tile_idx != tile_idx) /* let's avoid 2 divide ops. if possible */ 
                {
                    tile_x = (tile_idx%tileset_bitmap_width) << 3;
                    tile_y = (tile_idx/tileset_bitmap_width) << 3;
                }
                rpage_bitmap_blit(tileset_bitmap, tile_x, tile_y, 8, 8, x << 3, y << 3, dest_bitmap);
            }
            prev_tile_idx = tile_idx;
        }
    }
}

void __inline rpage_fill_rect(rect *r, short color)
{
    if (color < 0)
        color = (1 << main_screen->screen->RastPort.BitMap->Depth) - 1;
    SetAPen(&(main_screen->screen->RastPort), color);
    RectFill( &(main_screen->screen->RastPort), r->sx, r->sy, r->ex, r->ey);
}

void __inline rpage_fill_rect_clip(rect *r, short color, rect *clipping_rect)
{
    rect video_rect;
    UBYTE tmp_mask;

    video_rect.sx = r->sx;
    video_rect.sy = r->sy;
    video_rect.ex = r->ex;
    video_rect.ey = r->ey;

    if (video_rect.sx < clipping_rect->sx)
        video_rect.sx = clipping_rect->sx;
    else
    {
        if (video_rect.ex > clipping_rect->ex)
            video_rect.ex = clipping_rect->ex;
    }

    if (video_rect.ex <= video_rect.sx)
        return;

    if (video_rect.sy < clipping_rect->sy)
        video_rect.sy = clipping_rect->sy;
    else
    {
        if (video_rect.ey > clipping_rect->ey)
            video_rect.ey = clipping_rect->ey;
    }
    
    if (video_rect.ey <= video_rect.sy)
        return;

    if (color < 0)
        color = (1 << main_screen->screen->RastPort.BitMap->Depth) - 1;
    SetAPen(&(main_screen->screen->RastPort), color);
    tmp_mask = main_screen->screen->RastPort.Mask;
    main_screen->screen->RastPort.Mask = 0x8;    
    RectFill( &(main_screen->screen->RastPort), video_rect.sx, video_rect.sy, video_rect.ex, video_rect.ey);
    main_screen->screen->RastPort.Mask = tmp_mask;
}

void rpage_video_draw_rect(rect *r, short color)
{
    rect video_rect;

    video_rect.sx = 0;
    video_rect.sy = 0;
    video_rect.ex = main_screen->screen->Width - 1;
    video_rect.ey = main_screen->screen->Height - 1;

    if (color < 0)
        color = (1 << main_screen->screen->RastPort.BitMap->Depth) - 1;
    SetAPen(&(main_screen->screen->RastPort), color);
    Move(&(main_screen->screen->RastPort), max(r->sx, video_rect.sx), max(r->sy, video_rect.sy));
    Draw(&(main_screen->screen->RastPort), min(r->ex, video_rect.ex), max(r->sy, video_rect.sy));
    Draw(&(main_screen->screen->RastPort), min(r->ex, video_rect.ex), min(r->ey, video_rect.ey));
    Draw(&(main_screen->screen->RastPort), max(r->sx, video_rect.sx), min(r->ey, video_rect.ey));
    Draw(&(main_screen->screen->RastPort), max(r->sx, video_rect.sx), max(r->sy, video_rect.sy));
}

void __inline rpage_video_set_pixel(short x, short y, short color)
{
    if (color < 0)
        color = (1 << main_screen->screen->RastPort.BitMap->Depth) - 1;    
    SetAPen(&(main_screen->screen->RastPort), color);
    WritePixel(&(main_screen->screen->RastPort), x, y);
}

void rpage_video_set_font(char *font_filename, short font_size)
{
    struct TextAttr ta;

    if (main_font != NULL)
        CloseFont(main_font);

    ta.ta_Name = font_filename;
    ta.ta_YSize = font_size;
    ta.ta_Flags = FPB_DISKFONT | FPF_DESIGNED;
    ta.ta_Style = FS_NORMAL;

    main_font = OpenDiskFont(&ta);
    if (main_font)
        SetFont(&(main_screen->screen->RastPort), main_font);
    else
    {
        printf("Cannot open font %s!", font_filename);
        main_font = NULL;
    }
}

void rpage_video_draw_text(char *str, short x, short y, short color)
{
    if (color < 0)
        color = (1 << main_screen->screen->RastPort.BitMap->Depth) - 1;

    if (x < 0)
        x = (main_screen->screen->Width - TextLength(&(main_screen->screen->RastPort), str, strlen(str))) >> 1;
    if (y < 0)
        y = (main_screen->screen->Height - 8) >> 1;

    SetAPen(&(main_screen->screen->RastPort), color);
    SetBPen(&(main_screen->screen->RastPort), 0);
    Move(&(main_screen->screen->RastPort), x, y + 8);
    SetDrMd(&(main_screen->screen->RastPort), 0);
    Text(&(main_screen->screen->RastPort), str, strlen(str));
}

void rpage_video_close(void)
{
#ifdef DEBUG_MACROS
    printf("rpage_video_close()\n");
#endif
    if (main_font)
    {
        CloseFont(main_font);
        main_font = NULL;
    }

    closeMainScreen(main_screen);
    main_screen = NULL;
}

/*
    BITMAP (direct bitmap access)
    -----------------------------
*/

ULONG rpage_bitmap_calculate_bytesize(short width, short height, short depth)
{
    return (RASSIZE(width, height) * depth);
}

rpage_bitmap *rpage_bitmap_new(short width, short height, short depth)
{
    return (rpage_bitmap *)allocate_new_bitmap(width, height, depth);
}

BOOL rpage_load_pak_into_bitmap(rpage_bitmap **bitmap, rpage_palette **palette, BYTE *packed_buffer, char *bitmap_filename)
{
    return load_pak_img_to_bitmap((struct BitMap **)bitmap, (UWORD **)palette, packed_buffer, bitmap_filename);
}


short rpage_bitmap_get_width(rpage_bitmap *bitmap)
{
    return(short)(bitmap->BytesPerRow << 3);
}

short rpage_bitmap_get_height(rpage_bitmap *bitmap)
{
    return(short)(bitmap->Rows);
}

BOOL rpage_load_pak_to_new_bitmap(rpage_bitmap **new_bitmap, rpage_palette **new_palette, BYTE *packed_buffer, char *bitmap_filename)
{
    return load_pak_img_to_new_bitmap((struct BitMap **)new_bitmap, (UWORD **)new_palette, packed_buffer, bitmap_filename);
}

rpage_bitmap *rpage_build_bitmap_mask(rpage_bitmap *source_bitmap)
{
    // TODO
    return NULL;
}

void rpage_bitmap_free(rpage_bitmap *bitmap)
{
    free_allocated_bitmap(bitmap);
}

/*
    INPUT (mouse, keyboard...)
    --------------------------
*/

BOOL rpage_input_init(void)
{
    if (main_screen->screen == NULL)
    {
        rpage_system_alert("Cannot init. input without a screen open !");
        return FALSE;
    }

    if (main_screen->window == NULL)
    {
        rpage_system_alert("Cannot init. input without a window open !");
        return FALSE;
    }

    input_window_init(main_screen->window);
}

void rpage_input_update(void)
{
    prev_input_mouse_button = input_mouse_button;
    prev_input_mouse_position = input_mouse_position;
    input_update(&input_mouse_button, &(input_mouse_position.x), &(input_mouse_position.y), &input_rawkey);
}

void rpage_mouse_button_flush(void)
{
    ActivateWindow(main_screen->window); /* FIXME : this patch helps the main window to get the focus after game init */
    input_mouse_button = 0;
}

short rpage_keyboard_rawkey(void)
{
    return input_rawkey;
}

BOOL rpage_mouse_button_left_is_down(void)
{
    if ((input_mouse_button & PLATFORM_MOUSE_LEFT_BUTTON))
        return TRUE;

    return FALSE;
}

BOOL rpage_mouse_button_right_is_down(void)
{
    if ((input_mouse_button & PLATFORM_MOUSE_RIGHT_BUTTON))
        return TRUE;

    return FALSE;
}

BOOL rpage_mouse_button_left_was_down(void)
{
    if ((input_mouse_button & PLATFORM_MOUSE_LEFT_BUTTON) && !(prev_input_mouse_button & PLATFORM_MOUSE_LEFT_BUTTON))
        return TRUE;

    return FALSE;
}

BOOL rpage_mouse_button_right_was_down(void)
{
    if ((input_mouse_button & PLATFORM_MOUSE_RIGHT_BUTTON) && !(prev_input_mouse_button & PLATFORM_MOUSE_RIGHT_BUTTON))
        return TRUE;

    return FALSE;
}

void rpage_mouse_get_prev_values(short *button, vec2 *mouse_coords)
{
    if (button) *button = prev_input_mouse_button;
    if (mouse_coords) *mouse_coords = prev_input_mouse_position;   
}

void rpage_mouse_get_values(short *button, vec2 *mouse_coords)
{
    // input_update(button, &(mouse_coords->x), &(mouse_coords->y));
    if (button) *button = input_mouse_button;
    if (mouse_coords) *mouse_coords = input_mouse_position; 
}

void rpage_mouse_show(void)
{
    if (main_screen != NULL && main_screen->screen->FirstWindow != NULL)
    {
#ifdef DEBUG_MACROS
        printf("rpage_mouse_show()\n");
#endif
        // ClearPointer(main_screen->FirstWindow);
        SetPointer(main_screen->screen->FirstWindow, pointer_normal_data, 16, 16, -1, -1);
    }
    else
        rpage_system_alert("Cannot set cursor state without a window ownership!");
}

void rpage_mouse_wait(void)
{
    if (main_screen != NULL && main_screen->screen->FirstWindow != NULL)
    {
#ifdef DEBUG_MACROS
        printf("rpage_mouse_show()\n");
#endif
        // ClearPointer(main_screen->FirstWindow);
        SetPointer(main_screen->screen->FirstWindow, wait_pointer, WAIT_POINTER_HEIGHT, 16, -1, -1);
    }
    else
        rpage_system_alert("Cannot set cursor state without a window ownership!");
}

void rpage_mouse_hide(void)
{
    if (main_screen != NULL && main_screen->screen->FirstWindow != NULL)
    {
#ifdef DEBUG_MACROS
        printf("rpage_mouse_hide()\n");
#endif
        SetPointer(main_screen->screen->FirstWindow, pointer_normal_data, 0, 0, 0, 0);
    }
    else
        rpage_system_alert("Cannot set cursor state without a window ownership!");
}

#endif