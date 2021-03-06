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
#include "ZERNShading.hlsl"
#include "ZERNDecodeNormals.hlsl"

// SHADER RESOURCES (CONSTANT BUFFERS)
///////////////////////////////////////////////////////////////////////////////

cbuffer ZERNFixedMaterial_Constants								: register(ZERN_SHADER_CONSTANT_MATERIAL)
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
	bool			ZERNFixedMaterial_DitheredOpacityEnabled;

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

cbuffer ZERNFixedMaterial_Constant_Draw_Transform				: register(ZERN_SHADER_CONSTANT_DRAW_TRANSFORM)
{
	float4x4		ZERNFixedMaterial_WorldTransform;
	float4x4		ZERNFixedMaterial_WorldTransformInverseTranspose;
	float4			ZERNFixedMaterial_ClippingPlane0;
	float4			ZERNFixedMaterial_ClippingPlane1;
	float4			ZERNFixedMaterial_ClippingPlane2;
	float4			ZERNFixedMaterial_ClippingPlane3;
};

cbuffer ZERNFixedMaterial_Constant_Draw_Material				: register(ZERN_SHADER_CONSTANT_DRAW_MATERIAL)
{
	float4			ZERNFixedMaterial_Draw_Color;
	
	float3			ZERNFixedMaterial_Draw_Reserved;
	bool			ZERNFixedMaterial_Draw_LODTransition;
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

static const float ThresholdMatrix[8][8] =
{
	{ 0.0f / 64.0f, 32.0f / 64.0f, 8.0f / 64.0f, 40.0f / 64.0f, 2.0f / 64.0f, 34.0f / 64.0f, 10.0f / 64.0f, 42.0f / 64.0f}, 
	{48.0f / 64.0f, 16.0f / 64.0f, 56.0f / 64.0f, 24.0f / 64.0f, 50.0f / 64.0f, 18.0f / 64.0f, 58.0f / 64.0f, 26.0f / 64.0f}, 
	{12.0f / 64.0f, 44.0f / 64.0f, 4.0f / 64.0f, 36.0f / 64.0f, 14.0f / 64.0f, 46.0f / 64.0f, 6.0f / 64.0f, 38.0f / 64.0f}, 
	{60.0f / 64.0f, 28.0f / 64.0f, 52.0f / 64.0f, 20.0f / 64.0f, 62.0f / 64.0f, 30.0f / 64.0f, 54.0f / 64.0f, 22.0f / 64.0f}, 
	{ 3.0f / 64.0f, 35.0f / 64.0f, 11.0f / 64.0f, 43.0f / 64.0f, 1.0f / 64.0f, 33.0f / 64.0f, 9.0f / 64.0f, 41.0f / 64.0f}, 
	{51.0f / 64.0f, 19.0f / 64.0f, 59.0f / 64.0f, 27.0f / 64.0f, 49.0f / 64.0f, 17.0f / 64.0f, 57.0f / 64.0f, 25.0f / 64.0f},
	{15.0f / 64.0f, 47.0f / 64.0f, 7.0f / 64.0f, 39.0f / 64.0f, 13.0f / 64.0f, 45.0f / 64.0f, 5.0f / 64.0f, 37.0f / 64.0f},
	{63.0f / 64.0f, 31.0f / 64.0f, 55.0f / 64.0f, 23.0f / 64.0f, 61.0f / 64.0f, 29.0f / 64.0f, 53.0f / 64.0f, 21.0f / 64.0f} 
};

//static const float4x4 ThresholdMatrix =
//{
//	1.0 / 17.0,  9.0 / 17.0,  3.0 / 17.0, 11.0 / 17.0,
//	13.0 / 17.0,  5.0 / 17.0, 15.0 / 17.0,  7.0 / 17.0,
//	4.0 / 17.0, 12.0 / 17.0,  2.0 / 17.0, 10.0 / 17.0,
//	16.0 / 17.0,  8.0 / 17.0, 14.0 / 17.0,  6.0 / 17.0
//};
	
// INPUT OUTPUTS
///////////////////////////////////////////////////////////////////////////////

struct ZERNFixedMaterial_VSInput
{
	#ifdef ZERN_FM_MODEL
		float3 Position			: POSITION0;
		
		#ifdef ZERN_FM_SKIN_TRANSFORM
			uint4 BoneIndices	: BLENDINDICES;
		#endif
		
	int2 NormalEncoded		: NORMAL0;
	int2 TangentEncoded		: TANGENT0;
		float2 Texcoord         : TEXCOORD0;
		
		#ifdef ZERN_FM_VERTEX_COLOR
			float4 Color		: COLOR0;
		#endif
		
		#ifdef ZERN_FM_SKIN_TRANSFORM
			float4 BoneWeights	: BLENDWEIGHTS;
		#endif
		
		#ifdef ZERN_FM_INSTANCING
			float4x4	WorldTransform					: TEXCOORD1;
			float4x4	WorldTransformInverseTranspose	: TEXCOORD5;
			
			float4		DrawColor						: TEXCOORD9;
			uint4		DrawLODTransition				: TEXCOORD10;
		#endif
	#else
		float3 Position			: POSITION0;
		float3 Normal			: NORMAL0;
		float3 Tangent			: TANGENT0;
		float3 Binormal			: BINORMAL0;
		float2 Texcoord         : TEXCOORD0;
	#endif
};

struct ZERNFixedMaterial_VSOutput
{
	float4 Position			: SV_Position;
	float3 Normal			: TEXCOORD0;
	float3 Binormal			: TEXCOORD1;
	float3 Tangent			: TEXCOORD2;
	float2 Texcoord			: TEXCOORD3;
	float ViewDistance		: TEXCOORD4;
	float3 CurrPosition		: TEXCOORD5;
	float3 PrevPosition		: TEXCOORD6;
	
	#ifdef ZERN_FM_FORWARD
		float3 PositionView	: TEXCOORD7;
	#endif
	
	#ifdef ZERN_FM_VERTEX_COLOR
		float4 Color		: TEXCOORD8;
	#endif
	
	#ifdef ZERN_FM_INSTANCING
		nointerpolation float4	DrawColor			: TEXCOORD9;
		nointerpolation bool	DrawLODTransition	: TEXCOORD10;
	#endif
	
	#ifdef ZERN_FM_CLIPPING_PLANES
		float4 ClipDistance	: SV_ClipDistance;
	#endif
};

struct ZERNFixedMaterial_PSInput
{
	float4 Position			: SV_Position;
	float3 Normal			: TEXCOORD0;
	float3 Binormal			: TEXCOORD1;
	float3 Tangent			: TEXCOORD2;
	float2 Texcoord			: TEXCOORD3;
	float ViewDistance		: TEXCOORD4;
	float3 CurrPosition		: TEXCOORD5;
	float3 PrevPosition		: TEXCOORD6;
	
	#ifdef ZERN_FM_FORWARD
		float3 PositionView	: TEXCOORD7;
	#endif
	
	#ifdef ZERN_FM_VERTEX_COLOR
		float4 Color		: TEXCOORD8;
	#endif
	
	#ifdef ZERN_FM_INSTANCING
		nointerpolation float4	DrawColor			: TEXCOORD9;
		nointerpolation bool	DrawLODTransition	: TEXCOORD10;
	#endif
	bool IsFrontFace		: SV_IsFrontFace;
};

struct ZERNFixedMaterial_PSOutput
{
	#ifdef ZERN_FM_DEFERRED
		ZERNGBuffer GBuffer;
	#elif defined ZERN_FM_FORWARD
		float4 Color : SV_Target0;
	#endif
};

struct ZERNFixedMaterial_ShadowMapGenerationStage_VSOutput
{
	float4 Position			: SV_Position;
	float2 Texcoord         : TEXCOORD0;
	
	#if defined ZERN_FM_INSTANCING && defined ZERN_FM_DEPTH_PREPASS
		nointerpolation float	DrawOpacity			: TEXCOORD1;
		nointerpolation bool	DrawLODTransition	: TEXCOORD2;
	#endif
	
	#ifdef ZERN_FM_CLIPPING_PLANES
		float4 ClipDistance	: SV_ClipDistance;
	#endif
};

struct ZERNFixedMaterial_ShadowMapGenerationStage_PSInput
{
	float4 Position			: SV_Position;
	float2 Texcoord         : TEXCOORD0;
	
	#if defined ZERN_FM_INSTANCING && defined ZERN_FM_DEPTH_PREPASS
		nointerpolation float	DrawOpacity			: TEXCOORD1;
		nointerpolation bool	DrawLODTransition	: TEXCOORD2;
	#endif
};

ZERNFixedMaterial_VSOutput ZERNFixedMaterial_VertexShader(ZERNFixedMaterial_VSInput Input)
{
	ZERNFixedMaterial_VSOutput Output = (ZERNFixedMaterial_VSOutput)0;
	
	#ifdef ZERN_FM_MODEL
		float3 Normal = DecodeNormal(Input.NormalEncoded);
		float3 Tangent;
		float3 Binormal;
		DecodeTangentBinormal(Input.TangentEncoded, Normal, Tangent, Binormal);
		
		#ifdef ZERN_FM_SKIN_TRANSFORM
			float4x4 SkinTransform = ZERNSkin_GetSkinTransform(Input.BoneIndices, Input.BoneWeights);
			Input.Position = mul(SkinTransform, float4(Input.Position, 1.0f)).xyz;
			Normal = mul(SkinTransform, float4(Normal, 0.0f)).xyz;
			Tangent = mul(SkinTransform, float4(Tangent, 0.0f)).xyz;
			Binormal = mul(SkinTransform, float4(Binormal, 0.0f)).xyz;
		#endif
	
		#ifdef ZERN_FM_INSTANCING
			float4 PositionWorld = mul(Input.WorldTransform, float4(Input.Position, 1.0f));
			float3 NormalWorld = mul(Input.WorldTransformInverseTranspose, float4(Normal, 0.0f)).xyz;
			float3 TangentWorld = mul(Input.WorldTransform, float4(Tangent, 0.0f)).xyz;
			float3 BinormalWorld = mul(Input.WorldTransform, float4(Binormal, 0.0f)).xyz;
			
			Output.DrawColor = Input.DrawColor;
			Output.DrawLODTransition = Input.DrawLODTransition.w;
		#else
			float4 PositionWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Input.Position, 1.0f));
			float3 NormalWorld = mul(ZERNFixedMaterial_WorldTransformInverseTranspose, float4(Normal, 0.0f)).xyz;
			float3 TangentWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Tangent, 0.0f)).xyz;
			float3 BinormalWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Binormal, 0.0f)).xyz;
		#endif
	#else
		float4 PositionWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Input.Position, 1.0f));
		float3 NormalWorld = mul(ZERNFixedMaterial_WorldTransformInverseTranspose, float4(Input.Normal, 0.0f)).xyz;
		float3 TangentWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Input.Tangent, 0.0f)).xyz;
		float3 BinormalWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Input.Binormal, 0.0f)).xyz;
	#endif
	
	Output.Position = ZERNTransformations_WorldToProjection(PositionWorld);
	Output.Normal = ZERNTransformations_WorldToView(float4(NormalWorld, 0.0f));
	Output.Tangent = ZERNTransformations_WorldToView(float4(TangentWorld, 0.0f));
	Output.Binormal = ZERNTransformations_WorldToView(float4(BinormalWorld, 0.0f));
	Output.Texcoord = Input.Texcoord;
	Output.ViewDistance = length(PositionWorld.xyz - ZERNView_Position);
	Output.CurrPosition = Output.Position.xyw;
	Output.PrevPosition = ZERNTransformations_WorldToPrevProjection(PositionWorld).xyw;
	
	Output.CurrPosition.xy *= float2(0.5f, -0.5f);
	Output.PrevPosition.xy *= float2(0.5f, -0.5f);
	
	#ifdef ZERN_FM_FORWARD
		Output.PositionView = ZERNTransformations_WorldToView(PositionWorld);
	#endif
	
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

