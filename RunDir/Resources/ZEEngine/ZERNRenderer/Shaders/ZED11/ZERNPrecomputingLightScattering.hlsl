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

#include "ZERNLightScatteringCommon.hlsl"
#include "ZERNTransformations.hlsl"
#include "ZERNIntersections.hlsl"
#include "ZERNScreenCover.hlsl"

cbuffer ZERNPrecomputingLightScattering_PrecomputeConstants							: register(b8)
{
	float			ZERNPrecomputingLightScattering_IndexZ;
	float			ZERNPrecomputingLightScattering_IndexW;
	float2			ZERNPrecomputingLightScattering_Reserved0;
};

Texture3D<float3>	ZERNPrecomputingLightScattering_PreviousInScatteringBuffer		: register(t0);
Texture3D<float3>	ZERNPrecomputingLightScattering_HighOrderScatteringBuffer		: register(t1);
Texture2D<float3>	ZERNPrecomputingLightScattering_RandomVectorsTexture			: register(t2);

Texture2D<float2>	ZERNPrecomputingLightScattering_DensityBuffer					: register(t5);
Texture2D<float3>	ZERNPrecomputingLightScattering_SkyAmbientBuffer				: register(t6);

void ZERNPrecomputingLightScattering_MultiplyWithPhases(in float3 SunDirection, in float3 ViewDirection, inout float3 RayleighInScattering, inout float3 MieInScattering)
{
	float CosAngle = dot(SunDirection, ViewDirection);

	float3 PhaseRayleigh =  (3.0f / (16.0f * PI)) * (1.0f + CosAngle * CosAngle);
	
	float G = 0.65f;
	float GG = G * G;
	float3 PhaseMie = (3.0f / (8.0f * PI)) * ((1.0f - GG) * (1.0f + CosAngle * CosAngle)) / ((2.0f + GG) * pow(1.0f + GG - 2.0f * G * CosAngle, 3.0f / 2.0f));
	
	RayleighInScattering *= PhaseRayleigh;
	MieInScattering *= PhaseMie;
}

float2 ZERNPrecomputingLightScattering_Density_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float2 TexCoord = (PositionViewport.xy - 0.5f) / float2(1023.0f, 1023.0f);
	
	float Height = TexCoord.x * (ATMOSPHERE_HEIGHT - 2.0f * 16) + 16.0f;
	
	float CosViewZenith = TexCoord.y * 2.0f - 1.0f;
	
	float3 PositionWorld = float3(0.0f, Height, 0.0f);
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	float3 ViewDirection = float3(sqrt(1.0f - CosViewZenith * CosViewZenith), CosViewZenith, 0.0f);
	
	float4 StartEndDistance = (float4)0.0f;
	ZERNIntersections_RaySphere2(PositionWorld, ViewDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
	
	float3 RayStart = PositionWorld + ViewDirection * max(0.0f, StartEndDistance.x);
	float RayLength = StartEndDistance.y;
	if(StartEndDistance.z > 0.0f)	//if hits the earth
		RayLength = min(RayLength, StartEndDistance.z);
		
	float3 RayEnd = PositionWorld + ViewDirection * RayLength;
	
	float StepLength = length(RayEnd - RayStart) / EXTINCTION_NUM_STEPS;
	
	float2 PrevRayleighMieDensity = ZERNLightScatteringCommon_CalculateDensity(RayStart);
	float2 TotalRayleighMieDensity = 0.0f;
	
	for(float S = 1.0f; S <= EXTINCTION_NUM_STEPS; S += 1.0f)
	{
		float3 Position = RayStart + ViewDirection * S * StepLength;
		
		float2 CurRayleighMieDensity = ZERNLightScatteringCommon_CalculateDensity(Position);
		
		TotalRayleighMieDensity += (CurRayleighMieDensity + PrevRayleighMieDensity) * 0.5f * StepLength;
		
		PrevRayleighMieDensity = CurRayleighMieDensity;
	}
	
	return TotalRayleighMieDensity;
}

