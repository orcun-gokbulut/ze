//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFastLightScattering.hlsl
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

#ifndef __ZERN_FASTLIGHTSCATTERING_HLSL__
#define __ZERN_FASTLIGHTSCATTERING_HLSL__

#include "ZERNScreenCover.hlsl"
#include "ZERNGBuffer.hlsl"
#include "ZERNTransformations.hlsl"

#define PI					3.14159265359f
#define EARTH_RADIUS		6360000.0f
#define ATMOSPHERE_HEIGHT	80000.0f
#define TOTAL_RADIUS		(EARTH_RADIUS + ATMOSPHERE_HEIGHT)
#define RAYLEIGH_MIE_HEIGHT float2(7994.0f, 1200.0f)
#define EXT_NUM_STEPS		100.0f
#define LUT_DIMENSIONS		float4(32.0f, 128.0f, 64.0f, 16.0f)

cbuffer ZERNFastLightScattering_Constants								: register(b8)
{
	float3			ZERNFastLightScattering_LightDirection;
	float			ZERNFastLightScattering_Intensity;
	float3			ZERNFastLightScattering_LightColor;
	float			ZERNFastLightScattering_MieScatteringStrength;
};

SamplerState		ZERNFastLightScattering_SamplerLinearClamp			: register(s0);

Texture3D<float3>	ZERNFastLightScattering_MultipleScatteringBuffer	: register(t5);
Texture2D<float3>	ZERNFastLightScattering_ExtinctionBuffer			: register(t6);

static const float3 ZERNFastLightScattering_RayleighScatteringFactor	= float3(5.8e-6f, 13.5e-6f, 33.1e-6f);
static const float3 ZERNFastLightScattering_MieScatteringFactor			= float3(2.0e-5f, 2.0e-5f, 2.0e-5f);
static const float3 ZERNFastLightScattering_BackgroundColor				= float3(0.1f, 0.1f, 0.1f);

void ZERNFastLightScattering_IntersectionRaySphere(float3 RayOrigin, float3 RayDirection, float3 SphereCenter, float2 SphereRadius, out float4 StartEndDistance)
{
	float3 RayOrigin_SphereCenter = RayOrigin - SphereCenter;
	float2 A = dot(RayDirection, RayDirection);
	float2 B = 2.0f * dot(RayDirection, RayOrigin_SphereCenter);
	float2 C = dot(RayOrigin_SphereCenter, RayOrigin_SphereCenter) - SphereRadius * SphereRadius;
	
	float2 Discriminant = B * B - 4.0f * A * C;
	float2 SquareRootDiscriminant = sqrt(Discriminant);
	if(Discriminant.x >= 0.0f)
	{
		StartEndDistance.x = (-B.x - SquareRootDiscriminant.x) / 2.0f * A.x;
		StartEndDistance.y = (-B.x + SquareRootDiscriminant.x) / 2.0f * A.x;
	}
	if(Discriminant.y >= 0.0f)
	{
		StartEndDistance.z = (-B.y - SquareRootDiscriminant.y) / 2.0f * A.y;
		StartEndDistance.w = (-B.y + SquareRootDiscriminant.y) / 2.0f * A.y;
	}
}

