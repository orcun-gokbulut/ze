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

cbuffer ZERNFiltering_Constants									: register(b9)
{
	uint			ZERNFiltering_KernelRadius;
	float3			ZERNFiltering_Reserved;
};

Texture2D			ZERNFiltering_InputTexture					: register(t5);

static const float	ZERNFiltering_StandartDeviationSquare = 2.0f * 2.0f;

float ZERNFiltering_GetGaussianWeight(float SampleOffset)
{
	float Temp = 1.0f / sqrt(2.0f * ZERNMath_PI * ZERNFiltering_StandartDeviationSquare);
    return Temp * exp(-(SampleOffset * SampleOffset) / (2.0f * ZERNFiltering_StandartDeviationSquare));
}

float4 ZERNFiltering_ApplyBlur(float2 PositionViewport, float2 SampleDirection)
{
	float4 ResultColor = ZERNFiltering_InputTexture[PositionViewport] * ZERNFiltering_GetGaussianWeight(0.0f);
	
	for (uint I = 1; I <= ZERNFiltering_KernelRadius; I++)
		ResultColor += (ZERNFiltering_InputTexture[PositionViewport.xy - I * SampleDirection] + ZERNFiltering_InputTexture[PositionViewport.xy + I * SampleDirection]) * ZERNFiltering_GetGaussianWeight(I);
	
	return ResultColor;
}

float4 ZERNFiltering_BlurHorizontal_PixelShader(float4 PositionViewport : SV_Position) : SV_Target0
{
	return ZERNFiltering_ApplyBlur(PositionViewport.xy, float2(1.0f, 0.0f));
}

float4 ZERNFiltering_BlurVertical_PixelShader(float4 PositionViewport : SV_Position) : SV_Target0
{
	return ZERNFiltering_ApplyBlur(PositionViewport.xy, float2(0.0f, 1.0f));
}

float4 ZERNFiltering_Scale_PixelShader(float4 PositionViewport : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	return ZERNFiltering_InputTexture.SampleLevel(ZERNSampler_LinearClamp, TexCoord, 0.0f);
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

#endif
