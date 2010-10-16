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

#include <Shadow.hlsl>

//* Vertex Shader Constants
/////////////////////////////////////////////////////////////////////////////////////////

// Vertex Transformation
float3 ViewVector : register(vs, c0);

float4x4 WorldViewProjMatrix : register(vs, c4);

// Light Parameters
float4 LightParameters0 : register(ps, c0);
float4 LightParameters1 : register(ps, c1);
float4 LightParameters2 : register(ps, c2);
float4 LightParameters3 : register(ps, c3);
float2 PixelSize_2 : register(ps, c5);

#define LightPositionParam			LightParameters0.xyz
#define LightRange					LightParameters0.w
#define LightColorParam				LightParameters1.xyz
#define LightIntensityParam			LightParameters1.w
#define LightAttenuationParam		LightParameters2.xyz
#define LightDirectionParam			LightParameters3.xyz
#define LightFOVParam				LightParameters3.w

float3x3 LightRotationParam : register(ps, c12);
float4x4 LightProjectionMatrixParam : register(ps, c16);

sampler2D GBuffer1 : register(ps, s0);
sampler2D GBuffer2 : register(ps, s1);
sampler2D ProjectionMap : register(ps, s2);
samplerCUBE OmniProjectionMap : register(ps, s3);
sampler2D ProjectionShadowMap : register(ps, s4);
samplerCUBE OmniProjectionShadowMap : register(ps, s5);

#include "GBuffer.hlsl"

// Point Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct PLVSOutput
{
	float4 Position : POSITION0;
	float4 ScreenPosition : TEXCOORD0;
	float3 ViewVector : TEXCOORD1;
};

PLVSOutput PLVSMain(float4 Position : POSITION0)
{
	PLVSOutput Output;
	
	Output.Position = mul(Position, WorldViewProjMatrix);
	
	Output.ScreenPosition.xy = float2(Output.Position.x, -Output.Position.y) * 0.5f;
	Output.ScreenPosition.zw = Output.Position.zw;
	Output.ViewVector = Output.Position * ViewVector;
	
	return Output;
}

struct PLPSInput
{
	float4 ScreenPosition : TEXCOORD0;
	float3 ViewVector : TEXCOORD1;
};

float4 PLPSMain(PLPSInput Input) : COLOR0
{
	float4 Output = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	float2 ScreenPosition = Input.ScreenPosition.xy / Input.ScreenPosition.w + 0.5f + PixelSize_2;
	float3 Position = GetViewPosition(GBuffer1, ScreenPosition, Input.ViewVector);
	float3 Normal = GetViewNormal(GBuffer2, ScreenPosition);

	float3 LightDisplacement = LightPositionParam - Position;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;
	
	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	if (AngularAttenuation > 0.0f)
	{
		float DistanceAttenuation = 1.0f / dot(LightAttenuationParam, float3(1.0f, LightDistance, LightDistance * LightDistance));

		float3 SpecularPower = GetSpecularPower(GBuffer2, ScreenPosition);

		float ViewDirection = normalize(-Position);
		float3 HalfVector = normalize(LightDirection + ViewDirection);		

		Output.xyz = LightIntensityParam * LightColorParam;
		Output.w = pow(dot(Normal, HalfVector), SpecularPower);

		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;

	}
	
	return Output;
}

// Directional Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct DLVSOutput
{
	float4 Position : POSITION0;
	float4 ScreenPosition : TEXCOORD0;
	float3 ViewVector : TEXCOORD1;
};

PLVSOutput DLVSMain(float4 Position : POSITION0)
{
	DLVSOutput Output;
	
	Output.Position = Position;
	Output.ScreenPosition.xy = float2(Output.Position.x, -Output.Position.y) * 0.5f;
	Output.ScreenPosition.zw = Output.Position.zw;
	Output.ViewVector = Output.Position * ViewVector;
	
	return Output;
}

struct DLPSInput
{
	float4 ScreenPosition : TEXCOORD0;
	float3 ViewVector : TEXCOORD1;
};

float4 DLPSMain(PLPSInput Input) : COLOR0
{
	float4 Output;
	
	float2 ScreenPosition = Input.ScreenPosition.xy / Input.ScreenPosition.w + 0.5f + PixelSize_2;
	float3 Position = GetViewPosition(GBuffer1, ScreenPosition, Input.ViewVector);
	float3 Normal = GetViewNormal(GBuffer2, ScreenPosition);
	float3 SpecularPower = GetSpecularPower(GBuffer2, ScreenPosition);

	// Light Derived Parameters
	float ViewDirection = normalize(-Position);
	float3 HalfVector = normalize(LightDirectionParam + ViewDirection);
		
	float AngularAttenuation = saturate(dot(LightDirectionParam, Normal));
	Output.xyz = LightIntensityParam * LightColorParam;
	Output.w = pow(dot(Normal, HalfVector), SpecularPower);
	
	Output *= AngularAttenuation;

	return Output;
}