float4 ZERNFastLightScattering_CalculateTexCoordsFromWorldParams(float Height, float CosViewZenith, float CosSunZenith, float CosSunView, in float PrevTexCoordY)
{	
	float4 TexCoord;
	
	Height = clamp(Height, 16.0f, ATMOSPHERE_HEIGHT - 16.0f);
	float Texel_X = saturate((Height - 16.0f) / (ATMOSPHERE_HEIGHT - 2.0f * 16.0f));
	Texel_X = pow(Texel_X, 0.5f);
	
	float Texel_Y_Dimension = LUT_DIMENSIONS.y;
	
	float CosHorizontal = -sqrt(Height * (2.0f * EARTH_RADIUS + Height)) / (EARTH_RADIUS + Height);
	float Texel_Y;
	if((PrevTexCoordY >= 0.5f || !(PrevTexCoordY >= 0.0f && PrevTexCoordY < 0.5f)) && CosViewZenith > CosHorizontal)
	{
		Texel_Y = pow(saturate((CosViewZenith - CosHorizontal) / (1.0f - CosHorizontal)), 0.2f);
		Texel_Y = 0.5f + 0.5f / Texel_Y_Dimension + Texel_Y * (Texel_Y_Dimension * 0.5f - 1.0f) / Texel_Y_Dimension;
	}
	else
	{
		Texel_Y = pow(saturate((CosHorizontal - CosViewZenith) / (1.0f + CosHorizontal)), 0.2f);
		Texel_Y = 0.5f / Texel_Y_Dimension + Texel_Y * (Texel_Y_Dimension * 0.5f - 1.0f) / Texel_Y_Dimension;
	}
	
	float Texel_Z = 0.5f * (atan(max(CosSunZenith, -0.1975) * tan(1.26f * 1.1f)) / 1.1f + (1.0f - 0.26f));
	
	CosSunView = clamp(CosSunView, -1.0f, +1.0f);
	float Texel_W = acos(CosSunView) / PI;
	Texel_W = sign(Texel_W - 0.5f) * pow(abs((Texel_W - 0.5f) * 2.0f), 1.5f) * 0.5f + 0.5f;
	
	TexCoord = float4(Texel_X, Texel_Y, Texel_Z, Texel_W);
	TexCoord.xzw = ((TexCoord * (LUT_DIMENSIONS - 1.0f) + 0.5f) / LUT_DIMENSIONS).xzw;
	
	return TexCoord;
}

//X: Rayleigh Y: Mie
float2 ZERNFastLightScattering_CalculateDensity(float3 Position)
{
	float Height = length(Position - float3(0.0f, -EARTH_RADIUS, 0.0f)) - EARTH_RADIUS;
	
	return exp(-(float2(Height, Height) / RAYLEIGH_MIE_HEIGHT));
}

float3 ZERNFastLightScattering_CalculateExtinction(float3 Start, float3 End)
{
	float2 PrevRayleighMieDensity = ZERNFastLightScattering_CalculateDensity(Start);
	float2 TotalRayleighMieDensity = 0.0f;
	
	float3 StartToEnd = End - Start;
	float3 Direction = normalize(StartToEnd);
	float StepLength = length(StartToEnd) / EXT_NUM_STEPS;
	
	for(float S = 1.0f; S <= EXT_NUM_STEPS; S += 1.0f)
	{
		float3 Position = Start + Direction * S * StepLength;
		
		float2 CurRayleighMieDensity = ZERNFastLightScattering_CalculateDensity(Position);
		
		TotalRayleighMieDensity += ((CurRayleighMieDensity + PrevRayleighMieDensity) * 0.5f) * StepLength;
		
		PrevRayleighMieDensity = CurRayleighMieDensity;
	}
	
	return (ZERNFastLightScattering_RayleighScatteringFactor * TotalRayleighMieDensity.x + ZERNFastLightScattering_MieScatteringFactor * TotalRayleighMieDensity.y);
}

