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
float4x4 WorldViewProjMatrix : register(vs, c4);

// Light Parameters
float4 LightParameters0 : register(ps, c0);
float4 LightParameters1 : register(ps, c1);
float4 LightParameters2 : register(ps, c2);
float4 LightParameters3 : register(ps, c3);
float4 ScreenToTextureParams : register(ps, c5);

#define LightPositionParam			LightParameters0.xyz
#define LightRange					LightParameters0.w
#define LightColorParam				LightParameters1.xyz
#define LightIntensityParam			LightParameters1.w
#define LightAttenuationParam		LightParameters2.xyz
#define LightDirectionParam			LightParameters3.xyz
#define LightFOVParam				LightParameters3.w

float3x3 LightRotationParam : register(ps, c12);
float4x4 LightProjectionMatrixParam : register(ps, c16);

sampler2D ProjectionMap : register(ps, s2);
samplerCUBE OmniProjectionMap : register(ps, s3);
sampler2D ProjectionShadowMap : register(ps, s4);
samplerCUBE OmniProjectionShadowMap : register(ps, s5);

#include "GBuffer.hlsl"
#include "LBuffer.hlsl"

// Point Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ZEPointLight_VSOutput
{
	float4 Position : POSITION0;
	float3 ViewVector : TEXCOORD0;
};

ZEPointLight_VSOutput ZEPointLight_VertexShader(in float4 Position : POSITION0)
{
	ZEPointLight_VSOutput Output;
	
	Output.Position = mul(Position, WorldViewProjMatrix);
	
	Output.ViewVector = ZEGBuffer_GetViewVector(Output.Position);
	
	return Output;
}

struct ZEPointLight_PSInput
{
	float4 ScreenPosition : VPOS;
	float3 ViewVector : TEXCOORD0;
};

ZELBuffer ZEPointLight_PixelShader(ZEPointLight_PSInput Input) : COLOR0
{
	ZELBuffer LBuffer = (ZELBuffer)0;
	
	float2 ScreenPosition = Input.ScreenPosition * ScreenToTextureParams.xy + ScreenToTextureParams.zw;		
	float3 Position = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);

	float3 LightDisplacement = LightPositionParam - Position;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;
	
	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	if (AngularAttenuation > 0.0f)
	{
		float DistanceAttenuation = 1.0f / dot(LightAttenuationParam, float3(1.0f, LightDistance, LightDistance * LightDistance));

		float3 SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);

		float ViewDirection = normalize(-Position);
		float3 HalfVector = normalize(LightDirection + ViewDirection);		

		float4 Output;
		Output.rgb = LightIntensityParam * LightColorParam;
		Output.a = pow(dot(Normal, HalfVector), SpecularPower);

		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;
		
		ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
		ZELBuffer_SetSpecular(LBuffer, Output.a);
	}
	
	return LBuffer;
}

// Directional Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ZEDirectionalLight_VSOutput
{
	float4 Position : POSITION0;
	float3 ViewVector : TEXCOORD0;
};

ZEDirectionalLight_VSOutput ZEDirectionalLight_VertexShader(float4 Position : POSITION0)
{
	ZEDirectionalLight_VSOutput Output;
	
	Output.Position = Position;

	Output.ViewVector = ZEGBuffer_GetViewVector(Output.Position);
	
	return Output;
}

struct ZEDirectionalLight_PSInput
{
	float4 ScreenPosition : VPOS;
	float3 ViewVector : TEXCOORD0;
};

ZELBuffer ZEDirectionalLight_PixelShader(ZEDirectionalLight_PSInput Input) : COLOR0
{
	ZELBuffer LBuffer = (ZELBuffer)0;
		
	float2 ScreenPosition = Input.ScreenPosition * ScreenToTextureParams.xy + ScreenToTextureParams.zw;		
	float3 Position = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);
	float3 SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);

	// Light Derived Parameters
	float3 ViewDirection = normalize(-Position);
	float3 HalfVector = normalize(LightDirectionParam + ViewDirection);
		
	float AngularAttenuation = saturate(dot(LightDirectionParam, Normal));
	
	float4 Output;
	Output.rgb = LightIntensityParam * LightColorParam;
	Output.a = pow(dot(Normal, HalfVector), SpecularPower);
	
	Output *= AngularAttenuation;
	
	ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
	ZELBuffer_SetSpecular(LBuffer, Output.a);

	return LBuffer;
}

