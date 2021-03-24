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
/** @file Ltc2664_av.h
 * @brief Synchro simulator class definitions.
 */

 // Maintenance Log
//---------------------------------------------------------------------
// v1.0		7/23/2020	phf	Written
//---------------------------------------------------------------------#pragma once
#include "AlphiDll.h"
#include <stdint.h>
#define _USE_MATH_DEFINES
#include "math.h"

/** @brief Synchro simulator
* 
* This class allows controlling the on board simulator. It consists of 4 DAs, generating sinewaves at the proper frequency and amplitude to
* simulate a synchro output.
*/
class DLL Ltc2664_av
{
public:
	Ltc2664_av(volatile void* addr);

	void setSimGain(int chanNbr, int16_t val);
	int16_t getSimGain(int chanNbr);

	void setAngle(double angle);
	double angleToRadian(uint32_t intVal);
	uint32_t radianToAngle(double rad);
	double getFrequency(void);
	void setFrequency(double freq);

	inline void setAngularVelocity(double rad)
	{
		*velocityReg = radianToAngle(rad);
	}

	inline double getAngularVelocity(void)
	{
		return angleToRadian(*velocityReg);
	}

	inline uint32_t getAngularVelocityRaw(void)
	{
		return *velocityReg;
	}


private:
	static const int gain1_index = 0;
	static const int gain2_index = 2;
	static const int gain3_index = 4;
	static const int velocity_index = 4;

	volatile uint32_t* velocityReg;
	volatile int16_t* gain;

	inline static double angVel2Freq()
	{
		return (M_PI * 2)/1000000;  ///< Ratio between angular speed and frequency, set by hardware.
	}

};