float3 ZERNFastLightScattering_LookupPrecomputedScattering(float3 Position, float3 ViewDirection, float3 LightDirection, float3 EarthCenter, inout float PrevTexCoordY)
{
	float3 EarthCenterToPosition = Position - EarthCenter;
	float3 EarthCenterToPositionDirection = normalize(EarthCenterToPosition);
	float Height = length(EarthCenterToPosition) - EARTH_RADIUS;
	float CosViewZenith = dot(ViewDirection, EarthCenterToPositionDirection);
	float CosSunZenith = dot(LightDirection, EarthCenterToPositionDirection);
	float CosSunView = dot(ViewDirection, LightDirection);
	
	float4 TexCoord = ZERNFastLightScattering_CalculateTexCoordsFromWorldParams(Height, CosViewZenith, CosSunZenith, CosSunView, PrevTexCoordY);
	
	PrevTexCoordY = TexCoord.y;
	
	float3 TexCoord1;
	TexCoord1.xy = TexCoord.xy;
	float Slice = floor(TexCoord.w * LUT_DIMENSIONS.w - 0.5f);
	Slice = clamp(Slice, 0.0f, LUT_DIMENSIONS.w - 1.0f);
	float Weight = ( TexCoord.w * LUT_DIMENSIONS.w - 0.5f) - Slice;
	Weight = max(0.0f, Weight);
	float2 SliceMinMax = float2(Slice, Slice + 1.0f) / LUT_DIMENSIONS.w + float2(0.5f, -0.5f) / (LUT_DIMENSIONS.z * LUT_DIMENSIONS.w);
	TexCoord1.z = (Slice + TexCoord.z) / LUT_DIMENSIONS.w;
	TexCoord1.z = clamp(TexCoord1.z, SliceMinMax.x, SliceMinMax.y);
	
	float Slice1 = min(Slice + 1.0f, LUT_DIMENSIONS.w - 1.0f);
	float NextSliceOffset = (Slice1 - Slice) / LUT_DIMENSIONS.w;
	float3 TexCoord2 = TexCoord1 + float3(0.0f, 0.0f, NextSliceOffset);
	
	float3 Inscattering1 = ZERNFastLightScattering_MultipleScatteringBuffer.SampleLevel(ZERNFastLightScattering_SamplerLinearClamp, TexCoord1, 0);
	float3 Inscattering2 = ZERNFastLightScattering_MultipleScatteringBuffer.SampleLevel(ZERNFastLightScattering_SamplerLinearClamp, TexCoord2, 0);
	
	return lerp(Inscattering1, Inscattering2, Weight);
}

float3 ZERNFastLightScattering_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	
	float DepthHomogeneous = ZERNGBuffer_GetDepth(PositionViewport.xy);
	float DepthView = ZERNTransformations_HomogeneousToViewDepth(DepthHomogeneous);
	
	float2 TextureDimensions = ZERNGBuffer_GetDimensions();
	
	float2 PixelPositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, TextureDimensions);
	float3 ViewDirection = ZERNTransformations_ViewToWorld(float4(PixelPositionView, 1.0f, 0.0f));
	ViewDirection = normalize(ViewDirection);
	
	float3 LightDirection = -normalize(ZERNFastLightScattering_LightDirection);
	
	float4 StartEndDistance = (float4)0.0f;
	ZERNFastLightScattering_IntersectionRaySphere(ZERNView_Position, ViewDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
	
	float3 RayStart = ZERNView_Position + ViewDirection * max(0.0f, StartEndDistance.x);
		
	float RayLength = StartEndDistance.y;
	
	if(StartEndDistance.x < 0.0f && DepthView < (ZERNView_FarZ - 1.0f))	//in earth
		RayLength = DepthView;
	else if(StartEndDistance.x > 0.0f && StartEndDistance.z > 0.0f)		//in space
		RayLength = StartEndDistance.z;	
	
	float3 RayEnd = ZERNView_Position + ViewDirection * RayLength;
	
	float3 Extinction = exp(-ZERNFastLightScattering_CalculateExtinction(RayStart, RayEnd));
	
	float PrevTexCoordY = -1.0f;
	float3 Inscattering = ZERNFastLightScattering_LookupPrecomputedScattering(RayStart, ViewDirection, LightDirection, EarthCenter, PrevTexCoordY);
	Inscattering -= Extinction * ZERNFastLightScattering_LookupPrecomputedScattering(RayEnd, ViewDirection, LightDirection, EarthCenter, PrevTexCoordY);
	Inscattering *= ZERNFastLightScattering_Intensity;
	
	float3 PixelColor = ZERNGBuffer_GetAccumulationColor(PositionViewport.xy);
	
	if(DepthView > (ZERNView_FarZ - 1.0f))
		PixelColor = ZERNFastLightScattering_BackgroundColor * ZERNFastLightScattering_LightColor;
	
	return PixelColor * Extinction + Inscattering;
}

#endif