ZERNShading_Surface GetSurfaceDataFromResources(ZERNFixedMaterial_PSInput Input)
{
	#ifdef ZERN_FM_INSTANCING
		float Alpha = Input.DrawColor.a * ZERNFixedMaterial_Opacity;
	#else
		float Alpha = ZERNFixedMaterial_Draw_Color.a * ZERNFixedMaterial_Opacity;
	#endif
	
	#ifdef ZERN_FM_OPACITY_MAP
		Alpha *= ZERNFixedMaterial_OpacityMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).x;
	#elif defined ZERN_FM_BASE_MAP
		Alpha *= ZERNFixedMaterial_BaseMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).w;
	#endif
	
	#ifdef ZERN_FM_DEFERRED
		if (ZERNFixedMaterial_DitheredOpacityEnabled || 
		#ifdef ZERN_FM_INSTANCING
			(Input.DrawLODTransition))
		#else
			ZERNFixedMaterial_Draw_LODTransition)
		#endif
		{
			clip(Alpha - ThresholdMatrix[floor(Input.Position.x) % 8][floor(Input.Position.y) % 8]);
			//clip(Alpha - ThresholdMatrix[floor(Input.Position.x) % 4][floor(Input.Position.y) % 4]);
		}
		#ifdef ZERN_FM_ALPHA_CULL
		else
		{
			clip(Alpha - ZERNFixedMaterial_AlphaCullLimit);
		}
		#endif
	#elif defined ZERN_FM_FORWARD
		#ifdef ZERN_FM_ALPHA_CULL
			clip(Alpha - ZERNFixedMaterial_AlphaCullLimit);
		#endif
	#endif
	
	Input.Normal = (Input.IsFrontFace) ? Input.Normal : -Input.Normal;
	float3 Normal = normalize(Input.Normal);
	float3 Tangent = normalize(Input.Tangent);
	float3 Binormal = normalize(Input.Binormal);
	#ifdef ZERN_FM_NORMAL_MAP
		float3 NormalSample = ZERNFixedMaterial_NormalMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).xyz * 2.0f - 1.0f;
		NormalSample.z = sqrt(saturate(1.0f - dot(NormalSample.xy, NormalSample.xy)));	
		Normal = normalize(NormalSample.x * Tangent + NormalSample.y * Binormal + NormalSample.z * Normal);
	#endif

	#ifdef ZERN_FM_DETAIL_NORMAL_MAP
		float3 DetailNormalSample = ZERNFixedMaterial_DetailNormalMap.Sample(ZERNFixedMaterial_DetailNormalSampler, Input.Texcoord * ZERNFixedMaterial_DetailNormalMapTiling) * 2.0f - 1.0f;
		DetailNormalSample.z = sqrt(saturate(1.0f - dot(DetailNormalSample.xy, DetailNormalSample.xy)));
		float3 DetailNormal = normalize(DetailNormalSample.x * Tangent + DetailNormalSample.y * Binormal + DetailNormalSample.z * Normal);
		DetailNormal = normalize(Normal + DetailNormal);	
	
		float DetailNormalPixelDistance = Input.ViewDistance - ZERNFixedMaterial_DetailNormalMapAttenuationStart;
		float DetailNormalAttenuation = saturate(1.0f /  (1.0f + DetailNormalPixelDistance * ZERNFixedMaterial_DetailNormalMapAttenuationFactor));
    
		Normal = normalize(lerp(Normal, DetailNormal, DetailNormalAttenuation));
	#endif

	#ifdef ZERN_FM_INSTANCING
		float3 BaseColor = Input.DrawColor.rgb;
	#else
		float3 BaseColor = ZERNFixedMaterial_Draw_Color.rgb;
	#endif
	
	#ifdef ZERN_FM_BASE_MAP
		BaseColor *= ZERNFixedMaterial_BaseMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).rgb;
	#endif

	#ifdef ZERN_FM_DETAIL_BASE_MAP
		float3 DetailBaseColor = ZERNFixedMaterial_DetailBaseMapColor * ZERNFixedMaterial_DetailBaseMap.Sample(ZERNFixedMaterial_DetailBaseSampler, Input.Texcoord * ZERNFixedMaterial_DetailBaseMapTiling);
		
		float DetailBasePixelDistance = Input.ViewDistance - ZERNFixedMaterial_DetailBaseMapAttenuationStart;
		float DetailBaseAttenuation = saturate(1.0f /  (1.0f + DetailBasePixelDistance * ZERNFixedMaterial_DetailBaseMapAttenuationFactor));

		BaseColor = lerp(BaseColor, DetailBaseColor * BaseColor, DetailBaseAttenuation);
	#endif
	
	float AmbientWeight = dot(normalize(ZERNTransformations_ViewToWorld(float4(Normal, 0.0f))), float3(0.0f, 1.0f, 0.0f)) * 0.5f + 0.5f;
	
	float3 AmbientColor = BaseColor * (ZERNFixedMaterial_SceneAmbientEnabled ? lerp((float3)0.2f * dot(ZERNScene_AmbientColor, (float3)0.33f), ZERNScene_AmbientColor, AmbientWeight) : ZERNFixedMaterial_AmbientColor);
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
		SubsurfaceScattering = ZERNFixedMaterial_SubsurfaceScatteringMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).r;
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
		
	#ifdef ZERN_FM_VERTEX_COLOR
		AmbientColor *= Input.Color.rgb;
		DiffuseColor *= Input.Color.rgb;
	#endif
	
	ZERNShading_Surface Surface;
	#ifdef ZERN_FM_FORWARD
		Surface.PositionView = Input.PositionView;
	#endif
	Surface.NormalView = Normal;
	Surface.Diffuse = DiffuseColor;
	Surface.SubsurfaceScattering = SubsurfaceScattering;
	Surface.Specular = SpecularColor;
	Surface.SpecularPower = SpecularPower;
	Surface.Ambient = AmbientColor;
	Surface.Opacity = Alpha;
	Surface.Emissive = EmissiveColor;
	
	return Surface;
}

