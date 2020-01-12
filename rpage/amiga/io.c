/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <exec/types.h>
#include <libraries/dos.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <intuition/intuition.h>
#include <hardware/custom.h>
#include <devices/trackdisk.h>
#include <hardware/cia.h>
#include "rpage/amiga/io.h"

extern struct IntuitionBase *IntuitionBase;
extern struct Custom far custom;
struct Window *mouse_window = NULL;
struct IntuiMessage *window_message = NULL;
static char str_diskname[256];

int file_get_size(char *filename)
{
  struct FileLock *lock;
  struct FileInfoBlock *fib_ptr;
  int filesize = -1;

  memset(str_diskname, 0, 255);
  fib_ptr = (struct FileInfoBlock *)AllocMem( sizeof( struct FileInfoBlock ), MEMF_PUBLIC | MEMF_CLEAR );
  lock = (struct FileLock *)Lock(filename, SHARED_LOCK);

  if(lock != NULL)
  {
    if(Examine((BPTR)lock, fib_ptr) != NULL)
      filesize = fib_ptr->fib_Size;

    UnLock((BPTR)lock);  
  }

  FreeMem(fib_ptr, sizeof(struct FileInfoBlock));

  return filesize;
}

char *disk_get_logical_name(char *device_physical_name)
{
  short disk_idx;
  BOOL disk_is_present = FALSE;
  struct MsgPort *TrackMP;         /* Pointer for message port */
  struct IOExtTD *TrackIO;         /* Pointer for IORequest */
  struct FileLock *lock;
  struct FileInfoBlock *fib_ptr;

#ifdef DEBUG_MACROS
  printf("disk_get_logical_name(%s), ", device_physical_name);
#endif

  if (strncmp(device_physical_name, "df0:", 4) == 0)
    disk_idx = 0;
  else if (strncmp(device_physical_name, "df1:", 4) == 0)
    disk_idx = 1;
  else if (strncmp(device_physical_name, "df2:", 4) == 0)
    disk_idx = 2;
  else if (strncmp(device_physical_name, "df3:", 4) == 0)
    disk_idx = 3;    
  else
  {
    printf("unknown disk physical name!\n");
    return NULL;
  }

  // is there a disk present ?
  if (TrackMP=CreatePort(0,0) )
    if (TrackIO=(struct IOExtTD *)CreateExtIO(TrackMP, sizeof(struct IOExtTD)))
    {
      if (!OpenDevice(TD_NAME, (long)disk_idx,(struct IORequest *)TrackIO, 0))
      {
        TrackIO->iotd_Req.io_Flags = IOF_QUICK;
        TrackIO->iotd_Req.io_Command = TD_CHANGESTATE;
        BeginIO((struct IORequest *)TrackIO);
        WaitIO((struct IORequest *)TrackIO);
        // printf("df%d:%d/%d\n", disk_idx, TrackIO->iotd_Req.io_Error, TrackIO->iotd_Req.io_Actual);
        if (!(TrackIO->iotd_Req.io_Error))
        {
          if (!(TrackIO->iotd_Req.io_Actual))
            disk_is_present = TRUE;
        }
        else
          printf("BeginIO() error!\n");
      }
#ifdef DEBUG_MACROS
      else
        printf("%s did not open\n", TD_NAME);
#endif
    }

  // if a disk is found, get his name
  if(disk_is_present)
  {
    memset(str_diskname, 0, 255);
    fib_ptr = (struct FileInfoBlock *)AllocMem( sizeof( struct FileInfoBlock ), MEMF_PUBLIC | MEMF_CLEAR );
    lock = (struct FileLock *)Lock(device_physical_name, SHARED_LOCK);

    if(lock != NULL)
    {
      if(Examine((BPTR)lock, fib_ptr) != NULL)
        strcpy(str_diskname, fib_ptr->fib_FileName);
      
      UnLock((BPTR)lock);  
    }

    FreeMem(fib_ptr, sizeof(struct FileInfoBlock));

    strcat(str_diskname, ":");
#ifdef DEBUG_MACROS
    printf("%s\n", str_diskname);
#endif
    return str_diskname;
  }

#ifdef DEBUG_MACROS
  printf("no disk in drive!\n");
#endif
  return NULL; // device_physical_name;
}

void input_window_init(struct Window *window)
{
  mouse_window = window;
}

void input_update(short *button, short *x, short *y, unsigned short *rawkey)
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

    *rawkey = 0x0;
   
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
        *rawkey = code;
        /* Print out the raw keycode (both as decimal and hex.): */
        // printf("Raw keycode: %6d(d) %6x(h)\n", code, code );
        
        // /* Print out the qualifier (both as decimal and hex.): */
        // printf("Qualifier:   %6d(d) %6x(h)\n", qualifier, qualifier);
        
        // /* This shows how you can check if a SHIFT or CTRL */
        // /* qualifier key was also pressed:                 */
        // if( qualifier &= IEQUALIFIER_LSHIFT )
        //   printf("Left SHIFT button pressed\n");

        // if( qualifier &= IEQUALIFIER_RSHIFT )
        //   printf("Right SHIFT button pressed\n");
        
        // if( qualifier &= IEQUALIFIER_CONTROL )
        //   printf("CTRL button pressed\n");

        // printf("\n");
        break;
    }

    *button = button_left | button_right;
  }
  else
    printf("No window to get mouse from!\n");
}
#endif