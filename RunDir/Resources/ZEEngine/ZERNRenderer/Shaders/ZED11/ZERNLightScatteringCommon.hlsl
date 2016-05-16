//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNLightScatteringCommon.hlsl
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

#ifndef __ZERN_LIGHTSCATTERING_COMMON_H__
#define __ZERN_LIGHTSCATTERING_COMMON_H__

#include "ZERNMath.hlsl"

#define EARTH_RADIUS			6360000.0f
#define ATMOSPHERE_HEIGHT		80000.0f
#define TOTAL_RADIUS			(EARTH_RADIUS + ATMOSPHERE_HEIGHT)
#define RAYLEIGH_MIE_HEIGHT 	float2(7994.0f, 1200.0f)
#define VIEW_NUM_STEPS			100.0f
#define EXTINCTION_NUM_STEPS	20.0f
#define LUT_DIMENSIONS			float4(32.0f, 128.0f, 64.0f, 16.0f)

static const float3 ZERNLightScatteringCommon_RayleighScatteringFactor	= float3(5.8e-6, 13.5e-6, 33.1e-6);
static const float3 ZERNLightScatteringCommon_MieScatteringFactor		= (float3)2.0e-5 / 0.9f;
static const float3 ZERNLightScatteringCommon_OzoneAbsorptionFactor		= float3(2.483825e-25f, 1.799535e-25f, 1.360172e-26f) * 2.69e+25f * 6.e-7f;

SamplerState		ZERNLightScatteringCommon_SamplerLinearClamp	: register(s0);

void ZERNLightScatteringCommon_CalculateWorldParamsFromTexCoords(in float4 TexCoord, out float Height, out float CosViewZenith, out float CosSunZenith, out float CosSunView)
{
	TexCoord.xzw = saturate((TexCoord * LUT_DIMENSIONS - 0.5f) / (LUT_DIMENSIONS - 1.0f)).xzw;
	
	TexCoord.x = pow(TexCoord.x, 2.0f);
	Height = TexCoord.x * (ATMOSPHERE_HEIGHT - 2.0f * 16.0f) + 16.0f;
	
	float Texel_Y_Dimension = LUT_DIMENSIONS.y;
	
	float CosHorizontal = -sqrt(Height * (2.0f * EARTH_RADIUS + Height)) / (EARTH_RADIUS + Height);
	if(TexCoord.y > 0.5f)
	{
		TexCoord.y = saturate((TexCoord.y - (0.5f + 0.5f / Texel_Y_Dimension )) * Texel_Y_Dimension / (Texel_Y_Dimension * 0.5f - 1.0f));
		CosViewZenith = max(CosHorizontal + pow(TexCoord.y, 5.0f) * (1.0f - CosHorizontal), CosHorizontal + 1.e-4f);
	}
	else
	{
		TexCoord.y = saturate((TexCoord.y - 0.5f / Texel_Y_Dimension) * Texel_Y_Dimension / (Texel_Y_Dimension * 0.5f - 1.0f));
		CosViewZenith = min(CosHorizontal - pow(TexCoord.y , 5.0f) * (1.0f + CosHorizontal), CosHorizontal - 1.e-4f);
	}
	
	CosSunZenith = tan((2.0f * TexCoord.z - 1.0f + 0.26f) * 1.1f) / tan(1.26f * 1.1f);
	
	TexCoord.w = sign(TexCoord.w - 0.5f) * pow(abs((TexCoord.w - 0.5f) * 2.0f), 2.0f / 3.0f) * 0.5f + 0.5f;
	CosSunView = cos(TexCoord.w * ZERNMath_PI);
	
	CosViewZenith = clamp(CosViewZenith, -1.0f, +1.0f);
	CosSunZenith = clamp(CosSunZenith, -1.0f, +1.0f);
	
	float D = (1.0f - CosViewZenith * CosViewZenith) * (1.0f - CosSunZenith * CosSunZenith);
	D = sqrt(max(D, 1e-20));
	
	float2 MinMaxCosSunView = CosViewZenith * CosSunZenith + float2(-D, +D);
	
	CosSunView = clamp(CosSunView, MinMaxCosSunView.x, MinMaxCosSunView.y);
}

