//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNLightScattering.hlsl
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

#ifndef __ZERN_LIGHT_SCATTERING_HLSL__
#define __ZERN_LIGHT_SCATTERING_HLSL__

#include "ZERNScreenCover.hlsl"
#include "ZERNView.hlsl"

#define PI					3.14159265359f
#define EARTH_RADIUS		6360000.0f
#define ATMOSPHERE_HEIGHT	60000.0f
#define TOTAL_RADIUS		6420000.0f
#define RAYLEIGH_MIE_HEIGHT float2(7994.0f, 1200.0f)
#define VIEW_NUM_STEPS		20.0f
#define EXT_NUM_STEPS		15.0f
#define FLT_MAX				3.402823466e+38f

cbuffer ZERNLightScattering_Constants : register(b8)
{
	float3			ZERNLightScattering_LightDirection;
	float			ZERNLightScattering_LightIntensity;
	float3			ZERNLightScattering_LightColor;
	float			ZERNLightScattering_MieScatteringStrength;	//[-1, 1]
};

Texture2D			InputTexture : register(t0);
Texture2D<float>	DepthTexture : register(t1);

static const float3 RayleighScatteringFactor	= float3(5.5e-6, 13.0e-6, 22.4e-6);
static const float3 MieScatteringFactor			= float3(21.0e-6, 21.0e-6, 21.0e-6);

void ZERNLightScattering_IntersectionRaySphere(float3 RayOrigin, float3 RayDirection, float3 SphereCenter, float2 SphereRadius, out float4 StartEndDistance)
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

//X: Rayleigh Y: Mie
float2 ZERNLightScattering_CalculateDensity(float3 Position)
{
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	float3 EarthToPosition = Position - EarthCenter;
	float DistanceToEarthCenter = length(EarthToPosition);
	float Height = DistanceToEarthCenter - EARTH_RADIUS;
	
	return exp(-(float2(Height, Height) / RAYLEIGH_MIE_HEIGHT));
}

float3 ZERNLightScattering_CalculateExtinction(float3 Start, float3 End)
{
	float2 RayleighMieDensity = ZERNLightScattering_CalculateDensity(Start);
	float3 PrevRayleighExtinction = RayleighMieDensity.x * RayleighScatteringFactor;
	float3 PrevMieExtinction = RayleighMieDensity.y * MieScatteringFactor;
	float3 CurRayleighExtinction = 0.0f;
	float3 CurMieExtinction = 0.0f;
	float3 RayleighExtinction = 0.0f;
	float3 MieExtinction = 0.0f;
	float3 Direction = normalize(End - Start);
	float StepLength = length(End - Start) / EXT_NUM_STEPS;
	for(float S = 1.0f; S <= EXT_NUM_STEPS; S += 1.0f)
	{
		float3 Position = Start + Direction * S * StepLength;
		
		RayleighMieDensity = ZERNLightScattering_CalculateDensity(Position);
		
		CurRayleighExtinction = RayleighMieDensity.x * RayleighScatteringFactor;
		CurMieExtinction = RayleighMieDensity.y * MieScatteringFactor;
		
		RayleighExtinction += (CurRayleighExtinction + PrevRayleighExtinction) * StepLength * 0.5f;
		MieExtinction += (CurMieExtinction + PrevMieExtinction) * StepLength * 0.5f;
		
		PrevRayleighExtinction = CurRayleighExtinction;
		PrevMieExtinction = CurMieExtinction;
	}
	
	return (RayleighExtinction + MieExtinction);
}