float3 ZERNPrecomputingLightScattering_SingleScattering_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float2 TexCoordXY = PositionViewport.xy / LUT_DIMENSIONS.xy;
	float4 TexCoord = float4(TexCoordXY, ZERNPrecomputingLightScattering_IndexZ, ZERNPrecomputingLightScattering_IndexW);
	
	float Height, CosViewZenith, CosSunZenith, CosSunView;
	ZERNLightScatteringCommon_CalculateWorldParamsFromTexCoords(TexCoord, Height, CosViewZenith, CosSunZenith, CosSunView);
	
	float3 PositionWorld = float3(0.0f, Height, 0.0f);
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	float3 ViewDirection = float3(sqrt(saturate(1.0f - CosViewZenith * CosViewZenith)), CosViewZenith, 0.0f);
	
	float3 SunDirection;
	SunDirection.x = (ViewDirection.x > 0.0f) ? (CosSunView - CosSunZenith * ViewDirection.y) / ViewDirection.x : 0.0f;
	SunDirection.y = CosSunZenith;	
	SunDirection.z = sqrt(saturate(1.0f - dot(SunDirection.xy, SunDirection.xy)));
	
	float4 StartEndDistance = (float4)0.0f;
	ZERNIntersections_RaySphere2(PositionWorld, ViewDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
	
	float3 RayStart = PositionWorld + ViewDirection * max(0.0f, StartEndDistance.x);
	float RayLength = StartEndDistance.y;
	if(StartEndDistance.z > 0.0f)	//if hits the earth
		RayLength = min(RayLength, StartEndDistance.z);
		
	float3 RayEnd = PositionWorld + ViewDirection * RayLength;
	
	float StepLength = length(RayEnd - RayStart) / VIEW_NUM_STEPS;
		
	float3 PrevRayleighInScattering = 0.0f;
	float3 PrevMieInScattering = 0.0f;
	float3 TotalRayleighInScattering = 0.0f;
	float3 TotalMieInScattering = 0.0f;
	for(float I = 1.0f; I <= VIEW_NUM_STEPS; I += 1.0f)
	{
		float3 Position = RayStart + ViewDirection * I * StepLength;
		
		float3 Extinction = ZERNLightScatteringCommon_CalculateExtinction(RayStart, Position);
		
		ZERNIntersections_RaySphere1(Position, SunDirection, EarthCenter, TOTAL_RADIUS, StartEndDistance.xy);
		if(StartEndDistance.x < 0.0f && StartEndDistance.y > 0.0f)
		{
			StartEndDistance.y = clamp(StartEndDistance.y, 50.0f, ATMOSPHERE_HEIGHT - 50.0f);
			
			float3 LightEntryPoint = Position + SunDirection * StartEndDistance.y;
			Extinction += ZERNLightScatteringCommon_CalculateExtinction(Position, LightEntryPoint);
		}
		
		Extinction = exp(-Extinction);
		float2 RayleighMieDensity = ZERNLightScatteringCommon_CalculateDensity(Position);
		
		float3 CurRayleighInScattering = Extinction * RayleighMieDensity.x;
		float3 CurMieInScattering = Extinction * RayleighMieDensity.y;
		
		TotalRayleighInScattering += (CurRayleighInScattering + PrevRayleighInScattering) * 0.5f * StepLength;
		TotalMieInScattering += (CurMieInScattering + PrevMieInScattering) * 0.5f * StepLength;
		
		PrevRayleighInScattering = CurRayleighInScattering;
		PrevMieInScattering = CurMieInScattering;
	}
	
	ZERNPrecomputingLightScattering_MultiplyWithPhases(SunDirection, ViewDirection, TotalRayleighInScattering, TotalMieInScattering);
	
	return (TotalRayleighInScattering * ZERNLightScatteringCommon_RayleighScatteringFactor + TotalMieInScattering * ZERNLightScatteringCommon_MieScatteringFactor);
}

float3 ZERNPrecomputingLightScattering_HighOrderScattering_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float2 TexCoordXY = PositionViewport.xy / LUT_DIMENSIONS.xy;
	float4 TexCoord = float4(TexCoordXY, ZERNPrecomputingLightScattering_IndexZ, ZERNPrecomputingLightScattering_IndexW);
	
	float Height, CosViewZenith, CosSunZenith, CosSunView;
	ZERNLightScatteringCommon_CalculateWorldParamsFromTexCoords(TexCoord, Height, CosViewZenith, CosSunZenith, CosSunView);
	
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
		float3 PrevOrderScattering = ZERNLightScatteringCommon_LookupPrecomputedScattering(ZERNPrecomputingLightScattering_PreviousInScatteringBuffer, PositionWorld, Direction, SunDirection, EarthCenter, Dummy);
		
		float3 RayleighInScattering = ParticleDensity.x * PrevOrderScattering;
		float3 MieInScattering = ParticleDensity.y * PrevOrderScattering;

		ZERNPrecomputingLightScattering_MultiplyWithPhases(-Direction, ViewDirection, RayleighInScattering, MieInScattering);
		
		TotalScattering += (RayleighInScattering * ZERNLightScatteringCommon_RayleighScatteringFactor + MieInScattering * ZERNLightScatteringCommon_MieScatteringFactor);
	}
	
	return TotalScattering * 4.0f * PI / 128.0f;
}

