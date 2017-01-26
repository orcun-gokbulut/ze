//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFiltering.hlsl
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

#ifndef __ZERN_FILTERING_H__
#define __ZERN_FILTERING_H__

#include "ZERNScreenCover.hlsl"
#include "ZERNMath.hlsl"
#include "ZERNGBuffer.hlsl"
#include "ZERNSamplers.hlsl"
#include "ZERNTransformations.hlsl"
#include "ZERNShading.hlsl"
#include "ZERNRenderer.hlsl"
#include "ZERNFilteringCommon.hlsl"

cbuffer ZERNFiltering_GaussianBlur_Constants						: register(b9)
{
	uint			ZERNFiltering_GaussianBlur_KernelRadius;
	float			ZERNFiltering_GaussianBlur_StandartDeviation;
	float2			ZERNFiltering_GaussianBlur_Reserved;
};

cbuffer ZERNFiltering_ColorTransform_Constants						: register(b9)
{
	float4x4		ZERNFiltering_ColorTransform_Matrix;
};

cbuffer ZERNFiltering_Noise_Constants								: register(b9)
{
	float			ZERNFiltering_Noise_Weight;
	uint			ZERNFiltering_Noise_Granularity;
	float2			ZERNFiltering_Noise_Size;
};

SamplerState		ZERNFiltering_Sampler							: register(s0);

Texture2D<float4>	ZERNFiltering_InputTexture						: register(t5);
Texture2D<float4>	ZERNFiltering_NoiseTexture						: register(t6);

RWTexture2D<float4>	ZERNFiltering_OutputTexture						: register(u0);

float4 ZERNFiltering_BlurHorizontal_PixelShader(float4 PositionViewport : SV_Position) : SV_Target0
{
	return ZERNFilteringCommon_GaussianBlur(ZERNFiltering_InputTexture, PositionViewport.xy, float2(1.0f, 0.0f), ZERNFiltering_GaussianBlur_KernelRadius, ZERNFiltering_GaussianBlur_StandartDeviation);
}

float4 ZERNFiltering_BlurVertical_PixelShader(float4 PositionViewport : SV_Position) : SV_Target0
{
	return ZERNFilteringCommon_GaussianBlur(ZERNFiltering_InputTexture, PositionViewport.xy, float2(0.0f, 1.0f), ZERNFiltering_GaussianBlur_KernelRadius, ZERNFiltering_GaussianBlur_StandartDeviation);
}

float4 ZERNFiltering_Scale_PixelShader(float4 PositionViewport : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	return ZERNFiltering_InputTexture.SampleLevel(ZERNFiltering_Sampler, TexCoord, 0.0f);
}

void ZERNFiltering_EdgeDetection_PixelShader(float4 PositionViewport : SV_Position)
{
	ZERNShading_Surface Surfaces[SAMPLE_COUNT];
	[unroll]
    for (uint I = 0; I < SAMPLE_COUNT; I++)
	{
		Surfaces[I].PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, ZERNGBuffer_GetDimensions(), ZERNGBuffer_GetDepth(PositionViewport.xy, I));
		Surfaces[I].NormalView = ZERNGBuffer_GetViewNormal(PositionViewport.xy, I);
	}
	
	bool EdgePixel = false;
	[unroll]
	for (uint J = 1; J < SAMPLE_COUNT; J++)
		EdgePixel = EdgePixel || ((abs(Surfaces[J].PositionView.z - Surfaces[0].PositionView.z) > 0.1f) || (dot(Surfaces[J].NormalView, Surfaces[0].NormalView) < 0.99f));
	
	if (!EdgePixel)
		discard;
}

float4 ZERNFiltering_ColorTransform_PixelShader(float4 PositionViewport : SV_Position) : SV_Target0
{
	float4 InputColor = ZERNFiltering_InputTexture[PositionViewport.xy];
	
	return mul(ZERNFiltering_ColorTransform_Matrix, InputColor);
}

[numthreads(32, 32, 1)]
void ZERNFiltering_GenerateNoise_ComputeShader(uint3 ThreadId : SV_DispatchThreadID)
{
	uint State = ThreadId.y * 1024 + ThreadId.x;
	State *= ZERNRenderer_FrameId;
	
	ZERNFiltering_OutputTexture[ThreadId.xy] = ZERNFilteringCommon_Random_LCG(ZERNFilteringCommon_Random_WangHash(State), 1664525, 1013904223) * (1.0f / 4294967296.0f);
}

float4 ZERNFiltering_Noise_PixelShader(float4 PositionViewport : SV_Position) : SV_Target0
{
	uint2 PixelCoord = floor(PositionViewport.xy);
	uint2 NoisePixelCoord = PixelCoord / ZERNFiltering_Noise_Granularity;
	
	uint State = NoisePixelCoord.y * ZERNFiltering_Noise_Size.x + NoisePixelCoord.x;
	State *= ZERNRenderer_FrameId;
	
	float Noise = ZERNFilteringCommon_Random_LCG(ZERNFilteringCommon_Random_WangHash(State), 1664525, 1013904223) * (1.0f / 4294967296.0f);
	float4 Color = ZERNFiltering_InputTexture[PixelCoord];
	
	return lerp(Color, (float4)Noise, ZERNFiltering_Noise_Weight);
}

#endif
