/*  Resistance's Portable-Adventure-Game-Engine (R-PAGE), Copyright (C) 2019 François Gutherz, Resistance.no
    Released under MIT License, see license.txt for details.
*/

#ifdef LATTICE
#ifndef _TIME_ROUTINES_
#define _TIME_ROUTINES_

#include <exec/types.h>
#include <exec/ports.h>
#include <devices/timer.h>

BOOL init_timer_device(void);
void timer_device_get_system_time(struct timeval *);
void uninit_timer_device(void);

#endif
#endif
