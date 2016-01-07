//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNPrecomputingLightScattering.hlsl
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

#ifndef __ZERN_PRECOMPUTINGLIGHTSCATTERING_H__
#define __ZERN_PRECOMPUTINGLIGHTSCATTERING_H__

#define PI						3.14159265359f
#define EARTH_RADIUS			6360000.0f
#define ATMOSPHERE_HEIGHT		80000.0f
#define TOTAL_RADIUS			(EARTH_RADIUS + ATMOSPHERE_HEIGHT)
#define RAYLEIGH_MIE_HEIGHT 	float2(7994.0f, 1200.0f)
#define VIEW_NUM_STEPS			100.0f
#define EXTINCTION_NUM_STEPS	100.0f
#define FLT_MAX					3.402823466e+38f
#define LUT_DIMENSIONS			float4(32.0f, 128.0f, 64.0f, 16.0f)

#include "ZERNTransformations.hlsl"

cbuffer ZERNPrecomputingLightScattering_Constants									: register(b8)
{
	float			ZERNPrecomputingLightScattering_Index;
	float			ZERNPrecomputingLightScattering_Index2;
	float2			ZERNPrecomputingLightScattering_Reserved;
};

SamplerState		ZERNPrecomputingLightScattering_SamplerLinearClamp				: register(s0);
SamplerState		ZERNPrecomputingLightScattering_SamplerPointWrap				: register(s1);

Texture3D<float3>	ZERNPrecomputingLightScattering_PreviousInScatteringBuffer		: register(t0);
Texture3D<float3>	ZERNPrecomputingLightScattering_HighOrderScatteringBuffer		: register(t1);
Texture2D<float3>	ZERNPrecomputingLightScattering_ExtinctionBuffer				: register(t2);
Texture2D<float3>	ZERNPrecomputingLightScattering_RandomVectorsTexture			: register(t3);

static const float	MieScatteringStrength 		= 0.76f;	//[-1, 1]
static const float3 RayleighScatteringFactor	= float3(5.8e-6, 13.5e-6, 33.1e-6);
static const float3 MieScatteringFactor			= float3(2.0e-5, 2.0e-5, 2.0e-5);

void ZERNPrecomputingLightScattering_IntersectionRaySphere(float3 RayOrigin, float3 RayDirection, float3 SphereCenter, float2 SphereRadius, out float4 StartEndDistance)
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

void ZERNPrecomputingLightScattering_MultiplyWithPhases(in float3 SunDirection, in float3 ViewDirection, inout float3 RayleighInScattering, inout float3 MieInScattering)
{
	float CosAngle = dot(SunDirection, ViewDirection);
	float PhaseRayleigh = (3.0f / (16.0f * PI)) * (1.0f + CosAngle * CosAngle);
	float MieScatteringStrenghSquare = MieScatteringStrength * MieScatteringStrength;
	float PhaseMie = (3.0f / (8.0f * PI)) * ((1.0f - MieScatteringStrenghSquare) * (1.0f + CosAngle * CosAngle)) / 
					((2.0f + MieScatteringStrenghSquare) * pow(abs(1.0f + MieScatteringStrenghSquare - 2.0f * MieScatteringStrength * CosAngle), 1.5f));
					
	RayleighInScattering *= PhaseRayleigh;
	MieInScattering *= PhaseMie;
}

