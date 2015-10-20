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

#include "ZERNGBuffer.hlsl"
#include "ZERNSkin.hlsl"

// DEFINITIONS
///////////////////////////////////////////////////////////////////////////////

#if defined(ZERN_FM_NORMAL_MAP)
	#define ZERN_FM_TANGENT
#endif

// SHADER RESOURCES (CONSTANT BUFFERS)
///////////////////////////////////////////////////////////////////////////////

cbuffer ZERNFixedMaterial_Constants : register(ZERN_SHADER_CONSTANT_MATERIAL)
{
	float3 	ZERNFixedMaterial_AmbientColor;
	float 	ZERNFixedMaterial_Opacity;
	float3 	ZERNFixedMaterial_DiffuseColor;
	float 	ZERNFixedMaterial_SubSurfaceScatteringFactor;
	float3 	ZERNFixedMaterial_SpecularColor;
	float 	ZERNFixedMaterial_SpecularPower;
	uint	ZERNFixedMaterial_HeightMapTechnique;
	float	ZERNFixedMaterial_HeightMapScale;
	float	ZERNFixedMaterial_HeightMapOffset;
	float	ZERNFixedMaterial_Reserved0;
	float3 	ZERNFixedMaterial_EmmisiveColor;
	float 	ZERNFixedMaterial_AlphaCullLimit;
	float3 	ZERNFixedMaterial_ReflectionColor;
	bool	ZERNFixedMaterial_GlobalAmbient;
	float3	ZERNFixedMaterial_RefractionColor;
	float	ZERNFixedMaterial_RefractionIndex;
	float3	ZERNFixedMaterial_DetailDiffuseMapColor;
	float	ZERNFixedMaterial_DetailNormalMapFactor;
	float2 	ZERNFixedMaterial_DetailDiffuseMapTiling;
	float2	ZERNFixedMaterial_DetailNormalMapTiling;
};

cbuffer ZERNFixedMaterial_Constant_Draw_Transform : register(ZERN_SHADER_CONSTANT_DRAW_TRANSFORM)
{
	float4x4 ZERNFixedMaterial_WorldTransform;
};

// SHADER RESOURCES (TEXTURES)
///////////////////////////////////////////////////////////////////////////////

Texture2D<float4> 	ZRNFixedMaterial_BaseMap			 	 : register(t0);
Texture2D<float3> 	ZRNFixedMaterial_NormalMap				 : register(t1);
Texture2D<float3> 	ZRNFixedMaterial_SpecularMap			 : register(t2);
Texture2D<float3> 	ZRNFixedMaterial_EmmisiveMap			 : register(t3);
Texture2D<float>  	ZRNFixedMaterial_HeightMap				 : register(t4);
Texture2D<float>  	ZRNFixedMaterial_OpacityMap				 : register(t5);
Texture2D<float> 	ZRNFixedMaterial_SubsurfaceScatteringMap : register(t6);
TextureCube<float3> ZRNFixedMaterial_EnvironmentMap			 : register(t7);
Texture2D<float3> 	ZRNFixedMaterial_DetailBaseeMap			 : register(t8);
Texture2D<float3> 	ZRNFixedMaterial_DetailNormalMap		 : register(t9);

SamplerState	 	ZRNFixedMaterial_TextureSampler		 	 : register(s0);
SamplerState		ZRNFixedMaterial_EnvironmentMapSampler	 : register(s1);
SamplerState	 	ZRNFixedMaterial_DetailDiffuseSampler	 : register(s2);
SamplerState	 	ZRNFixedMaterial_DetailNormalSampler	 : register(s3);



// INPUT OUTPUTS
///////////////////////////////////////////////////////////////////////////////

struct ZERNFixedMaterial_GBufferStage_VSInput 
{
	float3 Position			: POSITION0;
	float3 Normal			: NORMAL0;
	float3 Tangent			: TANGENT0;
	float2 Texcoord         : TEXCOORD0;
	
	#if defined(ZERN_FM_VERTEX_COLOR)
	float4 Color			: TEXCOORD1;
	#endif
	