float4 ZERNLightScatteringCommon_CalculateTexCoordsFromWorldParams(float Height, float CosViewZenith, float CosSunZenith, float CosSunView, in float PrevTexCoordY)
{	
	float4 TexCoord;
	
	Height = clamp(Height, 16.0f, ATMOSPHERE_HEIGHT - 16.0f);
	float Texel_X = saturate((Height - 16.0f) / (ATMOSPHERE_HEIGHT - 2.0f * 16.0f));
	Texel_X = pow(Texel_X, 0.5f);
	
	float Texel_Y_Dimension = LUT_DIMENSIONS.y;
	
	float CosHorizontal = -sqrt(Height * (2.0f * EARTH_RADIUS + Height)) / (EARTH_RADIUS + Height);
	float Texel_Y;
	if((PrevTexCoordY >= 0.5f || PrevTexCoordY < 0.0f) && CosViewZenith > CosHorizontal)
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
	float Texel_W = acos(CosSunView) / ZERNMath_PI;
	Texel_W = sign(Texel_W - 0.5f) * pow(abs((Texel_W - 0.5f) * 2.0f), 1.5f) * 0.5f + 0.5f;
	
	TexCoord = float4(Texel_X, Texel_Y, Texel_Z, Texel_W);
	TexCoord.xzw = ((TexCoord * (LUT_DIMENSIONS - 1.0f) + 0.5f) / LUT_DIMENSIONS).xzw;
	
	return TexCoord;
}

//X: Rayleigh Y: Mie
float2 ZERNLightScatteringCommon_CalculateDensity(float3 Position)
{
	float Height = length(Position - float3(0.0f, -EARTH_RADIUS, 0.0f)) - EARTH_RADIUS;
	
	return exp(-Height / RAYLEIGH_MIE_HEIGHT);
}

float2 ZERNLightScatteringCommon_IntegrateDensity(float3 Start, float3 End)
{
	float2 PrevRayleighMieDensity = ZERNLightScatteringCommon_CalculateDensity(Start);
	float2 TotalRayleighMieDensity = 0.0f;
	
	float3 StartToEnd = End - Start;
	float3 Direction = normalize(StartToEnd);
	float StepLength = length(StartToEnd) / EXTINCTION_NUM_STEPS;
	
	for(float S = 1.0f; S <= EXTINCTION_NUM_STEPS; S += 1.0f)
	{
		float3 Position = Start + Direction * S * StepLength;
		
		float2 CurRayleighMieDensity = ZERNLightScatteringCommon_CalculateDensity(Position);
		
		TotalRayleighMieDensity += (CurRayleighMieDensity + PrevRayleighMieDensity) * 0.5f * StepLength;
		
		PrevRayleighMieDensity = CurRayleighMieDensity;
	}
	
	return TotalRayleighMieDensity;
}

float3 ZERNLightScatteringCommon_CalculateExtinction(float3 Start, float3 End)
{
	float2 Density = ZERNLightScatteringCommon_IntegrateDensity(Start, End);
	
	return ZERNLightScatteringCommon_RayleighScatteringFactor * Density.x + ZERNLightScatteringCommon_MieScatteringFactor * Density.y + ZERNLightScatteringCommon_OzoneAbsorptionFactor * Density.x;
}

float3 ZERNLightScatteringCommon_LookupPrecomputedScattering(Texture3D<float3> ScatteringBuffer, float3 Position, float3 ViewDirection, float3 LightDirection, float3 EarthCenter, inout float PrevTexCoordY)
{
	float3 EarthCenterToPosition = Position - EarthCenter;
	float3 EarthCenterToPositionDirection = normalize(EarthCenterToPosition);
	float Height = length(EarthCenterToPosition) - EARTH_RADIUS;
	float CosViewZenith = dot(ViewDirection, EarthCenterToPositionDirection);
	float CosLightZenith = dot(LightDirection, EarthCenterToPositionDirection);
	float CosLightView = dot(LightDirection, ViewDirection);
	
	float4 TexCoord = ZERNLightScatteringCommon_CalculateTexCoordsFromWorldParams(Height, CosViewZenith, CosLightZenith, CosLightView, PrevTexCoordY);
	
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
	
	float3 Inscattering1 = ScatteringBuffer.SampleLevel(ZERNLightScatteringCommon_SamplerLinearClamp, TexCoord1, 0);
	float3 Inscattering2 = ScatteringBuffer.SampleLevel(ZERNLightScatteringCommon_SamplerLinearClamp, TexCoord2, 0);
	
	return lerp(Inscattering1, Inscattering2, Weight);
}

#endif