void ZERNPrecomputingLightScattering_CalculateWorldParamsFromTexCoords(in float4 TexCoord, out float Height, out float CosViewZenith, out float CosSunZenith, out float CosSunView)
{
	TexCoord.xzw = saturate((TexCoord * LUT_DIMENSIONS - 0.5f) / (LUT_DIMENSIONS - 1.0f)).xzw;
	
	TexCoord.x = pow(TexCoord.x, 2.0f);
	Height = TexCoord.x * (ATMOSPHERE_HEIGHT - 2.0f * 16.0f) + 16.0f;
	
	float Texel_Y_Dimension = LUT_DIMENSIONS.y;
	
	float CosHorizontal = -sqrt(Height * (2.0f * EARTH_RADIUS + Height)) / (EARTH_RADIUS + Height);
	if(TexCoord.y > 0.5f)
	{
		TexCoord.y = (TexCoord.y - (0.5f + 0.5f / Texel_Y_Dimension )) * Texel_Y_Dimension / (Texel_Y_Dimension * 0.5f - 1.0f);
		CosViewZenith = max(CosHorizontal + pow(TexCoord.y, 5.0f) * (1.0f - CosHorizontal), CosHorizontal + 0.0001f);
	}
	else
	{
		TexCoord.y = (TexCoord.y - 0.5f / Texel_Y_Dimension) * Texel_Y_Dimension / (Texel_Y_Dimension * 0.5f - 1.0f);
		CosViewZenith = min(CosHorizontal - pow(TexCoord.y , 5.0f) * (1.0f + CosHorizontal), CosHorizontal - 0.0001f);
	}
	
	CosSunZenith = tan((2.0f * TexCoord.z - 1.0f + 0.26f) * 1.1f) / tan(1.26f * 1.1f);
	
	TexCoord.w = sign(TexCoord.w - 0.5f) * pow(abs((TexCoord.w - 0.5f) * 2.0f), 2.0f / 3.0f) * 0.5f + 0.5f;
	CosSunView = cos(TexCoord.w * PI);
	
	CosViewZenith = clamp(CosViewZenith, -1.0f, +1.0f);
	CosSunZenith = clamp(CosSunZenith, -1.0f, +1.0f);
	
	float D = (1.0f - CosViewZenith * CosViewZenith) * (1.0f - CosSunZenith * CosSunZenith);
	D = sqrt(max(D, 1e-20));
	
	float2 MinMaxCosSunView = CosViewZenith * CosSunZenith + float2(-D, +D);
	
	CosSunView = clamp(CosSunView, MinMaxCosSunView.x, MinMaxCosSunView.y);
}

float4 ZERNPrecomputingLightScattering_CalculateTexCoordsFromWorldParams(float Height, float CosViewZenith, float CosSunZenith, float CosSunView, in float PrevTexCoordY)
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
float2 ZERNPrecomputingLightScattering_CalculateDensity(float3 Position)
{
	float Height = length(Position - float3(0.0f, -EARTH_RADIUS, 0.0f)) - EARTH_RADIUS;
	
	return exp(-(float2(Height, Height) / RAYLEIGH_MIE_HEIGHT));
}

float3 ZERNPrecomputingLightScattering_CalculateExtinction(float3 Start, float3 End)
{
	float2 PrevRayleighMieDensity = ZERNPrecomputingLightScattering_CalculateDensity(Start);
	float2 TotalRayleighMieDensity = 0.0f;
	
	float3 StartToEnd = End - Start;
	float3 Direction = normalize(StartToEnd);
	float StepLength = length(StartToEnd) / EXTINCTION_NUM_STEPS;
	
	for(float S = 1.0f; S <= EXTINCTION_NUM_STEPS; S += 1.0f)
	{
		float3 Position = Start + Direction * S * StepLength;
		
		float2 CurRayleighMieDensity = ZERNPrecomputingLightScattering_CalculateDensity(Position);
		
		TotalRayleighMieDensity += ((CurRayleighMieDensity + PrevRayleighMieDensity) * 0.5f) * StepLength;
		
		PrevRayleighMieDensity = CurRayleighMieDensity;
	}
	
	return (RayleighScatteringFactor * TotalRayleighMieDensity.x + MieScatteringFactor * TotalRayleighMieDensity.y);
}

float3 ZERNPrecomputingLightScattering_LookupPrecomputedScattering(Texture3D<float3> ScatteringBuffer, float3 Position, float3 ViewDirection, float3 SunDirection, float3 EarthCenter, inout float PrevTexCoordY)
{
	float3 EarthCenterToPosition = Position - EarthCenter;
	float3 EarthCenterToPositionDirection = normalize(EarthCenterToPosition);
	float Height = length(EarthCenterToPosition) - EARTH_RADIUS;
	float CosViewZenith = dot(ViewDirection, EarthCenterToPositionDirection);
	float CosSunZenith = dot(SunDirection, EarthCenterToPositionDirection);
	float CosSunView = dot(SunDirection, ViewDirection);
	
	float4 TexCoord = ZERNPrecomputingLightScattering_CalculateTexCoordsFromWorldParams(Height, CosViewZenith, CosSunZenith, CosSunView, PrevTexCoordY);
	
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
	
	float3 Inscattering1 = ScatteringBuffer.SampleLevel(ZERNPrecomputingLightScattering_SamplerLinearClamp, TexCoord1, 0);
	float3 Inscattering2 = ScatteringBuffer.SampleLevel(ZERNPrecomputingLightScattering_SamplerLinearClamp, TexCoord2, 0);
	
	return lerp(Inscattering1, Inscattering2, Weight);
}

