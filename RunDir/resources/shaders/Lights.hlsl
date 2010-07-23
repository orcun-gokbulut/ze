//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Lights.hlsl
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

//* Vertex Shader Constants
/////////////////////////////////////////////////////////////////////////////////////////

// Vertex Transformation
float3 ViewVector : register(vs, c0);
float2 PixelSize : register(vs, c1);

float4x4 WorldViewProjMatrix : register(vs, c4);

// Light Parameters
float4 LightParameters0 : register(ps, c0);
float4 LightParameters1 : register(ps, c1);
float4 LightParameters2 : register(ps, c2);

#define LightPosition			LightParameters0.xyz
#define LightRange			
#define LightColor				LightParameters1.xyz
#define LightIntensity			LightParameters1.w
#define LightAttenuationFactors	LightParameters2.xyz

sampler2D GBuffer1;
sampler2D GBuffer2;

#include "GBuffer.hlsl"

// Point Light
struct PLVSOutput
{
	float4 Position : POSITION0;
	float2 ScreenPosition : TEXCOORD0;
	float3 ViewVector : TEXCOORD1;
};

PLVSOutput PLVSMain(float3 Position : POSITION0)
{
	PLVSOutput Output;
	
	Output.Position = mul(Position, WorldViewProjMatrix);
	Output.ScreenPosition = Output.Position * 0.5f - 0.5f + PixelSize * 0.5f;
	Output.ViewVector = Output.Position * ViewVector;
	
	return Output;
}

struct PLVSInput
{
	float2 ScreenPosition : TEXCOORD0;
	float3 ViewVector : TEXCOORD1;
};

float4 PLPSMain(PLVSInput Input) : COLOR0
{
	float4 Output;
	
	float3 Position = GetViewPosition(GBuffer1, Input.ScreenPosition, Input.ViewVector);
	float3 Normal = GetViewNormal(GBuffer2, Input.ScreenPosition);
	float3 SpecularPower = GetSpecularPower(GBuffer2, Input.ScreenPosition);

	// Light Derived Parameters
	float3 LightDisplacement = LightPosition - Position;
	float LightDistance = length(LightDisplacement);
	float LightDirection = LightDisplacement / LightDistance;
	float ViewDirection = normalize(-Position);
	float3 HalfVector = normalize(LightDirection + ViewDirection);
		
	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	float DistanceAttenuation = 1.0f / dot(LightAttenuationFactors, float3(LightDistance * LightDistance, LightDistance, 1.0f));
	Output.xyz = AngularAttenuation * DistanceAttenuation * LightIntensity * LightColor;
	Output.w = AngularAttenuation * pow(dot(Normal, HalfVector), SpecularPower) * DistanceAttenuation;
	
	return Output;
}

// Directional Light

// Projective Light

// Omni Projective Light