float3 ZERNPrecomputingLightScattering_HighOrderInScattering_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float2 TexCoordXY = PositionViewport.xy / LUT_DIMENSIONS.xy;
	float4 TexCoord = float4(TexCoordXY, ZERNPrecomputingLightScattering_IndexZ, ZERNPrecomputingLightScattering_IndexW);
	
	float Height, CosViewZenith, CosSunZenith, CosSunView;
	ZERNLightScatteringCommon_CalculateWorldParamsFromTexCoords(TexCoord, Height, CosViewZenith, CosSunZenith, CosSunView);
	
	float3 PositionWorld = float3(0.0f, Height, 0.0f);
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	float3 ViewDirection = float3(sqrt(saturate(1.0f - CosViewZenith * CosViewZenith)), CosViewZenith, 0.0f);
	
	float3 SunDirection;
	SunDirection.x = (ViewDirection.x > 0.0f) ? (CosSunView - CosSunZenith * ViewDirection.y) / ViewDirection.x : 0.0f;
	SunDirection.y = CosSunZenith;	
	SunDirection.z = sqrt(saturate(1.0f - dot(SunDirection.xy, SunDirection.xy)));
	
	float4 StartEndDistance = (float4)0.0f;
	ZERNIntersections_RaySphere2(PositionWorld, ViewDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
	
	float3 RayStart = PositionWorld + ViewDirection * max(0.0f, StartEndDistance.x);
	float RayLength = StartEndDistance.y;
	if(StartEndDistance.z > 0.0f)	//if hits the earth
		RayLength = min(RayLength, StartEndDistance.z);
		
	float3 RayEnd = PositionWorld + ViewDirection * RayLength;
	
	float StepLength = length(RayEnd - RayStart) / VIEW_NUM_STEPS;
	
	float2 PrevParticleDensity = exp(-Height / RAYLEIGH_MIE_HEIGHT);
	float Dummy = -1.0f;
	float3 PrevInScattering = ZERNLightScatteringCommon_LookupPrecomputedScattering(ZERNPrecomputingLightScattering_HighOrderScatteringBuffer, RayStart, ViewDirection, SunDirection, EarthCenter, Dummy);
	float2 TotalParticleDensity = 0.0f;
	float3 TotalInScattering = 0.0f;
	for(float I = 1.0f; I <= VIEW_NUM_STEPS; I += 1.0f)
	{
		float3 Position = RayStart + ViewDirection * I * StepLength;
		float CurHeight = length(Position - EarthCenter) - EARTH_RADIUS;
		float2 CurParticleDensity = exp(-CurHeight / RAYLEIGH_MIE_HEIGHT);
		
		TotalParticleDensity += (CurParticleDensity + PrevParticleDensity) * 0.5f * StepLength;
		
		float3 RayleighExtinction = ZERNLightScatteringCommon_RayleighScatteringFactor * TotalParticleDensity.x;
		float3 MieExtinction = ZERNLightScatteringCommon_MieScatteringFactor * TotalParticleDensity.y;
		float3 Extinction = exp(-(RayleighExtinction + MieExtinction));
		
		float Dummy2 = -1.0f;
		float3 CurInScattering = Extinction * ZERNLightScatteringCommon_LookupPrecomputedScattering(ZERNPrecomputingLightScattering_HighOrderScatteringBuffer, Position, ViewDirection, SunDirection, EarthCenter, Dummy2);

		TotalInScattering += (CurInScattering + PrevInScattering) * 0.5f * StepLength;
		
		PrevParticleDensity = CurParticleDensity;
		PrevInScattering = CurInScattering;
	}
	
	return TotalInScattering;
}

float3 ZERNPrecomputingLightScattering_AddOrders_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	return ZERNPrecomputingLightScattering_PreviousInScatteringBuffer.Load(int4(PositionViewport.xy, ZERNPrecomputingLightScattering_IndexZ, 0));
}

float3 ZERNPrecomputingLightScattering_SkyAmbient_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float CosSunEarthNormal = ((PositionViewport.x - 0.5f) / 1023.0f) * 2.0f - 1.0f;
	CosSunEarthNormal = clamp(CosSunEarthNormal, -1.0f, + 1.0f);
	float3 PositionWorld = float3(0.0f, 20.0f, 0.0f);
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	float3 SunDirection;
	SunDirection.x = sqrt(saturate(1.0f - CosSunEarthNormal * CosSunEarthNormal));
	SunDirection.y = CosSunEarthNormal;
	SunDirection.z = 0.0f;
	
	float3 TotalSkyAmbient = 0.0f;
	for(int I = 0; I < 128; I++)
	{
		float3 Direction = ZERNPrecomputingLightScattering_RandomVectorsTexture.Load(int3(I, 0, 0)).xyz;
		Direction = normalize(Direction);
		Direction.y = abs(Direction.y);
		
		float PrevTexCoordY = -1.0f;
		float3 Inscattering = ZERNLightScatteringCommon_LookupPrecomputedScattering(ZERNPrecomputingLightScattering_PreviousInScatteringBuffer, PositionWorld, Direction, SunDirection, EarthCenter, PrevTexCoordY);
		TotalSkyAmbient += Inscattering * dot(Direction, float3(0.0f, 1.0f, 0.0f));
	}
	
	return TotalSkyAmbient * 2.0f * PI / 128.0f;
}
#endif