	#if defined(ZERN_FM_SKIN_TRANSFORM)
		int4 BoneIndices    : BLENDINDICES0;
		float4 BoneWeights  : BLENDWEIGHT0;
	#endif
	
	#if defined(ZERN_FM_INSTANCING)
		float4x4 WorldTransform : INSTANCE0;	
	#endif
};

struct ZERNFixedMaterial_GBufferStage_VSOutput 
{
	float4 Position			: SV_Position;
	float3 ViewPosition		: TEXCOORD0;
	float3 Normal			: TEXCOORD1;
	float3 Binormal			: TEXCOORD2;
	float3 Tangent			: TEXCOORD3;
	float2 Texcoord			: TEXCOORD4;
	#if defined(ZERN_FM_VERTEX_COLOR)
		float4 Color		: TEXCOORD5;
	#endif
};

struct ZERNFixedMaterial_GBufferStage_PSInput
{
	float4 Position			: SV_Position;
	float3 ViewPosition		: TEXCOORD0;
	float3 Normal			: TEXCOORD1;
	float3 Binormal			: TEXCOORD2;
	float3 Tangent			: TEXCOORD3;
	float2 Texcoord			: TEXCOORD4;
	#if defined(ZERN_FM_VERTEX_COLOR)
		float4 Color		: TEXCOORD5;
	#endif
	//bool Side				: SV_IsFrontFace;
};


// GBUFFER STAGE - VERTEX SHADER
///////////////////////////////////////////////////////////////////////////////

