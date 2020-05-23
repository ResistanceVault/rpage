/*  Athanor 2, Original game by Eric "Atlantis" Safar, (C) Safargames 2019  
    Amiga version by Francois "Astrofra" Gutherz.
*/

#include <exec/types.h>
#include "rpage/utils.h"

#ifndef _POINTER_DATA_
#define _POINTER_DATA_

#define MOUSE_CURSOR_POINT 0
#define MOUSE_CURSOR_WAIT 1
#define MOUSE_CURSOR_READ 2

#define SYSTEM_CURSOR_HEIGHT 16L
extern vec2 system_cursors_hotspot[3];
extern UWORD chip system_cursors_img[3][36];

// /* Legacy (== vintage) AmigaOS 1.3 cursor */
// extern USHORT chip pointer_normal_data[36];

// /* Wait cursor */
// extern USHORT chip wait_pointer[(WAIT_POINTER_HEIGHT * 2) + 4];

/* Custom cursors */
extern vec2 game_cursors_hotspot[4];
extern UWORD chip game_cursors_img[4][36];

#endif