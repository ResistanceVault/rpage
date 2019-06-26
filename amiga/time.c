/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>
#include <exec/ports.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <devices/timer.h>

#include "rpage/amiga/debug.h"

// #include <proto/all.h>

struct Library *TimerBase = NULL;
struct MsgPort *timer_port = NULL;
struct timerequest *timer_io = NULL;
BYTE time_r_error = 0;

BOOL init_timer_device(void)
{
#ifdef DEBUG_MACROS
  printf("init_timer_device()\n");
#endif
  timer_port = CreatePort(NULL, 0);
  if (!timer_port)
  {
    printf("could not create message port\n");
    return FALSE;
  }
  timer_io = (struct timerequest *)CreateExtIO(timer_port,
                                               sizeof(struct timerequest));
  if (!timer_io)
  {
    printf("could not create create timerequest object\n");
    DeletePort(timer_port);
    return FALSE;
  }
  time_r_error = OpenDevice(TIMERNAME, UNIT_MICROHZ,
                            (struct IORequest *)timer_io, 0);
  if (time_r_error)
    printf("could not open timer.device\n");
  else
  {
    ULONG secs, mins, hours, days;

    TimerBase = (struct Library *)timer_io->tr_node.io_Device;
  }
}

void timer_device_get_system_time(struct timeval *time_val)
{
  timer_io->tr_node.io_Command = TR_GETSYSTIME;
  DoIO((struct IORequest *) timer_io);
  time_val->tv_secs = timer_io->tr_time.tv_secs;
  time_val->tv_micro = timer_io->tr_time.tv_micro;
}

void uninit_timer_device(void)
{
#ifdef DEBUG_MACROS
  printf("uninit_timer_device()\n");
#endif
  if (!CheckIO((struct IORequest *)timer_io))
    AbortIO((struct IORequest *)timer_io);

  WaitIO((struct IORequest *)timer_io);
  TimerBase = NULL;

  /* cleanup */
  CloseDevice((struct IORequest *)timer_io);
  if (timer_io)
    DeleteExtIO((struct IORequest *)timer_io);
  if (timer_port)
    DeletePort(timer_port);
}

#endif