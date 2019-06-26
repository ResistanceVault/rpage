/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#include <stdio.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <intuition/intuition.h>
#include <hardware/custom.h>
#include <hardware/cia.h>
#include "frwk/amiga/input.h"

extern struct IntuitionBase *IntuitionBase;
extern struct Custom far custom;
struct Window *mouse_window = NULL;
struct IntuiMessage *window_message = NULL;

void input_window_init(struct Window *window)
{
  mouse_window = window;
}

void input_update(short *button, short *x, short *y, short *rawkey)
{
  unsigned long class;
  unsigned short code;
  unsigned short qualifier;
  unsigned short button_left = 0, button_right = 0;
  
  if (mouse_window != NULL)
  {
    Wait(1 << mouse_window->UserPort->mp_SigBit);

    while (window_message = (struct IntuiMessage *)GetMsg(mouse_window->UserPort))
    {
      class = window_message->Class; /* IDCMP flag. */
      code = window_message->Code;
      qualifier = window_message->Qualifier;

      *x = window_message->MouseX; /* X position of the mouse. */
      *y = window_message->MouseY; /* Y position of the mouse. */
      ReplyMsg((struct Message *)window_message);
    }
   
    switch(class)
    {
      case MOUSEBUTTONS:
        switch (code)
        {
          case SELECTDOWN:
            // printf("Left mouse button pressed.\n");
            button_left = PLATFORM_MOUSE_LEFT_BUTTON;
            break;
          case SELECTUP:
            // printf("Left mouse button released.\n");
            button_left = 0;
            break;
          case MENUDOWN:   /* Right button pressed. */
            // printf("Right mouse button pressed.\n");
            button_right = PLATFORM_MOUSE_RIGHT_BUTTON;
            break;
          case MENUUP:     /* Right button released. */
            // printf("Right mouse button released.\n");
            button_right = 0;
            break;
        }
        break;
      case RAWKEY:         /* The user pressed/released a key! */
        /* Print out the raw keycode (both as decimal and hex.): */
        printf("Raw keycode: %6d(d) %6x(h)\n", code, code );
        
        /* Print out the qualifier (both as decimal and hex.): */
        printf("Qualifier:   %6d(d) %6x(h)\n", qualifier, qualifier);
        
        /* This shows how you can check if a SHIFT or CTRL */
        /* qualifier key was also pressed:                 */
        if( qualifier &= IEQUALIFIER_LSHIFT )
          printf("Left SHIFT button pressed\n");

        if( qualifier &= IEQUALIFIER_RSHIFT )
          printf("Right SHIFT button pressed\n");
        
        if( qualifier &= IEQUALIFIER_CONTROL )
          printf("CTRL button pressed\n");

        printf("\n");
        break;
    }

    *button = button_left | button_right;
  }
  else
    printf("No window to get mouse from!\n");
}
#endif