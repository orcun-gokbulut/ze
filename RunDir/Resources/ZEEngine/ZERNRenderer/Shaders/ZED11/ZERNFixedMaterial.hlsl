//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFixedMaterial.hlsl
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

#ifndef __ZERN_FIXED_MATERIAL_H__
#define __ZERN_FIXED_MATERIAL_H__

#include "ZERNGBuffer.hlsl"
#include "ZERNSkin.hlsl"
#include "ZERNTransformations.hlsl"
#include "ZERNScene.hlsl"

// SHADER RESOURCES (CONSTANT BUFFERS)
///////////////////////////////////////////////////////////////////////////////

cbuffer ZERNFixedMaterial_Constants : register(ZERN_SHADER_CONSTANT_MATERIAL)
{
	float3			ZERNFixedMaterial_AmbientColor;
	float			ZERNFixedMaterial_Opacity;
	
	float3			ZERNFixedMaterial_DiffuseColor;
	float			ZERNFixedMaterial_SubSurfaceScatteringFactor;
	
	float3			ZERNFixedMaterial_SpecularColor;
	float			ZERNFixedMaterial_SpecularPower;
	
	uint			ZERNFixedMaterial_HeightMapTechnique;
	float			ZERNFixedMaterial_HeightMapScale;
	float			ZERNFixedMaterial_HeightMapOffset;
	float			ZERNFixedMaterial_Reserved0;

	float3			ZERNFixedMaterial_EmissiveColor;
	float			ZERNFixedMaterial_AlphaCullLimit;

	float3			ZERNFixedMaterial_ReflectionColor;
	bool			ZERNFixedMaterial_SceneAmbientEnabled;

	float3			ZERNFixedMaterial_RefractionColor;
	float			ZERNFixedMaterial_RefractionIndex;

	float3			ZERNFixedMaterial_DetailBaseMapColor;
	float			ZERNFixedMaterial_DetailNormalMapFactor;

	float2			ZERNFixedMaterial_DetailBaseMapTiling;
	float2			ZERNFixedMaterial_DetailNormalMapTiling;

	float			ZERNFixedMaterial_DetailBaseMapAttenuationStart;
	float			ZERNFixedMaterial_DetailBaseMapAttenuationFactor;
	float			ZERNFixedMaterial_DetailNormalMapAttenuationStart;
	float			ZERNFixedMaterial_DetailNormalMapAttenuationFactor;
};

cbuffer ZERNFixedMaterial_Constant_Draw_Transform : register(ZERN_SHADER_CONSTANT_DRAW_TRANSFORM)
{
	float4x4		ZERNFixedMaterial_WorldTransform;
	float4x4		ZERNFixedMaterial_WorldTransformInverseTranspose;
	float4x4		ZERNFixedMaterial_PreSkinTransform;
	float4			ZERNFixedMaterial_ClippingPlane0;
	float4			ZERNFixedMaterial_ClippingPlane1;
	float4			ZERNFixedMaterial_ClippingPlane2;
	float4			ZERNFixedMaterial_ClippingPlane3;
};

// SHADER RESOURCES (TEXTURES)
///////////////////////////////////////////////////////////////////////////////

Texture2D<float4>	ZERNFixedMaterial_BaseMap			 	 	: register(t0);
Texture2D<float3>	ZERNFixedMaterial_NormalMap				 	: register(t1);
Texture2D<float3>	ZERNFixedMaterial_SpecularMap			 	: register(t2);
Texture2D<float3>	ZERNFixedMaterial_EmissiveMap			 	: register(t3);
Texture2D<float>	ZERNFixedMaterial_HeightMap				 	: register(t4);
Texture2D<float>	ZERNFixedMaterial_OpacityMap				: register(t5);
Texture2D<float>	ZERNFixedMaterial_SubsurfaceScatteringMap 	: register(t6);
TextureCube<float3>	ZERNFixedMaterial_EnvironmentMap			: register(t7);
Texture2D<float3>	ZERNFixedMaterial_DetailBaseMap			 	: register(t8);
Texture2D<float3>	ZERNFixedMaterial_DetailNormalMap			: register(t9);
Texture2D<float>	ZERNFixedMaterial_SpecularGlossMap			: register(t10);

SamplerState		ZERNFixedMaterial_TextureSampler			: register(s0);
SamplerState		ZERNFixedMaterial_EnvironmentMapSampler		: register(s1);
SamplerState		ZERNFixedMaterial_DetailBaseSampler			: register(s2);
SamplerState		ZERNFixedMaterial_DetailNormalSampler		: register(s3);

// COMMON FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

float ZERNFixelMaterial_CalculateDetailAttenuation()
{
}

// INPUT OUTPUTS
///////////////////////////////////////////////////////////////////////////////