ZERNFixedMaterial_PSOutput ZERNFixedMaterial_PixelShader(ZERNFixedMaterial_PSInput Input)
{
	ZERNFixedMaterial_PSOutput Output;
	
	ZERNShading_Surface Surface = GetSurfaceDataFromResources(Input);
	
	#ifdef ZERN_FM_DEFERRED
		Input.CurrPosition.xy /= Input.CurrPosition.z;
		Input.PrevPosition.xy /= Input.PrevPosition.z;
		
		ZERNGBuffer GBuffer = (ZERNGBuffer)0;
		ZERNGBuffer_SetAlphaToCoverage(GBuffer, Surface.Opacity);
		ZERNGBuffer_SetAccumulationColor(GBuffer, Surface.Ambient + Surface.Emissive);
		ZERNGBuffer_SetViewNormal(GBuffer, Surface.NormalView);
		ZERNGBuffer_SetSpecularColor(GBuffer, Surface.Specular);
		ZERNGBuffer_SetDiffuseColor(GBuffer, Surface.Diffuse);
		ZERNGBuffer_SetSubsurfaceScattering(GBuffer, Surface.SubsurfaceScattering);
		ZERNGBuffer_SetVelocity(GBuffer, Input.CurrPosition.xy - Input.PrevPosition.xy);
		ZERNGBuffer_SetSpecularPower(GBuffer, Surface.SpecularPower);
		
		Output.GBuffer = GBuffer;
	#elif defined ZERN_FM_FORWARD
		Output.Color.rgb = Surface.Ambient + Surface.Emissive + ZERNShading_Shade(Input.Position.xy, Surface);
		Output.Color.a = Surface.Opacity;
		
		float4 FogColor = ZERNShading_CalculateFogColor(Surface.PositionView);
		Output.Color.rgb = lerp(Output.Color.rgb, FogColor.rgb, FogColor.a);
	#endif
	
	return Output;
}