float3 ZERNPrecomputingLightScattering_Extinction_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float2 TexCoord = PositionViewport.xy / LUT_DIMENSIONS.xy;
	
	float Height, CosViewZenith, CosSunZenith, CosSunView;
	ZERNPrecomputingLightScattering_CalculateWorldParamsFromTexCoords(float4(TexCoord, 0.0f, 0.0f), Height, CosViewZenith, CosSunZenith, CosSunView);
	
	float3 PositionWorld = float3(0.0f, Height, 0.0f);
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	float3 ViewDirection = float3(sqrt(saturate(1.0f - CosViewZenith * CosViewZenith)), CosViewZenith, 0.0f);
	
	float4 StartEndDistance = (float4)0.0f;
	ZERNPrecomputingLightScattering_IntersectionRaySphere(PositionWorld, ViewDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
	
	float3 RayStart = PositionWorld + ViewDirection * max(0.0f, StartEndDistance.x);
	float RayLength = StartEndDistance.y;
	if(StartEndDistance.z > 0.0f)	//if hits the earth
		RayLength = min(RayLength, StartEndDistance.z);
		
	float3 RayEnd = PositionWorld + ViewDirection * RayLength;
	
	float StepLength = length(RayEnd - RayStart) / EXTINCTION_NUM_STEPS;
	
	float2 PrevRayleighMieDensity = ZERNPrecomputingLightScattering_CalculateDensity(RayStart);
	float2 TotalRayleighMieDensity = 0.0f;
	
	for(float S = 1.0f; S <= EXTINCTION_NUM_STEPS; S += 1.0f)
	{
		float3 Position = RayStart + ViewDirection * S * StepLength;
		
		float2 CurRayleighMieDensity = ZERNPrecomputingLightScattering_CalculateDensity(Position);
		
		TotalRayleighMieDensity += ((CurRayleighMieDensity + PrevRayleighMieDensity) * 0.5f) * StepLength;
		
		PrevRayleighMieDensity = CurRayleighMieDensity;
	}
	
	return (RayleighScatteringFactor * TotalRayleighMieDensity.x + MieScatteringFactor * TotalRayleighMieDensity.y);
}

