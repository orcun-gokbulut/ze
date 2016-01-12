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
float4x4 InvProjMatrix			: register(vs, c0);
float4x4 WorldViewMatrix		: register(vs, c4);
float4x4 WorldViewProjMatrix	: register(vs, c8);

// Light Parameters
float4 LightParameters0			: register(ps, c0);
float4 LightParameters1			: register(ps, c1);
float4 LightParameters2			: register(ps, c2);
float4 LightParameters3			: register(ps, c3);
float4 ScreenToTextureParams	: register(ps, c5);

#define LightPositionParam			LightParameters0.xyz
#define LightRange					LightParameters0.w
#define LightColorParam				LightParameters1.xyz
#define LightIntensityParam			LightParameters1.w
#define LightAttenuationParam		LightParameters2.xyz
#define ShadowVisibleDistance		LightParameters2.w
#define LightDirectionParam			LightParameters3.xyz
#define LightFOVParam				LightParameters3.w

float3x3 LightRotationParam			: register(ps, c12);
float4x4 LightProjectionMatrixParam : register(ps, c16);

sampler2D ProjectionMap				: register(ps, s4);
samplerCUBE OmniProjectionMap		: register(ps, s5);

// Directional Light parameters
// ------------------------------------------------------
#define ZE_MAX_CASCADE_COUNT	4

bool CastShadow									: register(ps, b0);
float4x4 LightTransforms[ZE_MAX_CASCADE_COUNT]	: register(ps, c90);

float3 CascadeData[ZE_MAX_CASCADE_COUNT]		: register(ps, c118);
#define	CASCADE_FAR_Z(Index)					CascadeData[Index].x
#define	CASCADE_NEAR_Z(Index)					CascadeData[Index].y
#define	CASCADE_DEPTH(Index)					CascadeData[Index].z

float4 ShadowParameters0						: register(ps, c124);
float4 ShadowParameters1						: register(ps, c125);
float4 ShadowParameters2						: register(ps, c126);
#define DepthScaledBias							ShadowParameters0.x
#define SlopeScaledBias							ShadowParameters0.y
#define ShadowDistance							ShadowParameters0.z
#define ShadowFadeDistance						ShadowParameters0.w
#define PenumbraScale							ShadowParameters1.x
#define ShadowMapTexelSize						ShadowParameters1.y
#define RotationMapTexelSize					ShadowParameters1.z
#define ShadowTransparencyFactor				ShadowParameters2.x

sampler2D ShadowMaps[ZE_MAX_CASCADE_COUNT]		: register(ps, s6);
sampler2D RandomRotationMap						: register(ps, s15);
// ------------------------------------------------------

#include "Shadow.hlsl"
#include "GBuffer.hlsl"
#include "LBuffer.hlsl"

// Point Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ZEPointLight_VSOutput
{
	float4 Position		: POSITION0;
	float3 ViewVector	: TEXCOORD0;
};

ZEPointLight_VSOutput ZEPointLight_VertexShader(in float4 Position : POSITION0)
{
	ZEPointLight_VSOutput Output;
	
	Output.Position = mul(WorldViewProjMatrix, Position);
	Output.ViewVector = ZEGBuffer_GetViewVector(mul(WorldViewMatrix, Position));
	
	return Output;
}

struct ZEPointLight_PSInput
{
	float4 ScreenPosition	: VPOS;
	float3 ViewVector		: TEXCOORD0;
};

ZELBuffer ZEPointLight_PixelShader(ZEPointLight_PSInput Input) : COLOR0
{
	ZELBuffer LBuffer = (ZELBuffer)0;
	
	float2 ScreenPosition = Input.ScreenPosition.xy * ScreenToTextureParams.xy + ScreenToTextureParams.zw;		
	float3 Position = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);

	float3 LightDisplacement = LightPositionParam - Position;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;
	
	float4 Output = 0;
	float AngularAttenuation = dot(LightDirection, Normal);
	float DistanceAttenuation = 1.0f / dot(LightAttenuationParam, float3(1.0f, LightDistance, LightDistance * LightDistance));

	if (AngularAttenuation > 0.0f)
	{
		float3 ViewDirection = normalize(-Position);
		float3 HalfVector = normalize(LightDirection + ViewDirection);		
		float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);

		Output.a = pow(dot(Normal, HalfVector), SpecularPower);
		Output.rgb = LightColorParam;

		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;
		Output *= LightIntensityParam;
	}
	else
	{
		float SubSurfaceScatteringFactor = ZEGBuffer_GetSubSurfaceScatteringFactor(ScreenPosition);
		if (SubSurfaceScatteringFactor > 0.0f)
		{			
			Output.a = 0.0f;
			Output.rgb = LightColorParam;
			
			Output.rgb *= -AngularAttenuation;
			Output.rgb *= DistanceAttenuation;
			Output.rgb *= LightIntensityParam;
			Output.rgb *= SubSurfaceScatteringFactor;
		}
	}
	
	ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
	ZELBuffer_SetSpecular(LBuffer, Output.a);
		
	return LBuffer;
}