ZERNFixedMaterial_GBufferStage_VSOutput ZERNFixedMaterial_GBufferStage_VertexShader(ZERNFixedMaterial_GBufferStage_VSInput Input)
{
	ZERNFixedMaterial_GBufferStage_VSOutput Output;

	//float4x4 WorldViewProjTransform = mul(ZERNView_ViewProjectionTransform, ZERNFixedMaterial_WorldTransform);
	float4x4 WorldViewProjTransform = mul(ZERNView_ProjectionTransform, ZERNView_ViewTransform);
	WorldViewProjTransform = mul(WorldViewProjTransform, ZERNFixedMaterial_WorldTransform);
	float4x4 WorldViewTransform = mul(ZERNView_ViewTransform, ZERNFixedMaterial_WorldTransform);
	
	#if defined(ZERN_FM_SKIN_TRANSFORM)
		WorldViewProjTransform = ZESkin_ApplySkinTransform(WorldViewProjTransform, Input.BoneIndices, Input.BoneTransforms);
		WorldViewTransform = ZESkin_ApplySkinTransform(WorldViewTransform, Input.BoneIndices, Input.BoneTransforms);
	#endif

	Output.Position = mul(WorldViewProjTransform, float4(Input.Position, 1.0f));
	Output.ViewPosition = mul(WorldViewTransform, float4(Input.Position, 1.0f)).xyz;
	Output.Normal = mul((float3x3)WorldViewTransform, Input.Normal);
	Output.Tangent = mul((float3x3)WorldViewTransform, Input.Tangent);
	Output.Binormal = normalize(cross(Output.Normal, Output.Tangent));
	
	Output.Texcoord = Input.Texcoord;

	#ifdef ZERN_FM_VERTEX_COLOR
		Output.Color = Color;
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
			Alpha = ZRNFixedMaterial_OpacityMap.Sample(ZRNFixedMaterial_TextureSampler, Input.Texcoord).x;
		#elif defined(ZE_FM_OPACITY_BASE_ALPHA)
			Alpha = ZRNFixedMaterial_BaseMap.Sample(ZRNFixedMaterial_TextureSampler, , Input.Texcoord).w;
		#endif
		
		if (Alpha <= ZERNFixedMaterial_OpacityCullLimit)
		{
			discard;
			return GBuffer;
		}
	#endif
	
	float3 Normal;
	#if defined(ZE_SHADER_NORMAL_MAP)
		float3 NormalSample = ZRNFixedMaterial_NormalMap.Sample(ZRNFixedMaterial_TextureSampler, Input.Texcoord) * 2.0f - 1.0f;
		Normal = normalize(NormalSample.x * Input.Tangent + NormalSample.y * Input.Binormal + NormalSample.z * Input.Normal);
	#else
		Normal = Input.Normal;
	#endif	
	
	float3 AmbientColor = ZERNFixedMaterial_AmbientColor;
	#ifdef ZERN_FM__BASE_MAP
		AmbientColor *= ZRNFixedMaterial_BaseMap.Sample(ZRNFixedMaterial_TextureSampler, Input.Texcoord).rgb;	
	#endif	

	
	float3 DiffuseColor = ZERNFixedMaterial_DiffuseColor;
	#ifdef ZERN_FM_BASE_MAP
		DiffuseColor *= BaseMap.Sample(ZRNFixedMaterial_TextureSampler, Input.Texcoord).rgb;	
	#endif	
	
	float3 SpecularColor = ZERNFixedMaterial_SpecularColor;
	#ifdef ZERN_FM_BASE_MAP
		SpecularColor *= SpecularMap.Sample(ZRNFixedMaterial_TextureSampler, Input.Texcoord).rgb;
	#endif
	
	float3 EmmisiveColor = ZERNFixedMaterial_EmmisiveColor;
	#ifdef ZERN_FM_EMMISIVE_MAP
		EmmisiveColor *= EmmisiveMap.Sample(ZRNFixedMaterial_TextureSampler, Input.Texcoord).rgb;
	#endif
	
	float SpecularPower = ZERNFixedMaterial_SpecularPower;
	#if defined(ZERN_FM_SPECULAR_GLOSS_MAP)
		SpecularPower *= GlossMap.Sample(ZRNFixedMaterial_TextureSampler, Input.Texcoord).r);
	#endif
	
	float SubsurfaceScattering = ZERNFixedMaterial_SubSurfaceScatteringFactor;
	#if defined(ZE_SHADER_SUBSURFACE_SCATTERING_MAP)
		SubSurfaceScattering = SubsurfaceScatteringMap.Sample(ZRNFixedMaterial_TextureSampler, Input.Texcoord).r);
	#endif
	
	float3 ReflectionRefractionColor = float3(0.0f, 0.0f, 0.0f);
	#ifdef ZERN_FM_REFLECTION
		float3 ReflectionDirection = normalize(reflect(float3(0.0f, 0.0f, -1.0f), Input.Normal));
		ReflectionRefractionColor += ZERNFixedMaterial_ReflectionColor * EnvironmentMap.Sample(ZRNFixedMaterial_CubeTextureSampler, ).rgb;
	#endif
		
	#ifdef ZERN_FM_REFRACTION
		float3 RefractionDirection = normalize(refract(float3(0.0f, 0.0f, -1.0f), Input.Normal, MaterialRefractionIndex));
		ReflectionRefractionColor += ZERNFixedMaterial_RefractionColor * EnvironmentMap.Sample(ZRNFixedMaterial_CubeTextureSampler, normalize(Input.RefractionVector)).rgb;
	#endif
		
	#ifdef ZE_FM_VERTEX_COLOR
		AmientColor *= Input.Color;
		DiffuseColor *= Input.Color;
		SpecularColor *= Input.Color;
	#endif
	
	float3 AccumulationColor = AmbientColor + EmmisiveColor + ReflectionRefractionColor;
	
	ZERNGBuffer_SetDepth(GBuffer, Input.ViewPosition.z);
	ZERNGBuffer_SetViewPosition(GBuffer, Input.ViewPosition.xyz);
	ZERNGBuffer_SetViewNormal(GBuffer, Normal);
	ZERNGBuffer_SetShadingModel(GBuffer,  ZERN_LM_BLINN_PONG);
	ZERNGBuffer_SetSubsurfaceScattering(GBuffer, SubsurfaceScattering);
	ZERNGBuffer_SetDiffuseColor(GBuffer, DiffuseColor);
	ZERNGBuffer_SetSpecularColor(GBuffer, SpecularColor);
	ZERNGBuffer_SetSpecularPower(GBuffer, SpecularPower);
	ZERNGBuffer_SetAccumulationColor(GBuffer, AccumulationColor);

	return GBuffer;
}