struct ZERNFixedMaterial_GBufferStage_VSInput
{
	float3 Position			: POSITION0;
	float3 Normal			: NORMAL0;
	float3 Tangent			: TANGENT0;
	float3 Binormal			: BINORMAL0;
	float2 Texcoord         : TEXCOORD0;
	
	#ifdef ZERN_FM_VERTEX_COLOR
		float4 Color		: COLOR0;
	#endif
	
	#ifdef ZERN_FM_SKIN_TRANSFORM
		uint4 BoneIndices	: BLENDINDICES;
		float4 BoneWeights	: BLENDWEIGHTS;
	#endif
};

struct ZERNFixedMaterial_GBufferStage_VSOutput 
{
	float4 Position			: SV_Position;
	float3 Normal			: TEXCOORD0;
	float3 Binormal			: TEXCOORD1;
	float3 Tangent			: TEXCOORD2;
	float2 Texcoord			: TEXCOORD3;
	float ViewDistance		: TEXCOORD4;
	
	#ifdef ZERN_FM_VERTEX_COLOR
		float4 Color		: TEXCOORD5;
	#endif
	
	#ifdef ZERN_FM_CLIPPING_PLANES
		float4 ClipDistance	: SV_ClipDistance;
	#endif
};

struct ZERNFixedMaterial_GBufferStage_PSInput
{
	float4 Position			: SV_Position;
	float3 Normal			: TEXCOORD0;
	float3 Binormal			: TEXCOORD1;
	float3 Tangent			: TEXCOORD2;
	float2 Texcoord			: TEXCOORD3;
	float ViewDistance		: TEXCOORD4;
	
	#ifdef ZERN_FM_VERTEX_COLOR
		float4 Color		: TEXCOORD5;
	#endif
};

struct ZERNFixedMaterial_ShadowMapGenerationStage_VSInput
{
	float3 Position			: POSITION0;
	float3 Normal			: NORMAL0;
	float3 Tangent			: TANGENT0;
	float3 Binormal			: BINORMAL0;
	float2 Texcoord         : TEXCOORD0;
	
	#ifdef ZERN_FM_SKIN_TRANSFORM
		uint4 BoneIndices	: BLENDINDICES;
		float4 BoneWeights	: BLENDWEIGHTS;
	#endif
};

struct ZERNFixedMaterial_ShadowMapGenerationStage_VSOutput
{
	float4 Position			: SV_Position;
	float2 Texcoord         : TEXCOORD0;
	#ifdef ZERN_FM_CLIPPING_PLANES
		float4 ClipDistance	: SV_ClipDistance;
	#endif
};

struct ZERNFixedMaterial_ShadowMapGenerationStage_PSInput
{
	float4 Position			: SV_Position;
	float2 Texcoord         : TEXCOORD0;
};

// GBUFFER STAGE - VERTEX SHADER
///////////////////////////////////////////////////////////////////////////////

ZERNFixedMaterial_GBufferStage_VSOutput ZERNFixedMaterial_GBufferStage_VertexShader(ZERNFixedMaterial_GBufferStage_VSInput Input)
{
	ZERNFixedMaterial_GBufferStage_VSOutput Output = (ZERNFixedMaterial_GBufferStage_VSOutput)0;
	
	#ifdef ZERN_FM_SKIN_TRANSFORM
		float4x4 SkinTransform = ZERNSkin_GetSkinTransform(Input.BoneIndices, Input.BoneWeights);
		Input.Position = mul(SkinTransform, float4(Input.Position, 1.0f)).xyz;
		Input.Normal = mul(SkinTransform, float4(Input.Normal, 0.0f)).xyz;
		Input.Tangent = mul(SkinTransform, float4(Input.Tangent, 0.0f)).xyz;
		Input.Binormal = mul(SkinTransform, float4(Input.Binormal, 0.0f)).xyz;
	#endif

	float4 PositionWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Input.Position, 1.0f));
	float3 NormalWorld = mul(ZERNFixedMaterial_WorldTransformInverseTranspose, float4(Input.Normal, 0.0f)).xyz;
	float3 TangentWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Input.Tangent, 0.0f)).xyz;
	float3 BinormalWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Input.Binormal, 0.0f)).xyz;
	
	Output.Position = ZERNTransformations_WorldToProjection(PositionWorld);
	Output.Normal = ZERNTransformations_WorldToView(float4(NormalWorld, 0.0f));
	Output.Tangent = ZERNTransformations_WorldToView(float4(TangentWorld, 0.0f));
	Output.Binormal = ZERNTransformations_WorldToView(float4(BinormalWorld, 0.0f)); //cross(Output.Tangent, Output.Normal);
	Output.Texcoord = Input.Texcoord;
	Output.ViewDistance = length(PositionWorld.xyz - ZERNView_Position);

	#ifdef ZERN_FM_VERTEX_COLOR
		Output.Color = Input.Color;
	#endif

	#ifdef ZERN_FM_CLIPPING_PLANES
		Output.ClipDistance.x = dot(PositionWorld, ZERNFixedMaterial_ClippingPlane0);
		Output.ClipDistance.y = dot(PositionWorld, ZERNFixedMaterial_ClippingPlane1);
		Output.ClipDistance.z = dot(PositionWorld, ZERNFixedMaterial_ClippingPlane2);
		Output.ClipDistance.w = dot(PositionWorld, ZERNFixedMaterial_ClippingPlane3);
	#endif
		
	return Output;
}