// Directional Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ZEDirectionalLight_VSOutput
{
	float4 Position 	: POSITION0;
	float3 ViewVector 	: TEXCOORD0;
};

ZEDirectionalLight_VSOutput ZEDirectionalLight_VertexShader(float4 Position : POSITION0)
{
	ZEDirectionalLight_VSOutput Output;
	
	// Clip space, screen aligned quad positions.
	Output.Position = Position;

	// Transfrom clip space positions to view space with InvProjMatrix
	Output.ViewVector = ZEGBuffer_GetViewVector(mul(InvProjMatrix, Position));
	
	return Output;
}

struct ZEDirectionalLight_PSInput
{
	float3 ViewVector 		: TEXCOORD0;
	float4 ScreenPosition 	: VPOS;
};

float ZEDirectionalLight_CalculateFilterScale(float ViewDepth, int CascadeIndex)
{
	float FilterScale = (PenumbraScale / pow(2.0f, (float)CascadeIndex)) * (1.0f - (CascadeIndex / ZE_MAX_CASCADE_COUNT));
	//float FilterScale = PenumbraScale * (CASCADE_DEPTH(CascadeIndex) / ShadowDistance);
	return clamp(FilterScale, 0.4f, PenumbraScale);
}

float ZEDirectionalLight_GetShadowFactor(float3 PixelViewPos, float3 PixelViewNormal, float2 ScreenPos, float AngularAttenuation)
{
	float ShadowFactor = 0.0f;
	if (CastShadow)
	{
		float DepthFactor = PixelViewPos.z / ShadowDistance;
		float SlopeFactor = 1.0f - saturate(AngularAttenuation);
		float NormalOffset = SlopeFactor *  SlopeScaledBias + DepthFactor * DepthScaledBias;
		float3 PixelBiasedPos = PixelViewPos + PixelViewNormal * NormalOffset;
		
		for (int CascadeN = 0; CascadeN < ZE_MAX_CASCADE_COUNT; CascadeN++)
		{
			if (PixelViewPos.z <= CASCADE_FAR_Z(CascadeN))
			{
				float4 TexSpacePos = mul(LightTransforms[CascadeN], float4(PixelBiasedPos, 1.0f));
				float FilterScale = ZEDirectionalLight_CalculateFilterScale(PixelViewPos.z, CascadeN);
				float2 RotationMapCoord = RotationMapTexelSize * ScreenPos;
	
				ShadowFactor = SampleShadowMap(ShadowMaps[CascadeN], TexSpacePos.xy, ShadowMapTexelSize, TexSpacePos.z, RandomRotationMap, RotationMapCoord, FilterScale);
				break;
			}
		}
		
		// Calculate fade out factor
		float FadeFactor = (ShadowDistance - PixelViewPos.z) / ShadowFadeDistance;
		ShadowFactor *= saturate(FadeFactor) * ShadowTransparencyFactor;	
	}
	
	return ShadowFactor;
}