float3 ZERNPrecomputingLightScattering_SingleScattering_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float2 PosXY = PositionViewport.xy / LUT_DIMENSIONS.xy;
	float4 TexCoord = float4(PosXY, ZERNPrecomputingLightScattering_Index, ZERNPrecomputingLightScattering_Index2);
	
	float Height, CosViewZenith, CosSunZenith, CosSunView;
	ZERNPrecomputingLightScattering_CalculateWorldParamsFromTexCoords(TexCoord, Height, CosViewZenith, CosSunZenith, CosSunView);
	
	float3 PositionWorld = float3(0.0f, Height, 0.0f);
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	float3 ViewDirection = float3(sqrt(saturate(1.0f - CosViewZenith * CosViewZenith)), CosViewZenith, 0.0f);
	
	float3 SunDirection;
	SunDirection.x = (ViewDirection.x > 0.0f) ? (CosSunView - CosSunZenith * ViewDirection.y) / ViewDirection.x : 0.0f;
	SunDirection.y = CosSunZenith;	
	SunDirection.z = sqrt(saturate(1.0f - dot(SunDirection.xy, SunDirection.xy)));
	
	float4 StartEndDistance = (float4)0.0f;
	ZERNPrecomputingLightScattering_IntersectionRaySphere(PositionWorld, ViewDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
	
	float3 RayStart = PositionWorld + ViewDirection * max(0.0f, StartEndDistance.x);
	float RayLength = StartEndDistance.y;
	if(StartEndDistance.z > 0.0f)	//if hits the earth
		RayLength = min(RayLength, StartEndDistance.z);
		
	float3 RayEnd = PositionWorld + ViewDirection * RayLength;
	
	float StepLength = length(RayEnd - RayStart) / VIEW_NUM_STEPS;
		
	float3 CurRayleighInScattering = 0.0f;
	float3 CurMieInScattering = 0.0f;	
	float3 PrevRayleighInScattering = 0.0f;
	float3 PrevMieInScattering = 0.0f;
	float3 RayleighInScattering = 0.0f;
	float3 MieInScattering = 0.0f;
	for(float I = 1.0f; I <= VIEW_NUM_STEPS; I += 1.0f)
	{
		float3 Position = RayStart + ViewDirection * I * StepLength;
		
		float3 Extinction = ZERNPrecomputingLightScattering_CalculateExtinction(RayStart, Position);
		
		ZERNPrecomputingLightScattering_IntersectionRaySphere(Position, SunDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
		if(StartEndDistance.x < 0.0f && StartEndDistance.y > 0.0f)
		{
			float3 LightEntryPoint = Position + SunDirection * StartEndDistance.y;
			Extinction += ZERNPrecomputingLightScattering_CalculateExtinction(Position, LightEntryPoint);
		}
		
		Extinction = exp(-Extinction);
		float2 RayleighMieDensity = ZERNPrecomputingLightScattering_CalculateDensity(Position);
		
		CurRayleighInScattering = Extinction * RayleighMieDensity.x;
		CurMieInScattering = Extinction * RayleighMieDensity.y;
		
		RayleighInScattering += ((CurRayleighInScattering + PrevRayleighInScattering) * 0.5f) * StepLength;
		MieInScattering += ((CurMieInScattering + PrevMieInScattering) * 0.5f) * StepLength;
		
		PrevRayleighInScattering = CurRayleighInScattering;
		PrevMieInScattering = CurMieInScattering;
	}
	
	ZERNPrecomputingLightScattering_MultiplyWithPhases(SunDirection, ViewDirection, RayleighInScattering, MieInScattering);
	
	return RayleighInScattering * RayleighScatteringFactor + MieInScattering * MieScatteringFactor;
}

float3 ZERNPrecomputingLightScattering_HighOrderScattering_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float2 PosXY = PositionViewport.xy / LUT_DIMENSIONS.xy;
	float4 TexCoord = float4(PosXY, ZERNPrecomputingLightScattering_Index , ZERNPrecomputingLightScattering_Index2);
	
	float Height, CosViewZenith, CosSunZenith, CosSunView;
	ZERNPrecomputingLightScattering_CalculateWorldParamsFromTexCoords(TexCoord, Height, CosViewZenith, CosSunZenith, CosSunView);
	
	float3 PositionWorld = float3(0.0f, Height, 0.0f);
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	float3 ViewDirection = float3(sqrt(saturate(1.0f - CosViewZenith * CosViewZenith)), CosViewZenith, 0.0f);

	float3 SunDirection;
	SunDirection.x = (ViewDirection.x > 0.0f) ? (CosSunView - CosSunZenith * ViewDirection.y) / ViewDirection.x : 0.0f;
	SunDirection.y = CosSunZenith;	
	SunDirection.z = sqrt(saturate(1.0f - dot(SunDirection.xy, SunDirection.xy)));
	
	float2 ParticleDensity = exp(-Height / RAYLEIGH_MIE_HEIGHT);
	
	float3 TotalScattering = 0.0f;
	for(int I = 0; I < 128; I++)
	{
		float3 Direction = ZERNPrecomputingLightScattering_RandomVectorsTexture.Load(int3(I, 0, 0)).xyz;
		Direction = normalize(Direction);

		float Dummy = -1.0f;
		float3 PrevOrderScattering = ZERNPrecomputingLightScattering_LookupPrecomputedScattering(ZERNPrecomputingLightScattering_PreviousInScatteringBuffer, PositionWorld, Direction, SunDirection, EarthCenter, Dummy);
		
		float3 RayleighInScattering = RayleighScatteringFactor * ParticleDensity.x * PrevOrderScattering;
		float3 MieInScattering =  MieScatteringFactor * ParticleDensity.y * PrevOrderScattering;

		ZERNPrecomputingLightScattering_MultiplyWithPhases(Direction, ViewDirection, RayleighInScattering, MieInScattering);
		
		TotalScattering += RayleighInScattering + MieInScattering;
	}
	
	return TotalScattering * 4.0f * PI / 128.0f;
}

