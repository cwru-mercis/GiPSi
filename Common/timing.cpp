/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Timing Routines Implementation (timing.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	TIMING.CPP v0.0
////
////	Timing routines
////
////////////////////////////////////////////////////////////////



#include "timing.h"
#include <stdio.h>

#ifdef WIN32
#include <windows.h> 

static _int64 start_time[MAX_TIMER];
static _int64 stop_time[MAX_TIMER];
static _int64 freq = -1;

void start_timer(int id)
{
	QueryPerformanceCounter((LARGE_INTEGER *)&start_time[id]);
}

double get_timer(int id)
{
	QueryPerformanceCounter((LARGE_INTEGER *)&stop_time[id]);
	return (double) ((stop_time[id] - start_time[id])*1000.0/freq);
}

void init_timers(void)
{

#ifdef TIMING_DEBUG
	printf("Initializing timers:\t using QueryPerfomanceCounter()\n");
#endif

	if(freq == -1)
		QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
}

#else

#include <sys/time.h>
#include <cstdlib>

static long long int start_time[MAX_TIMER];
static long long int stop_time[MAX_TIMER];
static long long int freq = -1;

void start_timer(int id)
{
  static timeval t;

  gettimeofday(&t, NULL);
  start_time[id] = (long long int) (t.tv_sec * 1e6 + t.tv_usec);
}

double get_timer(int id)
{
  static timeval t;

  gettimeofday(&t, NULL);
  return (double) (((t.tv_sec * 1e6 + t.tv_usec)-(start_time[id])) * 1e-3);
}

void init_timers(void){
#ifdef TIMING_DEBUG
	printf("sizeof(long long int) = %d\n", sizeof(long long int));
	printf("Initializing timers:\t using gettimeofday()\n");
#endif
}

#endif

double fhz(double t) 
{
    if(t == 0.0L) t = -1.0L;
    return (double) (1000.0L / t);
}


