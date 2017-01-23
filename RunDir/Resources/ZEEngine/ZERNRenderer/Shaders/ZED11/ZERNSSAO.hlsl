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
#include "ZERNSamplers.hlsl"

struct DeinterleaveDepthOutput
{
	float Depth00 : SV_Target0;
	float Depth10 : SV_Target1;
	float Depth20 : SV_Target2;
	float Depth30 : SV_Target3;
	
	float Depth01 : SV_Target4;
	float Depth11 : SV_Target5;
	float Depth21 : SV_Target6;
	float Depth31 : SV_Target7;
};

struct ResolveDepthOutput
{
	#ifdef DEINTERLEAVED
		float DepthView : SV_Target0;
	#else
		float DepthHomogeneous : SV_Depth;
	#endif
};

cbuffer ZERNSSAO_Constants								: register(b8)
{
	float4				ZERNSSAO_SphereSamples[32];
	uint				ZERNSSAO_SampleCount;
	float				ZERNSSAO_OcclusionRadius;
	float				ZERNSSAO_NormalBias;
	float				ZERNSSAO_Intensity;
		
	float2				ZERNSSAO_WidthHeight;
	float2				ZERNSSAO_InvWidthHeight;
		
	uint				ZERNSSAO_KernelRadius;
	float				ZERNSSAO_BlurSharpness;
	float				ZERNSSAO_DistanceThreshold;
	float				ZERNSSAO_Reserved0;
};

cbuffer ZERNSSAO_DeinterleavedConstants					: register(b9)
{
	float3				ZERNSSAO_RandomVector;
	uint				ZERNSSAO_DepthArrayIndex;
	
	float2				ZERNSSAO_Offset;
	float2				ZERNSSAO_Reserved1;
};

Texture2D<float2>		ZERNSSAO_InputTexture			: register(t5);
Texture2D<float3>		ZERNSSAO_RandomVectorsTexture	: register(t6);
Texture2D<float>		ZERNSSAO_DepthTexture			: register(t7);

Texture2DArray<float>	ZERNSSAO_DeinterleavedDepth		: register(t8);
Texture2DArray<float2>	ZERNSSAO_DeinterleavedOcclusion	: register(t9);

float ZERNSSAO_GetCrossBilateralWeight(float SampleOffset, float SampleDepth, float CenterDepth)
{
    const float BlurSigma = (float)ZERNSSAO_KernelRadius * 0.5f;
    const float BlurFalloff = 1.0f / (2.0f * BlurSigma * BlurSigma);

    float DeltaZ = (SampleDepth - CenterDepth) * ZERNSSAO_BlurSharpness;

    return exp2(-SampleOffset * SampleOffset * BlurFalloff - DeltaZ * DeltaZ);
}

float2 ZERNSSAO_ApplyCrossBilateralBlur(float2 TexCoord, float2 TexelOffset)
{
	float2 AO_Z = ZERNSSAO_InputTexture.SampleLevel(ZERNSampler_PointClamp, TexCoord, 0.0f).xy;
	float TotalAO = AO_Z.x;
	float CenterDepth = AO_Z.y;
	float TotalWeight = 1.0f;
	
	for (uint I = 1; I <= ZERNSSAO_KernelRadius; I++)
	{
		float2 SampleTexCoord = TexCoord - I * TexelOffset;
		float2 SampleAO_Z = ZERNSSAO_InputTexture.SampleLevel(ZERNSampler_PointClamp, SampleTexCoord, 0.0f).xy;
		
		float Weight = ZERNSSAO_GetCrossBilateralWeight(I, SampleAO_Z.y, CenterDepth);
		TotalAO += SampleAO_Z.x * Weight;
		TotalWeight += Weight;
	}
	
	for (uint J = 1; J <= ZERNSSAO_KernelRadius; J++)
	{
		float2 SampleTexCoord = TexCoord + J * TexelOffset;
		float2 SampleAO_Z = ZERNSSAO_InputTexture.SampleLevel(ZERNSampler_PointClamp, SampleTexCoord, 0.0f).xy;
		
		float Weight = ZERNSSAO_GetCrossBilateralWeight(J, SampleAO_Z.y, CenterDepth);
		TotalAO += SampleAO_Z.x * Weight;
		TotalWeight += Weight;
	}
	
	return float2(TotalAO / TotalWeight, CenterDepth);
}

void ZERNSSAO_SSAO_VertexShader_Main(in uint VertexIndex : SV_VertexId, out float4 Position : SV_Position, out float2 Texcoords : TEXCOORD0)
{
	Position = ZERNScreenCover_GetPosition(VertexIndex);
	Texcoords = ZERNScreenCover_GetTexcoords(Position);
	
	Position.z = ZERNTransformations_ViewToProjectionZ(ZERNSSAO_DistanceThreshold);
}