// GBUFFER STAGE - PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

ZERNGBuffer ZERNFixedMaterial_GBufferStage_PixelShader(ZERNFixedMaterial_GBufferStage_PSInput Input)
{
	ZERNGBuffer GBuffer = (ZERNGBuffer)0;
	
	#ifdef ZERN_FM_ALPHA_CULL
		float Alpha = ZERNFixedMaterial_Opacity;
		#ifdef ZERN_FM_OPACITY_MAP
			Alpha = ZERNFixedMaterial_OpacityMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).x;
		#elif defined(ZERN_FM_OPACITY_BASE_ALPHA)
			Alpha = ZERNFixedMaterial_BaseMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).w;
		#endif
		
		if (Alpha <= ZERNFixedMaterial_AlphaCullLimit)
		{
			discard;
			return GBuffer;
		}
	#endif
	
	float3 Normal = normalize(Input.Normal);
	float3 Tangent = normalize(Input.Tangent);
	float3 Binormal = normalize(Input.Binormal);
	#ifdef ZERN_FM_NORMAL_MAP
		float3 NormalSample = ZERNFixedMaterial_NormalMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).xyz * 2.0f - 1.0f;
		Normal = normalize(NormalSample.x * Tangent + NormalSample.y * Binormal + NormalSample.z * Normal);
	#endif

	#ifdef ZERN_FM_DETAIL_NORMAL_MAP
		float3 DetailNormalSample = ZERNFixedMaterial_DetailNormalMap.Sample(ZERNFixedMaterial_DetailNormalSampler, Input.Texcoord * ZERNFixedMaterial_DetailNormalMapTiling) * 2.0f - 1.0f;
		float3 DetailNormal = normalize(DetailNormalSample.x * Tangent + DetailNormalSample.y * Binormal + DetailNormalSample.z * Normal);
		DetailNormal = normalize(Normal + DetailNormal);	
	
		float DetailNormalPixelDistance = Input.ViewDistance - ZERNFixedMaterial_DetailNormalMapAttenuationStart;
		float DetailNormalAttenuation = saturate(1.0f /  (1.0f + DetailNormalPixelDistance * ZERNFixedMaterial_DetailNormalMapAttenuationFactor));
    
		Normal = normalize(lerp(Normal, DetailNormal, DetailNormalAttenuation));
	#endif

	float3 BaseColor = float3(1.0f, 1.0f, 1.0f);
	#ifdef ZERN_FM_BASE_MAP
		BaseColor *= ZERNFixedMaterial_BaseMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).rgb;
	#endif

	#ifdef ZERN_FM_DETAIL_BASE_MAP
		float3 DetailBaseColor = ZERNFixedMaterial_DetailBaseMapColor * ZERNFixedMaterial_DetailBaseMap.Sample(ZERNFixedMaterial_DetailBaseSampler, Input.Texcoord * ZERNFixedMaterial_DetailBaseMapTiling);
		
		float DetailBasePixelDistance = Input.ViewDistance - ZERNFixedMaterial_DetailBaseMapAttenuationStart;
		float DetailBaseAttenuation = saturate(1.0f /  (1.0f + DetailBasePixelDistance * ZERNFixedMaterial_DetailBaseMapAttenuationFactor));
    
		BaseColor = lerp(BaseColor, DetailBaseColor * BaseColor, DetailBaseAttenuation);
	#endif


	float3 AmbientColor = BaseColor * (ZERNFixedMaterial_SceneAmbientEnabled ? ZERNScene_AmbientColor : ZERNFixedMaterial_AmbientColor);
	float3 DiffuseColor = BaseColor * ZERNFixedMaterial_DiffuseColor;
	float3 SpecularColor = ZERNFixedMaterial_SpecularColor;
	float3 EmissiveColor = ZERNFixedMaterial_EmissiveColor;
	float SpecularPower = ZERNFixedMaterial_SpecularPower;

	#ifdef ZERN_FM_EMISSIVE_MAP
		EmissiveColor *= ZERNFixedMaterial_EmissiveMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).rgb;
	#endif

	#if defined(ZERN_FM_SPECULAR_MAP)
		SpecularColor *= ZERNFixedMaterial_SpecularMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).rgb;
	#endif

	#if defined(ZERN_FM_SPECULAR_GLOSS_MAP)
		SpecularPower *= ZERNFixedMaterial_SpecularGlossMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).r;
	#endif

	float SubsurfaceScattering = ZERNFixedMaterial_SubSurfaceScatteringFactor;
	#if defined(ZERN_FM_SUBSURFACE_SCATTERING_MAP)
		SubSurfaceScattering = ZERNFixedMaterial_SubsurfaceScatteringMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).r);
	#endif
	
	float3 ReflectionRefractionColor = float3(0.0f, 0.0f, 0.0f);
	#ifdef ZERN_FM_REFLECTION
		float3 ReflectionDirection = normalize(reflect(float3(0.0f, 0.0f, -1.0f), Normal));
		ReflectionRefractionColor += ZERNFixedMaterial_ReflectionColor * ZERNFixedMaterial_EnvironmentMap.Sample(ZERNFixedMaterial_EnvironmentMapSampler, normalize(Input.ReflectionVector)).rgb;
	#endif
		
	#ifdef ZERN_FM_REFRACTION
		float3 RefractionDirection = normalize(refract(float3(0.0f, 0.0f, -1.0f), Normal, ZERNFixedMaterial_RefractionIndex));
		ReflectionRefractionColor += ZERNFixedMaterial_RefractionColor * ZERNFixedMaterial_EnvironmentMap.Sample(ZERNFixedMaterial_EnvironmentMapSampler, normalize(Input.RefractionVector)).rgb;
	#endif
		
	#ifdef ZE_FM_VERTEX_COLOR
		AmbientColor *= Input.Color;
		DiffuseColor *= Input.Color;
	#endif
	
	ZERNGBuffer_SetAccumulationColor(GBuffer, AmbientColor);
	ZERNGBuffer_SetViewNormal(GBuffer, Normal);
	ZERNGBuffer_SetSpecularColor(GBuffer, SpecularColor);
	ZERNGBuffer_SetDiffuseColor(GBuffer, DiffuseColor);
	ZERNGBuffer_SetSubsurfaceScattering(GBuffer, SubsurfaceScattering);
	ZERNGBuffer_SetEmissiveColor(GBuffer, EmissiveColor);
	ZERNGBuffer_SetSpecularPower(GBuffer, SpecularPower);

	return GBuffer;
}

