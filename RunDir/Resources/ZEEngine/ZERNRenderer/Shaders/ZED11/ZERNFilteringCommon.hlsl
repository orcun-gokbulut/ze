//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFilteringCommon.hlsl
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#ifndef __ZERN_FILTERING_COMMON_H__
#define __ZERN_FILTERING_COMMON_H__

#include "ZERNMath.hlsl"

float ZERNFilteringCommon_GetGaussianWeight(float SampleOffset, float StandartDeviation)
{
	float Temp = 1.0f / sqrt(2.0f * ZERNMath_PI * StandartDeviation * StandartDeviation);
    return Temp * exp(-(SampleOffset * SampleOffset) / (2.0f * StandartDeviation * StandartDeviation));
}

float4 ZERNFilteringCommon_GaussianBlur(Texture2D<float4> Texture, uint2 PixelCoord, uint2 SampleDirection, uint KernelRadius, float StandartDeviation)
{
	float4 ResultColor = Texture[PixelCoord] * ZERNFilteringCommon_GetGaussianWeight(0.0f, StandartDeviation);
	
	for (uint I = 1; I <= KernelRadius; I++)
		ResultColor += (Texture[PixelCoord - I * SampleDirection] + Texture[PixelCoord + I * SampleDirection]) * ZERNFilteringCommon_GetGaussianWeight(I, StandartDeviation);
	
	return ResultColor;
}

uint ZERNFilteringCommon_Random_WangHash(uint Seed)
{
	Seed = (Seed ^ 61) ^ (Seed >> 16);
	Seed *= 9;
	Seed = Seed ^ (Seed >> 4);
	Seed *= 0x27d4eb2d;
	Seed = Seed ^ (Seed >> 15);
	
	return Seed;
}

uint ZERNFilteringCommon_Random_LCG(uint State, uint C1, uint C2)
{
	return C1 * State + C2;
}

uint ZERNFilteringCommon_Random_Taus(uint State, int C1, int C2, int C3, uint M)
{
	uint Temp = (((State << C1) ^ State) >> C2);
	return ((State & M) << C3) ^ Temp;
}

uint ZERNFilteringCommon_Random_TausLCG(uint4 State)
{
	return (
	ZERNFilteringCommon_Random_Taus(State.x, 13, 19, 12, 4294967294) ^ 
	ZERNFilteringCommon_Random_Taus(State.y, 2, 25, 4, 4294967294) ^ 
	ZERNFilteringCommon_Random_Taus(State.z, 3, 11, 17, 4294967294) ^ 
	ZERNFilteringCommon_Random_LCG(State.w, 1664525, 1013904223)
	);
}

#endif