// SHADOW MAP GENERATION STAGE - VERTEX SHADER
///////////////////////////////////////////////////////////////////////////////

ZERNFixedMaterial_ShadowMapGenerationStage_VSOutput ZERNFixedMaterial_ShadowMapGenerationStage_VertexShader_Main(ZERNFixedMaterial_VSInput Input)
{
	ZERNFixedMaterial_ShadowMapGenerationStage_VSOutput Output = (ZERNFixedMaterial_ShadowMapGenerationStage_VSOutput)0;
	
	#ifdef ZERN_FM_MODEL
		#ifdef ZERN_FM_SKIN_TRANSFORM
			float4x4 SkinTransform = ZERNSkin_GetSkinTransform(Input.BoneIndices, Input.BoneWeights);
			Input.Position = mul(SkinTransform, float4(Input.Position, 1.0f)).xyz;
		#endif
		
		#ifdef ZERN_FM_INSTANCING
			float4 PositionWorld = mul(Input.WorldTransform, float4(Input.Position, 1.0f));
			#ifdef ZERN_FM_DEPTH_PREPASS
				Output.DrawOpacity = Input.DrawColor.a;
				Output.DrawLODTransition = Input.DrawLODTransition.w;
			#endif
		#else
			float4 PositionWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Input.Position, 1.0f));
		#endif
	#else
		float4 PositionWorld = mul(ZERNFixedMaterial_WorldTransform, float4(Input.Position, 1.0f));
	#endif
	
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