ZELBuffer ZEDirectionalLight_PixelShader(ZEDirectionalLight_PSInput Input) : COLOR0
{
	ZELBuffer LBuffer = (ZELBuffer)0.0f;
	
	float2 ScreenPosition = Input.ScreenPosition.xy * ScreenToTextureParams.xy + ScreenToTextureParams.zw;		
	float3 Position = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);
	float AngularAttenuation = dot(LightDirectionParam, Normal);
	
	float ShadowFactor = ZEDirectionalLight_GetShadowFactor(Position, Normal, Input.ScreenPosition.xy, AngularAttenuation);

	float4 Output = (float4)0.0f;
	if (AngularAttenuation > 0.0f)
	{
		float3 ViewDirection = normalize(-Position);
		float3 HalfVector = normalize(LightDirectionParam + ViewDirection);
		float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);

		Output.a = pow(dot(Normal, HalfVector), SpecularPower);
		Output.rgb = LightColorParam;

		Output *= AngularAttenuation;
		Output *= LightIntensityParam;
		Output *= (1.0f - ShadowFactor);
	}
	else
	{
		float SubSurfaceScatteringFactor = ZEGBuffer_GetSubSurfaceScatteringFactor(ScreenPosition);
		if (SubSurfaceScatteringFactor > 0.0f)
		{
			Output.a = 0.0f;
			Output.rgb = LightColorParam;
			
			Output *= -AngularAttenuation;
			Output *= LightIntensityParam;
			Output *= (1.0f - ShadowFactor);
			Output *= SubSurfaceScatteringFactor;
		}
	}
	
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
	
	Output.Position = mul(WorldViewProjMatrix, Position);
	
	Output.ViewVector = ZEGBuffer_GetViewVector(mul(WorldViewMatrix, Position));
	
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

	float2 ScreenPosition = Input.ScreenPosition.xy * ScreenToTextureParams.xy + ScreenToTextureParams.zw;			
	float3 Position = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);

	float3 LightDisplacement = LightPositionParam - Position;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;

	float4 TextureLookup = mul(LightProjectionMatrixParam, float4(Position, 1.0f));
	float2 SampleCoord = TextureLookup.xy / TextureLookup.w;
	if (SampleCoord.x < 0.0f || SampleCoord.x > 1.0f) discard;
	if (SampleCoord.y < 0.0f || SampleCoord.y > 1.0f) discard;
	float3 ProjLightColor = tex2Dlod(ProjectionMap, float4(SampleCoord, 0.0f, 0.0f)).rgb;

	float4 Output = 0;
	float AngularAttenuation = dot(LightDirection, Normal);
	float DistanceAttenuation = 1.0f / dot(LightAttenuationParam, float3(1.0f, LightDistance, LightDistance * LightDistance));

	if (AngularAttenuation > 0.0f)
	{
		float3 ViewDirection = normalize(-Position);
		float3 HalfVector = normalize(LightDirection + ViewDirection);
		float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);

		Output.a = pow(dot(Normal, HalfVector), SpecularPower);
		Output.rgb = ProjLightColor * LightColorParam;
		
		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;
		Output *= LightIntensityParam;
	}
	else
	{
		float SubSurfaceScatteringFactor = ZEGBuffer_GetSubSurfaceScatteringFactor(ScreenPosition);
		if (SubSurfaceScatteringFactor > 0.0f)
		{
			Output.a = 0.0f;
			Output.rgb = ProjLightColor * LightColorParam;
			
			Output *= -AngularAttenuation;
			Output *= DistanceAttenuation;
			Output *= LightIntensityParam;
			Output *= SubSurfaceScatteringFactor;
		}
	}
	
	ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
	ZELBuffer_SetSpecular(LBuffer, Output.a);

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
	
	Output.Position = mul(WorldViewProjMatrix, Position);
	Output.ViewVector = ZEGBuffer_GetViewVector(mul(WorldViewMatrix, Position));
	
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
	
	float2 ScreenPosition = Input.ScreenPosition.xy * ScreenToTextureParams.xy + ScreenToTextureParams.zw;		
	float3 Position = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);

	float3 LightDisplacement = LightPositionParam - Position;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;
	
	float3 TextureLookup = mul((float3x3)LightRotationParam, LightDirection);
	float3 ProjLightColor = texCUBE(OmniProjectionMap, TextureLookup).rgb;

	float4 Output;
	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	float DistanceAttenuation = 1.0f / dot(LightAttenuationParam, float3(1.0f, LightDistance, LightDistance * LightDistance));

	if (AngularAttenuation > 0.0f)
	{			
		float3 ViewDirection = normalize(-Position);
		float3 HalfVector = normalize(LightDirection + ViewDirection);		
		float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);

		Output.a = pow(dot(Normal, HalfVector), SpecularPower);
		Output.rgb = ProjLightColor * LightColorParam;
		
		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;
		Output *= LightIntensityParam;
	}
	else
	{
		float SubSurfaceScatteringFactor = ZEGBuffer_GetSubSurfaceScatteringFactor(ScreenPosition);
		if (SubSurfaceScatteringFactor > 0.0f)
		{
			Output.a = 0.0f;
			Output.rgb = ProjLightColor * LightColorParam;

			Output *= -AngularAttenuation;
			Output *= DistanceAttenuation;
			Output *= LightIntensityParam;
			Output *= SubSurfaceScatteringFactor;
		}
	}
		
	ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
	ZELBuffer_SetSpecular(LBuffer, Output.a);
	
	return LBuffer;
}
