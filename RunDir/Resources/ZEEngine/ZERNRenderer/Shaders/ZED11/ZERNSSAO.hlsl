//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSSAO.hlsl
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

#ifndef __ZERN_SSAO_HLSL__
#define __ZERN_SSAO_HLSL__

#include "ZERNScreenCover.hlsl"
#include "ZERNTransformations.hlsl"
#include "ZERNGBuffer.hlsl"

Texture2D<float3>	ZERNSSAO_RandomVectorsTexture	: register(t5);
Texture2D<float3>	ZERNSSAO_OcclusionTexture		: register(t6);

cbuffer ZERNSSAO_Constants							: register(b8)
{
	float4			ZERNSSAO_SphereSamples[32];
	uint			ZERNSSAO_SampleCount;
	float3			ZERNSSAO_Reserved0;
	float			ZERNSSAO_OcclusionRadius;
	float			ZERNSSAO_MinDepthBias;
	float			ZERNSSAO_Intensity;
	float			ZERNSSAO_DownScale;
};

cbuffer ZERNSSAO_FilterConstants					: register(b9)
{
	float4			ZERNSSAO_KernelValues[32];
	uint			ZERNSSAO_KernelSize;
	float3			ZERNSSAO_Reserved1;
};

SamplerState 		ZERNSSAO_SamplerPointWrap		: register(s0);
SamplerState 		ZERNSSAO_SamplerPointClamp		: register(s1);
SamplerState 		ZERNSSAO_SamplerLinearClamp		: register(s2);

float3 ZERNSSAO_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{	
	float2 OutputDimensions = ZERNGBuffer_GetDimensions() / ZERNSSAO_DownScale;
	float2 TexCoord = ZERNTransformations_ViewportToTexelCenter(PositionViewport.xy, OutputDimensions);
	float3 PositionView = ZERNTransformations_TexelToView(TexCoord, ZERNGBuffer_GetDepth(ZERNSSAO_SamplerPointClamp, TexCoord));
	
	if(PositionView.z < 100.0f)
	{
		float3 RandomVector = ZERNSSAO_RandomVectorsTexture.Sample(ZERNSSAO_SamplerPointWrap, 4.0f * TexCoord) * 2.0f - 1.0f;
		RandomVector = normalize(RandomVector);
		
		float3 NormalView = ZERNGBuffer_GetViewNormal(ZERNSSAO_SamplerPointClamp, TexCoord);
		NormalView = normalize(NormalView);
		float3 NormalBiasedPositionView = PositionView + NormalView * ZERNSSAO_MinDepthBias;
		
		float TotalOcclusion = 0.0f;
		for(uint I = 0; I < ZERNSSAO_SampleCount; I++)
		{
			float3 SampleOffsetVector = reflect(ZERNSSAO_SphereSamples[I].xyz, RandomVector);
			float Flip = sign(dot(SampleOffsetVector, NormalView));
			
			float3 SamplePositionView = PositionView + Flip * SampleOffsetVector * ZERNSSAO_OcclusionRadius;
			float2 SamplePositionTexelCenter = ZERNTransformations_ViewToTexelCenter(SamplePositionView, OutputDimensions);
			
			float OccluderDepthHomogeneous = ZERNGBuffer_GetDepth(ZERNSSAO_SamplerPointClamp, SamplePositionTexelCenter);
			float OccluderDepthView = ZERNTransformations_HomogeneousToViewDepth(OccluderDepthHomogeneous);
			float3 OccluderPositionView = SamplePositionView * (OccluderDepthView / SamplePositionView.z);
			
			float Angle = max(0.0f, dot(NormalView, normalize(OccluderPositionView - NormalBiasedPositionView)));
			
			float DistanceEuclidian = length(OccluderPositionView - NormalBiasedPositionView);
			TotalOcclusion += Angle * saturate((ZERNSSAO_OcclusionRadius - DistanceEuclidian) / ZERNSSAO_OcclusionRadius);
		}
		
		TotalOcclusion /= ZERNSSAO_SampleCount;
		
		float Accessilibity = saturate(1.0f - TotalOcclusion);
		
		return saturate(pow(Accessilibity, ZERNSSAO_Intensity));
	}
	else
	{
		return (float3)1.0f;
	}
}

float3 ZERNSSAO_Blend_PixelShader(float4 PositionViewport : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	return ZERNSSAO_OcclusionTexture.Sample(ZERNSSAO_SamplerLinearClamp, TexCoord);
}

float3 ZERNSSAO_Filter_PixelShader(float4 PositionViewport : SV_Position) : SV_Target0
{
	float2 TextureSize;
	ZERNSSAO_OcclusionTexture.GetDimensions(TextureSize.x, TextureSize.y);
	float2 TexelOffset = 1.0f / TextureSize;
	float2 TexCoord = PositionViewport.xy * TexelOffset;
	
	float Depth = ZERNGBuffer_GetDepth(ZERNSSAO_SamplerPointClamp, TexCoord);
	float3 Normal = ZERNGBuffer_GetViewNormal(ZERNSSAO_SamplerPointClamp, TexCoord);
	
	float3 ResultColor = 0.0f;
	float TotalWeight = 0.0f;
	for(uint I = 0; I < ZERNSSAO_KernelSize; I++)
	{
		float2 NeighborTexCoord = TexCoord + ZERNSSAO_KernelValues[I].xy * TexelOffset;
		
		float SampleDepth = ZERNGBuffer_GetDepth(ZERNSSAO_SamplerPointClamp, NeighborTexCoord);
		float3 SampleNormal = ZERNGBuffer_GetViewNormal(ZERNSSAO_SamplerPointClamp, NeighborTexCoord);
		
		if(abs(Depth - SampleDepth) <= 0.2f && dot(Normal, SampleNormal) >= 0.8f)
		{
			float3 SampleColor = ZERNSSAO_OcclusionTexture.SampleLevel(ZERNSSAO_SamplerPointClamp, NeighborTexCoord, 0);
			ResultColor += SampleColor * ZERNSSAO_KernelValues[I].w;
			TotalWeight += ZERNSSAO_KernelValues[I].w;
		}
	}
	
	return saturate(ResultColor) / TotalWeight;
}

#endif
