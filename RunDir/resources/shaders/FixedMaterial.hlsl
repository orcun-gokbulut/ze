//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - FixedMaterial.hlsl
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

#include "FixedShaderComponents.hlsl"

//* Vertex Shader Constants
/////////////////////////////////////////////////////////////////////////////////////////

// Vertex Transformation
float4x4 WorldViewProjMatrix : register(vs, c0);
float4x4 WorldViewMatrix : register(vs, c4);
float4x4 WorldViewInvTrpsMatrix : register(vs, c8);

// Other general constants 4 vectors
float MaterialRefractionIndex : register(vs, c16);


// Pixel Shader Constants
/////////////////////////////////////////////////////////////////////////////////////////

// Fixed Material Properties
float4 MaterialParams0 : register(ps, c0);
float4 MaterialParams1 : register(ps, c1);
float4 MaterialParams2 : register(ps, c2);
float4 MaterialParams3 : register(ps, c3);
float4 MaterialParams4 : register(ps, c4);
float4 MaterialParams5 : register(ps, c5);
float4 MaterialParams6 : register(vs, c12);
float FarZ : register(c6);

#define	MaterialAmbientColor        MaterialParams0.xyz
#define	MaterialOpacity				MaterialParams0.w
#define	MaterialDiffuseColor        MaterialParams1.xyz
#define	MaterialSpecularColor       MaterialParams2.xyz
#define	MaterialSpecularFactor		MaterialParams2.w
#define	MaterialEmmisiveColor       MaterialParams3.xyz
#define	MaterialEmmisiveFactor		MaterialParams3.w
#define	MaterialReflectionFactor	MaterialParams4.x
#define	MaterialRefractionFactor    MaterialParams4.y
#define	MaterialDetailMapTiling     MaterialParams4.zw
#define MaterialDistortionFactor	MaterialParams5.x
#define MaterialDistortionAmount	MaterialParams5.y
#define MaterialRefractionIndex		MaterialParams6.x

// Textures
/////////////////////////////////////////////////////////////////////////////////////////

// Pipeline Textures
sampler2D LightBuffer;
sampler2D SSAOBuffer;

// Material Textures
sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
sampler2D ParallaxMap : register(s2);
sampler2D SpecularMap : register(s3);
sampler2D OpacityMap : register(s4);
sampler2D DetailDiffuseMap : register(s5);
sampler2D DetailNormalMap : register(s6);
sampler2D EmmisiveMap : register(s7);
sampler2D ReflectionMap : register(s8);
sampler2D RefractionMap : register(s9);
sampler2D LightMap : register(s10);
sampler2D DistortionMap : register(s11);


struct VSInput 
{
	float4 Position             : POSITION0;
	float3 Normal               : NORMAL0;
	float2 Texcoord             : TEXCOORD0;

	#ifdef ZE_SHADER_SKINTRANSFORM
		int4 BoneIndices        : BLENDINDICES0;
		float4 BoneWeights      : BLENDWEIGHT0;
	#endif
};

#include "SkinTransform.hlsl"

// Pre Z Pass
/////////////////////////////////////////////////////////////////////////////////////////

struct PreZVSOutput
{
	float4 Position : POSITION0;
};

PreZVSOutput PreZVSMain(VSInput Input)
{
	PreZVSOutput Output;

	SkinTransform(Input);

	Output.Position = mul(Input.Position, WorldViewProjMatrix);
	
	return Output;
}

float4 PreZPSMain() : COLOR0
{
	return 0.0f;
}

// G-Buffer Pass
/////////////////////////////////////////////////////////////////////////////////////////

// Vertex Shader
struct GBVSInput 
{
	float4 Position             : POSITION0;
	float3 Normal               : NORMAL0;
	float2 Texcoord             : TEXCOORD0;

	#ifdef ZE_SHADER_SKINTRANSFORM
		int4 BoneIndices        : BLENDINDICES0;
		float4 BoneWeights      : BLENDWEIGHT0;
	#endif
};

struct GBVSOutput 
{
	float4 Position : POSITION0;
	float4 NormalDepth : TEXCOORD0;
	#if defined(ZE_SHADER_TEXCOORD)
		float2 Texcoord : TEXCOORD1;
	#endif
	
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Tangent : TEXCOORD2;
		float3 Binormal : TEXCOORD3;
	#endif
};


GBVSOutput GBVSMain(GBVSInput Input)
{
	GBVSOutput Output;

	SkinTransform(Input);

	// Pipeline 
	Output.Position = mul(Input.Position, WorldViewProjMatrix);
	Output.NormalDepth.xyz = mul(Input.Normal, WorldViewInvTrpsMatrix).xyz;
	#ifdef ZE_SHADER_TANGENT_SPACE
		Output.Tangent = mul(Input.Tangent, WorldViewInvTrpsMatrix).xyz;
		Output.Binormal = mul(Input.Binormal, WorldViewInvTrpsMatrix).xyz;
	#endif
	Output.NormalDepth.w = mul(Input.Position, WorldViewMatrix).z;

	#if defined(ZE_SHADER_TEXCOORD)
		Output.Texcoord = Input.Texcoord;
	#endif

	return Output;
}

// Pixel Shader
struct GBPSInput
{
	float4 NormalDepth : TEXCOORD0;
	#if defined(ZE_SHADER_SPECULAR_GLOSS_MAP) || defined(ZE_SHADER_NORMAL_MAP)
		float2 Texcoord : TEXCOORD1;
	#endif
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Tangent : TEXCOORD2;
		float3 Binormal : TEXCOORD3;
	#endif
};

struct GBPSOutput
{
	float4 NormalGloss : COLOR0;
	float4 Depth : COLOR1;
};

void GBDoParallax(inout GBPSInput Input)
{
	// Trace Ray
	// Recalculate Depth and Texcoord
}

float3 GBGetNormal(in GBPSInput Input)
{
	#ifdef ZE_SHADER_TANGENT_SPACE
		float3 Normal = tex2D(NormalMap, Input.Texcoord);
		Normal = normalize(Normal.x * Input.Tangent + Normal.y * Input.Binormal + Normal.z * Input.Normal);
		return Normal * 0.5f + 0.5f;	
	#else
		return Input.NormalDepth.xyz * 0.5f + 0.5f;
	#endif	
}

float GBGetGloss(in GBPSInput Input)
{
	#ifdef ZE_SHADER_SPECULAR_GLOSS_MAP
		return tex2D(GlossMap, Input.Texcoord).x;
	#else
		return 0.0f;
	#endif
}

float GBGetDepth(in GBPSInput Input)
{
	return Input.NormalDepth.w / FarZ;
}

GBPSOutput GBPSMain(GBPSInput Input)
{
	GBPSOutput Output;
	
	GBDoParallax(Input);
	
	Output.NormalGloss.xyz = GBGetNormal(Input);
	Output.NormalGloss.w = GBGetGloss(Input);
	Output.Depth = GBGetDepth(Input);
	return Output;
}

// Material Pass
/////////////////////////////////////////////////////////////////////////////////////////

// Shadow Pass
/////////////////////////////////////////////////////////////////////////////////////////