float3 ZERNPrecomputingLightScattering_HighOrderInScattering_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float2 PosXY = PositionViewport.xy / LUT_DIMENSIONS.xy;
	float4 TexCoord = float4(PosXY, ZERNPrecomputingLightScattering_Index , ZERNPrecomputingLightScattering_Index2);
	
	float Height, CosViewZenith, CosSunZenith, CosSunView;
	ZERNPrecomputingLightScattering_CalculateWorldParamsFromTexCoords(TexCoord, Height, CosViewZenith, CosSunZenith, CosSunView);
	
	float3 PositionWorld = float3(0.0f, Height, 0.0f);
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	float3 ViewDirection = float3(sqrt(saturate(1.0f - CosViewZenith * CosViewZenith)), CosViewZenith, 0.0f);
	
	float3 SunDirection;
	SunDirection.x = (ViewDirection.x > 0.0f) ? (CosSunView - CosSunZenith * ViewDirection.y) / ViewDirection.x : 0.0f;
	SunDirection.y = CosSunZenith;	
	SunDirection.z = sqrt(saturate(1.0f - dot(SunDirection.xy, SunDirection.xy)));
	
	float4 StartEndDistance = (float4)0.0f;
	ZERNPrecomputingLightScattering_IntersectionRaySphere(PositionWorld, ViewDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
	
	float3 RayStart = PositionWorld + ViewDirection * max(0.0f, StartEndDistance.x);
	float RayLength = StartEndDistance.y;
	if(StartEndDistance.z > 0.0f)	//if hits the earth
		RayLength = min(RayLength, StartEndDistance.z);
		
	float3 RayEnd = PositionWorld + ViewDirection * RayLength;
	
	float StepLength = length(RayEnd - RayStart) / VIEW_NUM_STEPS;
	
	float2 PrevParticleDensity = exp(-Height / RAYLEIGH_MIE_HEIGHT);
	float Dummy = -1.0f;
	float3 PrevInScattering = ZERNPrecomputingLightScattering_LookupPrecomputedScattering(ZERNPrecomputingLightScattering_HighOrderScatteringBuffer, RayStart, ViewDirection, SunDirection, EarthCenter, Dummy);
	float2 TotalParticleDensity = 0.0f;
	float3 TotalInScattering = 0.0f;
	for(float I = 1.0f; I <= VIEW_NUM_STEPS; I += 1.0f)
	{
		float3 Position = RayStart + ViewDirection * I * StepLength;
		float CurHeight = length(Position - EarthCenter) - EARTH_RADIUS;
		float2 ParticleDensity = exp(-CurHeight / RAYLEIGH_MIE_HEIGHT);
		
		TotalParticleDensity += ((ParticleDensity + PrevParticleDensity) * 0.5f) * StepLength;
		
		float3 RayleighExtinction = RayleighScatteringFactor * TotalParticleDensity.x;
		float3 MieExtinction = MieScatteringFactor * TotalParticleDensity.y;
		float3 Extinction = exp(-(RayleighExtinction + MieExtinction));
		
		float Dummy2 = -1.0f;
		float3 InScattering = Extinction * ZERNPrecomputingLightScattering_LookupPrecomputedScattering(ZERNPrecomputingLightScattering_HighOrderScatteringBuffer, Position, ViewDirection, SunDirection, EarthCenter, Dummy2);

		TotalInScattering += ((InScattering + PrevInScattering) * 0.5f) * StepLength;
		
		PrevParticleDensity = ParticleDensity;
		PrevInScattering = InScattering;
	}
	
	return TotalInScattering;
}

float3 ZERNPrecomputingLightScattering_AddOrders_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	return ZERNPrecomputingLightScattering_PreviousInScatteringBuffer.Load(int4(PositionViewport.xy, ZERNPrecomputingLightScattering_Index, 0));
}

#endif