float4 ZERNFixedMaterial_ShadowMapGenerationStage_PixelShader_Main(ZERNFixedMaterial_ShadowMapGenerationStage_PSInput Input) : SV_Target0
{
	float Alpha = ZERNFixedMaterial_Opacity;
	#ifdef ZERN_FM_DEPTH_PREPASS
		#ifdef ZERN_FM_INSTANCING
			Alpha *= Input.DrawOpacity;
		#else
			Alpha *= ZERNFixedMaterial_Draw_Color.a;
		#endif
	#endif
	
	#ifdef ZERN_FM_OPACITY_MAP
		Alpha *= ZERNFixedMaterial_OpacityMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).x;
	#elif defined ZERN_FM_BASE_MAP
		Alpha *= ZERNFixedMaterial_BaseMap.Sample(ZERNFixedMaterial_TextureSampler, Input.Texcoord).w;
	#endif
	
	#ifdef ZERN_FM_DEPTH_PREPASS
		if (ZERNFixedMaterial_DitheredOpacityEnabled || 
		#ifdef ZERN_FM_INSTANCING
			(Input.DrawLODTransition))
		#else
			(ZERNFixedMaterial_Draw_LODTransition))
		#endif
		{
			clip(Alpha - ThresholdMatrix[floor(Input.Position.x) % 8][floor(Input.Position.y) % 8]);
			//clip(Alpha - ThresholdMatrix[floor(Input.Position.x) % 4][floor(Input.Position.y) % 4]);
		}
		#if defined ZERN_FM_ALPHA_CULL
		else
		{
			clip(Alpha - ZERNFixedMaterial_AlphaCullLimit);
		}
		#endif
	#endif
	
	#if defined ZERN_FM_ALPHA_CULL
		clip(Alpha - ZERNFixedMaterial_AlphaCullLimit);
	#endif
	
	return float4(0.0f, 0.0f, 0.0f, Alpha);
}

#endif