// Projective Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ZEProjectiveLight_VSOutput
{
	float4 Position : POSITION0;
	float3 ViewVector : TEXCOORD0;
};

ZEProjectiveLight_VSOutput ZEProjectiveLight_VertexShader(float4 Position : POSITION0)
{
	ZEProjectiveLight_VSOutput Output;
	
	Output.Position = mul(Position, WorldViewProjMatrix);
	
	Output.ViewVector = ZEGBuffer_GetViewVector(Output.Position);

	return Output;
}

struct ZEProjectiveLight_PSInput
{
	float4 ScreenPosition : VPOS;
	float3 ViewVector : TEXCOORD0;
};
	
ZELBuffer ZEProjectiveLight_PixelShader(ZEProjectiveLight_PSInput Input) : COLOR0
{
	ZELBuffer LBuffer = (ZELBuffer)0;

	float2 ScreenPosition = Input.ScreenPosition * ScreenToTextureParams.xy + ScreenToTextureParams.zw;			
	float3 Position = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);

	float3 LightDisplacement = LightPositionParam - Position;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;

	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	if (AngularAttenuation > 0.0f)
	{
		float3 SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);

		float ViewDirection = normalize(-Position);
		float3 HalfVector = normalize(LightDirection + ViewDirection);

		float DistanceAttenuation = 1.0f / dot(LightAttenuationParam, float3(1.0f, LightDistance, LightDistance * LightDistance));

		float4 TextureLookup = mul(float4(Position, 1.0f), LightProjectionMatrixParam);
		float3 ProjLightColor = tex2Dproj(ProjectionMap, TextureLookup) * LightColorParam;

		float4 Output;
		Output.rgb = LightIntensityParam * ProjLightColor;
		Output.a = pow(dot(Normal, HalfVector), SpecularPower);

		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;

		Output *= SampleShadowMap(ProjectionShadowMap, TextureLookup);
		
		ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
		ZELBuffer_SetSpecular(LBuffer, Output.a);
	}
	
	return LBuffer;
}


// Omni Projective Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ZEOmniProjectiveLight_VSOutput
{
	float4 Position : POSITION0;
	float3 ViewVector : TEXCOORD0;
};

ZEOmniProjectiveLight_VSOutput ZEOmniProjectiveLight_VertexShader(float4 Position : POSITION0)
{
	ZEOmniProjectiveLight_VSOutput Output;
	
	Output.Position = mul(Position, WorldViewProjMatrix);
	Output.ViewVector = ZEGBuffer_GetViewVector(Output.Position);
	
	return Output;
}

struct ZEOmniProjectiveLight_PSInput
{
	float3 ScreenPosition : VPOS;
	float3 ViewVector : TEXCOORD0;
};
	
ZELBuffer ZEOmniProjectiveLight_PixelShader(ZEOmniProjectiveLight_PSInput Input) : COLOR0
{
	ZELBuffer LBuffer = (ZELBuffer)0;
	
	float2 ScreenPosition = Input.ScreenPosition * ScreenToTextureParams.xy + ScreenToTextureParams.zw;		
	float3 Position = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);

	float3 LightDisplacement = LightPositionParam - Position;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;
	
	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	if (AngularAttenuation > 0.0f)
	{	
		float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);
		
		float3 TextureLookup = mul(LightDirection, (float3x3)LightRotationParam);
		float3 ProjLightColor = texCUBE(OmniProjectionMap, TextureLookup).rgb * LightColorParam;
		
		float ViewDirection = normalize(-Position);
		float3 HalfVector = normalize(LightDirection + ViewDirection);
			
		float DistanceAttenuation = 1.0f / dot(LightAttenuationParam, float3(1.0f, LightDistance, LightDistance * LightDistance));

		float4 Output;
		Output.rgb = LightIntensityParam * ProjLightColor;
		Output.a = pow(dot(Normal, HalfVector), SpecularPower);
		
		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;
		
		ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
		ZELBuffer_SetSpecular(LBuffer, Output.a);
	}
	
	return LBuffer;
}