// Projective Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PjLVSOutput
{
	float4 Position : POSITION0;
	float4 ScreenPosition : TEXCOORD0;
	float3 ViewVector : TEXCOORD1;
};

PjLVSOutput PjLVSMain(float4 Position : POSITION0)
{
	PjLVSOutput Output;
	
	Output.Position = mul(Position, WorldViewProjMatrix);
	Output.ScreenPosition = Output.Position;
	Output.ViewVector = Output.Position * ViewVector;
	Output.ScreenPosition = Output.Position * 0.5f ;
	Output.ScreenPosition.y = -Output.ScreenPosition.y;
	Output.ScreenPosition.zw = Output.Position.zw;
	return Output;
}

struct PjLPSInput
{
	float4 ScreenPosition : TEXCOORD0;
	float3 ViewVector : TEXCOORD1;
};
	
float4 PjLPSMain(PjLPSInput Input) : COLOR0
{
	float4 Output = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	float2 ScreenPosition = Input.ScreenPosition.xy / Input.ScreenPosition.w + 0.5f + PixelSize_2;
	float3 Position = GetViewPosition(GBuffer1, ScreenPosition, Input.ViewVector);
	float3 Normal = GetViewNormal(GBuffer2, ScreenPosition);

	float3 LightDisplacement = LightPositionParam - Position;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;

	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	if (AngularAttenuation > 0.0f)
	{
		float3 SpecularPower = GetSpecularPower(GBuffer2, ScreenPosition);

		float ViewDirection = normalize(-Position);
		float3 HalfVector = normalize(LightDirection + ViewDirection);

		float DistanceAttenuation = 1.0f / dot(LightAttenuationParam, float3(1.0f, LightDistance, LightDistance * LightDistance));

		float4 TextureLookup = mul(float4(Position, 1.0f), LightProjectionMatrixParam);
		float3 ProjLightColor = tex2Dproj(ProjectionMap, TextureLookup) * LightColorParam;

		Output.xyz = LightIntensityParam * ProjLightColor;
		Output.w = pow(dot(Normal, HalfVector), SpecularPower);

		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;

		Output *= SampleShadowMap(ProjectionShadowMap, TextureLookup);
	}
	return Output;
}


// Omni Projective Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct OPLVSOutput
{
	float4 Position : POSITION0;
	float3 ScreenPosition : TEXCOORD0;
	float3 ViewVector : TEXCOORD1;
};

OPLVSOutput OPLVSMain(float4 Position : POSITION0)
{
	OPLVSOutput Output;
	
	Output.Position = mul(Position, WorldViewProjMatrix);
	Output.ScreenPosition.xy = float2(Output.Position.x, -Output.Position.y);
	Output.ScreenPosition.z = Output.Position.w;
	Output.ViewVector = Output.Position * ViewVector;
	
	return Output;
}

struct OPLPSInput
{
	float3 ScreenPosition : TEXCOORD0;
	float3 ViewVector : TEXCOORD1;
};
	
float4 OPLPSMain(OPLPSInput Input) : COLOR0
{
	float4 Output = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	float2 ScreenPosition = Input.ScreenPosition.xy / Input.ScreenPosition.z * 0.5f + 0.5f + PixelSize_2;
	float3 Position = GetViewPosition(GBuffer1, ScreenPosition, Input.ViewVector);
	float3 Normal = GetViewNormal(GBuffer2, ScreenPosition);

	float3 LightDisplacement = LightPositionParam - Position;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;
	
	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	if (AngularAttenuation > 0.0f)
	{	
		float SpecularPower = GetSpecularPower(GBuffer2, ScreenPosition);
		
		float3 TextureLookup = mul(LightDirection, (float3x3)LightRotationParam);
		float3 ProjLightColor = texCUBE(OmniProjectionMap, TextureLookup).rgb * LightColorParam;
		
		float ViewDirection = normalize(-Position);
		float3 HalfVector = normalize(LightDirection + ViewDirection);
			
		float DistanceAttenuation = 1.0f / dot(LightAttenuationParam, float3(1.0f, LightDistance, LightDistance * LightDistance));
		Output.xyz = LightIntensityParam * ProjLightColor;
		Output.w = pow(dot(Normal, HalfVector), SpecularPower);
		
		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;
	}
	
	return Output;
}