float2 ZERNSSAO_SSAO_PixelShader_Main(float4 PositionViewport : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	#ifdef DEINTERLEAVED
		float DepthView = ZERNSSAO_DeinterleavedDepth[int3(PositionViewport.xy, ZERNSSAO_DepthArrayIndex)];
		if (DepthView > ZERNSSAO_DistanceThreshold)
			return float2(1.0f, DepthView);
	
		float3 PositionView = ZERNTransformations_TexelToView2(TexCoord, DepthView);
		float2 PositionFullResViewport = floor(PositionViewport.xy) * 4.0f + ZERNSSAO_Offset;
		float3 NormalView = ZERNGBuffer_GetViewNormal(PositionFullResViewport.xy);
		float3 RandomVector = ZERNSSAO_RandomVector;
	#else
		float3 PositionView = ZERNTransformations_TexelToView(TexCoord, ZERNSSAO_DepthTexture[PositionViewport.xy]);
		float3 NormalView = ZERNGBuffer_GetViewNormal(PositionViewport.xy);
		float3 RandomVector = ZERNSSAO_RandomVectorsTexture.Sample(ZERNSampler_PointWrap, PositionViewport.xy / 4.0f);
	#endif
	
	float3 NormalBiasedPositionView = PositionView + NormalView * ZERNSSAO_NormalBias;
	
	float TotalOcclusion = 0.0f;
	for (uint I = 0; I < ZERNSSAO_SampleCount; I++)
	{
		float3 RandomOrientedSampleVector = reflect(ZERNSSAO_SphereSamples[I].xyz, RandomVector);
		float Flip = sign(dot(RandomOrientedSampleVector, NormalView));
		
		float3 SamplePositionView = PositionView + Flip * RandomOrientedSampleVector * ZERNSSAO_OcclusionRadius;
		
		#ifdef DEINTERLEAVED
			float2 SampleTexCoord = ZERNTransformations_ViewToTexelCorner(SamplePositionView);
			float OccluderDepthView = ZERNSSAO_DeinterleavedDepth.SampleLevel(ZERNSampler_PointClamp, float3(SampleTexCoord, ZERNSSAO_DepthArrayIndex), 0.0f);
		#else
			float2 SamplePositionViewport = ZERNTransformations_ViewToViewport(SamplePositionView, ZERNSSAO_WidthHeight);
			float OccluderDepthView = ZERNTransformations_HomogeneousToViewDepth(ZERNSSAO_DepthTexture[SamplePositionViewport]);
		#endif
		
		float3 OccluderPositionView = SamplePositionView * (OccluderDepthView / SamplePositionView.z);
		
		float3 VectorToOccluder = OccluderPositionView - NormalBiasedPositionView;
		float DistanceToOccluder = length(VectorToOccluder);
		
		float Angle = max(0.0f, dot(NormalView, (VectorToOccluder / DistanceToOccluder)));
		TotalOcclusion += Angle * exp(-DistanceToOccluder);
	}
	
	TotalOcclusion /= ZERNSSAO_SampleCount;
	
	return float2(pow(saturate(1.0f - TotalOcclusion), ZERNSSAO_Intensity), PositionView.z);
}

float2 ZERNSSAO_CrossBilateralBlurX_PixelShader(float4 PositionViewport : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	return ZERNSSAO_ApplyCrossBilateralBlur(TexCoord, float2(ZERNSSAO_InvWidthHeight.x, 0.0f));
}

float3 ZERNSSAO_CrossBilateralBlurY_PixelShader(float4 PositionViewport : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	return ZERNSSAO_ApplyCrossBilateralBlur(TexCoord, float2(0.0f, ZERNSSAO_InvWidthHeight.y)).rrr;
}

ResolveDepthOutput ZERNSSAO_ResolveDepth_PixelShader(float4 PositionViewport : SV_Position, float2 TexCoord : TEXCOORD0)
{
	float DepthHomogeneous = ZERNGBuffer_GetDepth(PositionViewport.xy, 0);
	
	ResolveDepthOutput Output;
	#ifdef DEINTERLEAVED
		Output.DepthView = ZERNTransformations_HomogeneousToViewDepth(DepthHomogeneous);
	#else
		Output.DepthHomogeneous = DepthHomogeneous;
	#endif
	
	return Output;
}

DeinterleaveDepthOutput ZERNSSAO_DeinterleaveDepth_PixelShader(float4 PositionViewport : SV_Position)
{
	float2 PositionFullViewport = floor(PositionViewport.xy) * 4.0f + ZERNSSAO_Offset;
	float2 TexCoord = PositionFullViewport * ZERNSSAO_InvWidthHeight;
	
	 // Gather sample ordering: (-,+),(+,+),(+,-),(-,-)
	float4 Samples0 = ZERNSSAO_DepthTexture.GatherRed(ZERNSampler_LinearClamp, TexCoord);
	float4 Samples1 = ZERNSSAO_DepthTexture.GatherRed(ZERNSampler_LinearClamp, TexCoord, int2(2, 0));
	
	DeinterleaveDepthOutput Output;
	Output.Depth00 = Samples0.w;
	Output.Depth10 = Samples0.z;
	Output.Depth20 = Samples1.w;
	Output.Depth30 = Samples1.z;
	
	Output.Depth01 = Samples0.x;
	Output.Depth11 = Samples0.y;
	Output.Depth21 = Samples1.x;
	Output.Depth31 = Samples1.y;
	
	return Output;
}

float2 ZERNSSAO_ReinterleaveSSAO_PixelShader(float4 PositionViewport : SV_Position) : SV_Target0
{
	uint2 PositionFullResViewport = PositionViewport.xy;
	uint2 Index = PositionFullResViewport & 3;
	uint ArrayIndex = Index.y * 4 + Index.x;
	uint2 PositionQuarterResViewport = PositionFullResViewport >> 2;
	
	return ZERNSSAO_DeinterleavedOcclusion[int3(PositionQuarterResViewport, ArrayIndex)];
}

#endif
