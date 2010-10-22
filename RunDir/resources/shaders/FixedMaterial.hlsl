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

#include "FixedMaterialComponents.hlsl"

//* Vertex Shader Constants
/////////////////////////////////////////////////////////////////////////////////////////

// Vertex Transformation
float4x4 WorldViewProjMatrix : register(vs, c0);
float4x4 WorldViewMatrix : register(vs, c4);
float4x4 WorldViewInvTrpsMatrix : register(vs, c8);
float4x4 BoneMatrices[58] : register(vs, c32);

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
float4 PipelineParameters0 : register(ps, c6);
float4 MaterialParams6 : register(vs, c12);



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

#define PixelSize_2					PipelineParameters0.xy
#define FarZ						PipelineParameters0.z

// Textures
/////////////////////////////////////////////////////////////////////////////////////////

// Pipeline Textures
sampler2D GBuffer1 : register(s0);
sampler2D GBuffer2 : register(s1);
sampler2D LBuffer1 : register(s2);
sampler2D LBuffer2 : register(s3);
sampler2D SSAOBuffer : register(s4);

// Material Textures
sampler2D BaseMap : register(s5);
sampler2D NormalMap : register(s6);
sampler2D ParallaxMap : register(s7);
sampler2D SpecularMap : register(s8);
sampler2D OpacityMap : register(s9);
sampler2D EmmisiveMap : register(s10);
sampler2D EnvironmentMap : register(s11);
sampler2D LightMap : register(s12);
sampler2D DistortionMap : register(s13);
sampler2D DetailDiffuseMap : register(s14);
sampler2D DetailNormalMap : register(s15);

struct VSInput 
{
	float4 Position : POSITION0;
	float3 Normal : NORMAL0;
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Binormal : BINORMAL0;
		float3 Tangent : TANGENT0;
	#endif
	
	float2 Texcoord             : TEXCOORD0;

	#if defined(ZE_SHADER_SKIN_TRANSFORM)
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
struct GBVSOutput 
{
	float4 Position : POSITION0;
	float4 NormalDepth : TEXCOORD0;
	float2 Texcoord : TEXCOORD1;
	float3 Position_ : TEXCOORD5;
	
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Binormal : TEXCOORD2;
		float3 Tangent : TEXCOORD3;
	#endif
};


GBVSOutput GBVSMain(VSInput Input)
{
	GBVSOutput Output;

	SkinTransform(Input);

	// Pipeline 
	Output.Position = mul(Input.Position, WorldViewProjMatrix);

	Output.NormalDepth.xyz = mul(Input.Normal, WorldViewInvTrpsMatrix).xyz;
	#if defined(ZE_SHADER_TANGENT_SPACE)
		Output.Tangent = mul(Input.Tangent, WorldViewInvTrpsMatrix).xyz;
		Output.Binormal = mul(Input.Binormal, WorldViewInvTrpsMatrix).xyz;
	#endif
	
	Output.NormalDepth.w = mul(Input.Position, WorldViewMatrix).z;
	Output.Position_ = mul(Input.Position, WorldViewMatrix).xyz;
	
	Output.Texcoord = Input.Texcoord;
	return Output;
}

// Pixel Shader
struct GBPSInput
{
	float4 NormalDepth : TEXCOORD0;
	#if defined(ZE_SHADER_SPECULAR_GLOSS_MAP) || defined(ZE_SHADER_NORMAL_MAP)
		float2 Texcoord : TEXCOORD1;
	#endif
	float3 Position: TEXCOORD5;
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Binormal : TEXCOORD2;
		float3 Tangent : TEXCOORD3;
	#endif
};

struct GBPSOutput
{
	float4 Position : COLOR0;
	float4 NormalGloss : COLOR1;
};

void GBDoParallax(inout GBPSInput Input)
{

}

float3 GBGetNormal(in GBPSInput Input)
{
	#if defined(ZE_SHADER_NORMAL_MAP)
		float3 Normal = tex2D(NormalMap, Input.Texcoord) * 2.0f - 1.0f;
		Normal = normalize(Normal.x * Input.Tangent + Normal.y * Input.Binormal + Normal.z * Input.NormalDepth.xyz);
		return Normal * 0.5f + 0.5f;	
	#else
		return Input.NormalDepth.xyz * 0.5f + 0.5f;
	#endif	
}

float GBGetGloss(in GBPSInput Input)
{
	#if defined(ZE_SHADER_SPECULAR_GLOSS_MAP)
		return tex2D(GlossMap, Input.Texcoord).x;
	#else
		return 0.0f;
	#endif
}

float GBGetDepth(in GBPSInput Input)
{
	return length(Input.Position);
	//return Input.NormalDepth.w;
}

GBPSOutput GBPSMain(GBPSInput Input)
{
	GBPSOutput Output;
	
	GBDoParallax(Input);
	
	Output.NormalGloss.xyz = GBGetNormal(Input);
	Output.NormalGloss.w = GBGetGloss(Input);
	//Output.Depth = GBGetDepth(Input);
	Output.Position = float4(Input.Position.xyz, 1.0f);
	return Output;
}



// Forward Pass
/////////////////////////////////////////////////////////////////////////////////////////
struct FPVSOutput
{
	float4 Position : POSITION0;
	float2 Texcoord : TEXCOORD0;
	float3 ScreenPosition : TEXCOORD1;
	
	#ifdef ZE_SHADER_DETAIL_MAP
	float2 DetailTexcoord : TEXCOORD1;
	#endif
	
	#ifdef ZE_SHADER_LIGHT_MAP
		float2 LightMapTexcoord     : TEXCOORD2;
	#endif
	
