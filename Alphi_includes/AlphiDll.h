//
// Copyright (c) 2020 Alphi Technology Corporation, Inc.  All Rights Reserved
//
// You are hereby granted a copyright license to use, modify and
// distribute this SOFTWARE so long as the entire notice is retained
// without alteration in any modified and/or redistributed versions,
// and that such modified versions are clearly identified as such.
// No licenses are granted by implication, estopple or otherwise under
// any patents or trademarks of Alphi Technology Corporation (Alphi).
//
// The SOFTWARE is provided on an "AS IS" basis and without warranty,
// to the maximum extent permitted by applicable law.
//
// ALPHI DISCLAIMS ALL WARRANTIES WHETHER EXPRESS OR IMPLIED, INCLUDING
// WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
// AND ANY WARRANTY AGAINST INFRINGEMENT WITH REGARD TO THE SOFTWARE
// (INCLUDING ANY MODIFIED VERSIONS THEREOF) AND ANY ACCOMPANYING
// WRITTEN MATERIAL.
//
// To the maximum extent permitted by applicable law, IN NO EVENT SHALL
// ALPHI BE LIABLE FOR ANY DAMAGE WHATSOEVER (INCLUDING WITHOUT LIMITATION,
// DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
// BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM THE USE
// OR INABILITY TO USE THE SOFTWARE.  GMS assumes no responsibility for
// for the maintenance or support of the SOFTWARE
//
/** @file AlphiDll.h
* @brief Utility DLL definitions
*/

// Maintenance Log
//---------------------------------------------------------------------
//---------------------------------------------------------------------
#ifndef _ALPHIDLL_H
#define _ALPHIDLL_H

#include "time.h"

#ifdef WIN32
	#include <Windows.h>

	/** @brief Sleep for a number of microseconds
	 @param usec Number of microseconds to sleep
	 */
	static inline void usleep(__int64 usec)
	{
		HANDLE timer;
		LARGE_INTEGER ft;

		ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

		timer = CreateWaitableTimer(NULL, TRUE, NULL);
		if (timer == NULL) return;
		SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
		WaitForSingleObject(timer, INFINITE);
		CloseHandle(timer);
	}

	static inline void nanosleep(struct timespec* t, void* na)
	{
		usleep(t->tv_nsec / 1000);
	}

	/** @brief get system time with a nanosecond definition */
	inline int clock_gettime(int, struct timespec* spec)      //C-file part
	{
		__int64 wintime;
		GetSystemTimeAsFileTime((FILETIME*)&wintime);
		wintime -= 116444736000000000i64;  //1jan1601 to 1jan1970
		spec->tv_sec = (long)(wintime / 10000000i64);           //seconds
		spec->tv_nsec = wintime % 10000000i64 * 100;      //nano-seconds
		return 0;
	}


#else
// LINUX specific
	#define LINUX
	#define UNIX

#include <stdint.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>

#define FALSE 0
#define TRUE 1
	typedef int HRESULT;

	inline uint64_t GetTickCount64()
	{
	    struct timespec ts;

	    clock_gettime( CLOCK_REALTIME, &ts);
	    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
	}

	inline void QueryPerformanceCounter(double *t)
	{
	    struct timespec ts;

	    clock_gettime( CLOCK_REALTIME, &ts);

	    *t = ts.tv_sec * 1000 + (double)ts.tv_nsec / 1000000;
	}

	inline void QueryPerformanceFrequency(double *f)
	{
		*f = 1;
	}

	inline void Sleep(int ms)
	{
		usleep(ms*1000);
	}

    char getche();
	char _getch();
#define kbhit() _kbhit()
	bool _kbhit();


	// end linux specific
#endif

#define ALPHI_S_OK	0
#define WD_DEVICE_NOT_FOUND ENODEV

#ifndef __cplusplus
// ----------------------------------------------------
// Compile like a standard C header.

#	ifdef COMPILE_DLL
#		define Dll    __declspec( dllexport )
#	else
#		define Dll    __declspec( dllimport )
#	endif

// ----------------------------------------------------
#else // Compile like a C++ header.

#	ifdef WIN32
#		ifdef _WINDLL
#			define Dll    extern "C" __declspec( dllexport )
#			define DLL    __declspec( dllexport )
#		else
#			define Dll    extern "C" __declspec( dllimport )
#			define DLL    __declspec( dllimport )
#		endif
#	else
#		define Dll
#		define DLL
#	endif
#endif

#ifdef WINDOWS
struct timespec { 
	long tv_sec; 
	long tv_nsec; 
};    //header part
#endif
#endif



