//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNBlurCompute.hlsl
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

#ifndef __ZERN_BLUR_COMPUTE_H__
#define __ZERN_BLUR_COMPUTE_H__

#include "ZERNMath.hlsl"

cbuffer ZERNBlurCompute_Constants					: register(b9)
{
	//float	ZERNBlurCompute_KernelWeights[63];
	uint	ZERNBlurCompute_KernelRadius;
	float	ZERNBlurCompute_StandartDeviation;
	float2	ZERNBlurCompute_Reserved;
};

#define GROUP_DIM_X					320
#define KERNEL_RADIUS				30
#define EXTRA_SAMPLE_COUNT			KERNEL_RADIUS + KERNEL_RADIUS
#define SHARED_MEM_SIZE				GROUP_DIM_X + EXTRA_SAMPLE_COUNT
#define SAMPLE_COUNT_PER_THREAD		SHARED_MEM_SIZE / GROUP_DIM_X;

Texture2D<float3>	InputTexture	: register(t5);
RWTexture2D<float3>	OutputTexture	: register(u0);

groupshared float3	SharedTexels[SHARED_MEM_SIZE];

float ZERNBlurCompute_GetGaussianWeight(float SampleOffset, float Deviation)
{
	float Temp = 1.0f / sqrt(2.0f * ZERNMath_PI * Deviation * Deviation);
    return Temp * exp(-(SampleOffset * SampleOffset) / (2.0f * Deviation * Deviation));
}

[numthreads(GROUP_DIM_X, 1, 1)]
void ZERNBlurCompute_BlurHorizontal_ComputeShader_Main(uint3 GroupId : SV_GroupID, uint3 GroupThreadId : SV_GroupThreadID)
{
	uint GroupX = GroupId.x * GROUP_DIM_X;
	
	uint StartX = GroupX - KERNEL_RADIUS;
	uint EndX = GroupX + GROUP_DIM_X + KERNEL_RADIUS;
	
	SharedTexels[GroupThreadId.x] = InputTexture[uint2(StartX + GroupThreadId.x, GroupId.y)];
	
	if (GroupThreadId.x < EXTRA_SAMPLE_COUNT)
		SharedTexels[GroupThreadId.x + GROUP_DIM_X] = InputTexture[uint2(StartX + GroupThreadId.x + GROUP_DIM_X, GroupId.y)];
	
	GroupMemoryBarrierWithGroupSync();
	
	float3 ResultColor = SharedTexels[GroupThreadId.x + KERNEL_RADIUS] * ZERNBlurCompute_GetGaussianWeight(0.0f, 2.0f);
	for (uint I = 1; I <= KERNEL_RADIUS; I++)
		ResultColor += (SharedTexels[GroupThreadId.x + KERNEL_RADIUS - I] + SharedTexels[GroupThreadId.x + KERNEL_RADIUS + I]) * ZERNBlurCompute_GetGaussianWeight(I, 2.0f);
	
	OutputTexture[uint2(GroupX + GroupThreadId.x, GroupId.y)] = ResultColor;
}

[numthreads(GROUP_DIM_X, 1, 1)]
void ZERNBlurCompute_BlurVertical_ComputeShader_Main(uint3 GroupId : SV_GroupID, uint3 GroupThreadId : SV_GroupThreadID)
{
	uint GroupX = GroupId.x * GROUP_DIM_X;
	
	uint StartX = GroupX - KERNEL_RADIUS;
	uint EndX = GroupX + GROUP_DIM_X + KERNEL_RADIUS;
	
	SharedTexels[GroupThreadId.x] = InputTexture[uint2(GroupId.y, StartX + GroupThreadId.x)];
	
	if (GroupThreadId.x < EXTRA_SAMPLE_COUNT)
		SharedTexels[GroupThreadId.x + GROUP_DIM_X] = InputTexture[uint2(GroupId.y, StartX + GroupThreadId.x + GROUP_DIM_X)];
	
	GroupMemoryBarrierWithGroupSync();
	
	float3 ResultColor = SharedTexels[GroupThreadId.x + KERNEL_RADIUS] * ZERNBlurCompute_GetGaussianWeight(0.0f, 2.0f);
	for (uint I = 1; I <= KERNEL_RADIUS; I++)
		ResultColor += (SharedTexels[GroupThreadId.x + KERNEL_RADIUS - I] + SharedTexels[GroupThreadId.x + KERNEL_RADIUS + I]) * ZERNBlurCompute_GetGaussianWeight(I, 2.0f);
	
	OutputTexture[uint2(GroupId.y, GroupX + GroupThreadId.x)] = ResultColor;
}

#endif