	#ifdef ZESHADER_REFLECTION
		float3 ReflectionVector     : TEXCOORD3;
	#endif

	#ifdef ZESHADER_REFRACTION
		float3 RefractionVector     : TEXCOORD4;
	#endif	
};

FPVSOutput FPVSMain(VSInput Input)
{
	FPVSOutput Output;
	SkinTransform(Input);

	// Pipeline 
	Output.Position = mul(Input.Position, WorldViewProjMatrix);
	Output.Texcoord = Input.Texcoord;
	
	Output.ScreenPosition.xy = float2(Output.Position.x, -Output.Position.y);
	Output.ScreenPosition.z = Output.Position.w;
	
	#ifdef ZE_SHADER_REFLECTION
		Output.ReflectionVector = reflect(-CameraDirection, Normal);
	#endif
	
	#ifdef ZE_SHADER_REFRACTION
		Output.RefractionVector = refract(-CameraDirection, Normal, MaterialRefractionIndex);
	#endif

	#ifdef ZE_SHADER_LIGHTMAP
		Output.LightMapTexcoord = Input.LightMapTexcoord;
	#endif
	
	return Output;
}

struct FPPSOutput
{
	float4	Color : COLOR0;
};

struct FPPSInput
{
	float2 Texcoord : TEXCOORD0;
	float3 ScreenPosition : TEXCOORD1;
	
	#ifdef ZE_SHADER_DETAIL_MAP
		float2 DetailTexcoord : TEXCOORD1;
	#endif
	
	#ifdef ZE_SHADER_LIGHT_MAP
		float2 LightMapTexcoord     : TEXCOORD2;
	#endif
	
	#ifdef ZESHADER_REFLECTION
		float3 ReflectionVector     : TEXCOORD3;
	#endif

	#ifdef ZESHADER_REFRACTION
		float3 RefractionVector     : TEXCOORD4;
	#endif	
};

FPPSOutput FPPSMain(FPPSInput Input)
{
	FPPSOutput Output;
	Output.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	float2 ScreenPosition = Input.ScreenPosition.xy / Input.ScreenPosition.z * 0.5f + 0.5f + PixelSize_2;

	#ifdef ZE_SHADER_AMBIENT
		float3 AmbientColor = MaterialAmbientColor;
		#ifdef ZE_SHADER_BASE_MAP
			AmbientColor *= tex2D(BaseMap, Input.Texcoord).rgb;
		#endif
		//#ifdef ZE_SHADER_SSAO
			AmbientColor *= tex2D(SSAOBuffer, ScreenPosition).r;
		//#endif
		
		#ifdef ZE_SHADER_LIGHT_MAP
			AmbientColor *= tex2D(LightMap, Input.Texcoord).rgb;
		#endif
		
		Output.Color.rgb = AmbientColor;
	#endif
	
	#ifdef ZE_SHADER_DIFFUSE
		float3 DiffuseColor = MaterialDiffuseColor;
		#ifdef ZE_SHADER_BASE_MAP
			DiffuseColor *= tex2D(BaseMap, Input.Texcoord).rgb;
		#endif
	
		DiffuseColor *= tex2D(LBuffer1, ScreenPosition).rgb;
		
		Output.Color.rgb += DiffuseColor;
	#endif
	
	#ifdef ZE_SHADER_SPECULAR
		float3 SpecularColor = MaterialSpecularColor;
		#ifdef ZE_SHADER_SPECULAR_MAP
			SpecularColor *= tex2D(SpecularMap, Input.Texcoord).rgb;
		#endif
		
		SpecularColor *= tex2D(LBuffer1, ScreenPosition).a * normalize(tex2D(LBuffer1, ScreenPosition)).rgb;
		
		Output.Color.rgb += SpecularColor;
	#endif
	
	#ifdef ZESHADER_EMMISIVE
		float3 EmmisiveColor = MaterialEmmisiveColor;
		#ifdef ZE_SHADER_EMMISIVE_MAP
			EmmisiveColor *= MaterialEmmisiveFactor * tex2D(EmmisiveMap, Input.Texcoord);
		#endif

		Output.Color.rgb += EmmisiveColor;
	#endif

	#ifdef ZE_SHADER_REFLECTION
		Output.Color.rgb += MaterialReflectionFactor * texCUBE(EnvironmentMap, normalize(Input.ReflectionVector)).rgb;
	#endif
		
	#ifdef ZE_SHADER_REFRACTION
		Output.Color.rgb += MaterialRefractionFactor * texCUBE(EnvironmentMap, normalize(Input.RefractionVector)).rgb;
	#endif
		
	#ifdef ZE_SHADER_OPASITY
		#ifdef ZE_SHADER_OPACITY_MAP
			#ifdef ZESHADER_OPACITY_CONSTANT
				Output.Color.a = MaterialOpacity * tex2D(OpacityMap, Input.Texcoord).r;
			#else
				Output.Color.a = tex2D(DiffuseMap, Input.Texcoord).r;
			#endif
		#elif defined(ZESHADER_OPACITY_DIFFUSE_ALPHA)
			#ifdef ZESHADER_OPACITY_CONSTANT
				Output.Color.a = MaterialOpacity * tex2D(DiffuseMap, Input.Texcoord).a;
			#else
				Output.Color.a = tex2D(DiffuseMap, Input.Texcoord).a;
			#endif
		#else 
			Output.Color.a = MaterialOpacity;
		#endif
	#endif
	
	return Output;
}


// Shadow Pass
/////////////////////////////////////////////////////////////////////////////////////////