// SHADOW MAP GENERATION STAGE - VERTEX SHADER
///////////////////////////////////////////////////////////////////////////////

ZERNFixedMaterial_ShadowMapGenerationStage_VSOutput ZERNFixedMaterial_ShadowMapGenerationStage_VertexShader_Main(ZERNFixedMaterial_ShadowMapGenerationStage_VSInput Input)
{
	#ifdef ZERN_FM_SKIN_TRANSFORM
		float4x4 SkinTransform = ZERNSkin_GetSkinTransform(Input.BoneIndices, Input.BoneWeights);
		Input.Position = mul(SkinTransform, float4(Input.Position, 1.0f)).xyz;
	#endif
	
	float4 PositionWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Input.Position, 1.0f));
	
	ZERNFixedMaterial_ShadowMapGenerationStage_VSOutput Output;
	Output.Position = ZERNTransformations_WorldToProjection(PositionWorld);
	Output.Texcoord = Input.Texcoord;
	
	#ifdef ZERN_FM_CLIPPING_PLANES
		Output.ClipDistance.x = dot(PositionWorld, ZERNFixedMaterial_ClippingPlane0);
		Output.ClipDistance.y = dot(PositionWorld, ZERNFixedMaterial_ClippingPlane1);
		Output.ClipDistance.z = dot(PositionWorld, ZERNFixedMaterial_ClippingPlane2);
		Output.ClipDistance.w = dot(PositionWorld, ZERNFixedMaterial_ClippingPlane3);
	#endif
	
	return Output;
}

// SHADOW MAP GENERATION STAGE - PIXEL SHADER FOR ALPHA TEST
///////////////////////////////////////////////////////////////////////////////

void ZERNFixedMaterial_ShadowMapGenerationStage_PixelShader_Main(ZERNFixedMaterial_ShadowMapGenerationStage_PSInput Input)
{
	float Alpha = ZERNFixedMaterial_Opacity;
	#ifdef ZERN_FM_OPACITY_MAP
		Alpha = ZERNFixedMaterial_OpacityMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).x;
	#elif defined(ZERN_FM_OPACITY_BASE_ALPHA)
		Alpha = ZERNFixedMaterial_BaseMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).w;
	#endif
	
	clip(Alpha - ZERNFixedMaterial_AlphaCullLimit);
}

#endif