//PositionScreen, [-1, 1]
float3 ZERNLightScattering_CalculateInscattering(float2 PositionScreen, float DepthView)
{
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	
	float2 DirectionVector = float2(PositionScreen) / float2(ZERNView_ProjectionTransform._11, ZERNView_ProjectionTransform._22);
	
	float3 ViewDirection = DirectionVector.x * ZERNView_RightVector.xyz + DirectionVector.y * ZERNView_UpVector.xyz + ZERNView_FrontVector.xyz;
	ViewDirection = normalize(ViewDirection);
	
	/*float R = PositionScreen.x * PositionScreen.x + PositionScreen.y * PositionScreen.y;
	
	if(R > 1.0f)
		return (float3)0.0f;

	float Phi = atan2(PositionScreen.y, PositionScreen.x);
	float Theta = acos(1.0f - R);
	float3 ViewDirection = float3(sin(Theta) * cos(Phi), cos(Theta), sin(Theta) * sin(Phi));*/
	
	float4 StartEndDistance = (float4)0.0f;
	ZERNLightScattering_IntersectionRaySphere(ZERNView_Position.xyz, ViewDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
	
	float3 RayStart = ZERNView_Position.xyz + ViewDirection * max(0.0f, StartEndDistance.x);
	
	float RayLength = StartEndDistance.y;
	if(DepthView < (ZERNView_FarZ - 100.0f))
		RayLength = min((DepthView * 100.0f), RayLength);
	else if(ZERNView_Position.y > ATMOSPHERE_HEIGHT)
		RayLength = StartEndDistance.z;
	
	float3 RayEnd = ZERNView_Position.xyz + ViewDirection * RayLength;
	
	float3 LightDirection = normalize(ZERNLightScattering_LightDirection);
	
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
		
		float3 Extinction = ZERNLightScattering_CalculateExtinction(RayStart, Position);
		
		ZERNLightScattering_IntersectionRaySphere((Position), -LightDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
		if(StartEndDistance.y > 0.0f)
		{
			float3 LightEntryPoint = Position + (-LightDirection) * (StartEndDistance.y);
			Extinction += ZERNLightScattering_CalculateExtinction(Position, LightEntryPoint);
		}
		Extinction = exp(-Extinction);
		float2 RayleighMieDensity = ZERNLightScattering_CalculateDensity(Position);
		
		CurRayleighInScattering = Extinction * RayleighMieDensity.x;
		CurMieInScattering = Extinction * RayleighMieDensity.y;
		
		RayleighInScattering += (CurRayleighInScattering + PrevRayleighInScattering) * StepLength * 0.5f;
		MieInScattering += (CurMieInScattering + PrevMieInScattering) * StepLength * 0.5f;
		
		PrevRayleighInScattering = CurRayleighInScattering;
		PrevMieInScattering = CurMieInScattering;
	}
	
	float CosAngle = dot(LightDirection, -ViewDirection);
	float PhaseRayleigh = (3.0f / (16.0f * PI)) * (1.0f + CosAngle * CosAngle);
	float MieScatteringStrenghSquare = ZERNLightScattering_MieScatteringStrength * ZERNLightScattering_MieScatteringStrength;
	float PhaseMie = (3.0f / (8.0f * PI)) * ((1.0f - MieScatteringStrenghSquare) * (1.0f + CosAngle * CosAngle)) / ((2.0f + MieScatteringStrenghSquare) * pow(abs(1.0f + MieScatteringStrenghSquare - 2.0f * ZERNLightScattering_MieScatteringStrength * CosAngle), 1.5f));
	
	return ZERNLightScattering_LightIntensity * (RayleighInScattering * RayleighScatteringFactor * PhaseRayleigh + MieInScattering * MieScatteringFactor * PhaseMie);
}

float3 ZERNLightScattering_GetPositionWorld(float2 PositionViewport)
{	
	float2 TextureDimension;
	DepthTexture.GetDimensions(TextureDimension.x, TextureDimension.y);
	
	float2 PositionScreen = (PositionViewport - 0.5f) * (float2(2.0f, -2.0f) / TextureDimension) + float2(-1.0f, 1.0f);
	float Depth = DepthTexture.Load(int3(PositionViewport, 0)).x;
	
	float4 PositionWorld = mul(ZERNView_InvViewProjectionTransform, float4(PositionScreen, Depth, 1.0f));
	PositionWorld /= PositionWorld.w;
	
	return PositionWorld.xyz;
}

float3 ZERNLightScattering_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float Depth = DepthTexture.Load(int3(PositionViewport.xy, 0)).x;
	float DepthView = ZERNView_ProjectionTransform._34 / (Depth - ZERNView_ProjectionTransform._33);
	
	float2 TextureDimension;
	DepthTexture.GetDimensions(TextureDimension.x, TextureDimension.y);
	
	float2 PositionScreen = (PositionViewport.xy - 0.5f) * (float2(2.0f, -2.0f) / TextureDimension) + float2(-1.0f, 1.0f);
	
	float3 PixelColor = InputTexture.Load(int3(PositionViewport.xy, 0)).rgb;
	
	float3 PositionWorld = ZERNLightScattering_GetPositionWorld(PositionViewport.xy);
	float3 Extinction = exp(-ZERNLightScattering_CalculateExtinction(ZERNView_Position.xyz, float3(PositionWorld.xy, DepthView * 100.0f)));
	float3 InScattering = ZERNLightScattering_CalculateInscattering(PositionScreen, DepthView);
	
	return PixelColor * Extinction + InScattering;
}

#endif
