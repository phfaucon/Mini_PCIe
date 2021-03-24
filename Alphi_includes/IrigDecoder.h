#pragma once

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
/** @file IrigDecoder.h
 * @brief Irig Decoder class to get time
 */
// Maintenance Log
//---------------------------------------------------------------------
// v1.0		9/23/2020	phf	Written
//---------------------------------------------------------------------
#include "AlphiDll.h"
#include <stdint.h>
#include <ctime>

class DLL IrigDecoder
{
public:
	struct IrigDate
	{
		int tm_sec;
		int tm_min;
		int tm_hour;
		int tm_yday;
		int tm_year;
	};

	static const int time_offset = 2;
	static const int day_offset = 3;
	static const int second_offset = 4;

	inline IrigDecoder(volatile void* addr)
	{
		base = (volatile uint32_t*)addr;
	}

	inline uint32_t getTimeRaw()
	{
		return base[time_offset];
	}

	//    irig_b_time_d 	     <= {2'b0 ,h_10 ,h_1 ,1'b0 ,m_10 ,m_1 ,1'b0, s_10, s_1, 4'b0, s_p1};  // hh:mm:ss:0t
	inline void getTime(IrigDate* ttm)
	{
		uint32_t t = getTimeRaw();
//		uint8_t s_p1 = t & 0xf;
		t = t >> 8;
		ttm->tm_sec = (t & 0xf) + ((t >> 4) & 0x07) * 10;
		t = t >> 8;
		ttm->tm_min = (t & 0xf) + ((t >> 4) & 0x07) * 10;
		t = t >> 8;
		ttm->tm_hour = (t & 0xf) + ((t >> 4) & 0x03) * 10;
	}

	inline uint32_t getDayRaw()
	{
		return base[day_offset];
	}

	//    irig_b_day_d <= {8'b0 ,y_10 ,y_1 ,2'b0, d_100, d_10, d_1};  // 00:yy:0ddd
	inline void getDay(IrigDate* ttm)
	{
		uint32_t t = getDayRaw();
		ttm->tm_yday = t & 0xf;
		t = t >> 4;
		ttm->tm_yday += t & 0xf * 10;
		t = t >> 4;
		ttm->tm_yday += t & 0x3 * 10;
		t = t >> 4;
		ttm->tm_year = t & 0xf;
		t = t >> 4;
		ttm->tm_year = t & 0xf * 10;
	}

	inline uint32_t getSecond()
	{
		return base[second_offset];
	}

	/*	assign avl_readdata = { 32{avl_addr == 3'b010}} & irig_b_time
	//							   | {32{avl_addr == 3'b011}} & irig_b_day
	//							   | {32{avl_addr == 3'b100}} & irig_b_sec

	reg [3:0]  s_1  ;
	reg [2:0]  s_10 ;
	reg [3:0]  m_1  ;
	reg [2:0]  m_10 ;
	reg [3:0]  h_1  ;
	reg [1:0]  h_10 ;
	reg [3:0]  d_1  ;
	reg [3:0]  d_10 ;
	reg [1:0]  d_100;
	reg [3:0]  s_p1 ;
	reg [3:0]  y_1  ;
	reg [3:0]  y_10 ;
	reg [17:0] unus ;
	reg [15:0] sec  ;

		//    irig_b_time_d 	     <= {2'b0 ,h_10 ,h_1 ,1'b0 ,m_10 ,m_1 ,1'b0, s_10, s_1, 4'b0, s_p1};  // hh:mm:ss:0t
		//    irig_b_sec_d <= {16'b0, sec};  // 0000:ssss

	tm_sec	int	seconds after the minute	0-60*
	tm_min	int	minutes after the hour	0-59
	tm_hour	int	hours since midnight	0-23
	tm_mday	int	day of the month	1-31
	tm_mon	int	months since January	0-11
	tm_year	int	years since 1900
	tm_wday	int	days since Sunday	0-6
	tm_yday	int	days since January 1	0-365
	tm_isdst	int	Daylight Saving Time flag

		*/

	inline void getIrigDate(struct tm* t)
	{
		IrigDate d = { 0 };
		getTime(&d);
		getDay(&d);

		char s[100] = "2014-224-17:20:00";

		sprintf(s, "20%02d-%d-%d:%d:%d",
			d.tm_year, d.tm_yday, d.tm_hour, d.tm_min, d.tm_sec);

		strftime(s, strlen(s), "%Y-%j-%H:%M:%S", t);

	}

private:
	volatile uint32_t* base;

};
