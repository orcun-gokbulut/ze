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
#include "GBuffer.hlsl"
#include "LBuffer.hlsl"

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
float4 PipelineParamatersPS[10] : register(c0);
float4 MaterialParametersPS[20] : register(ps, c10);


#define	MaterialAmbientColor        			MaterialParametersPS[0].xyz
#define	MaterialOpacity							MaterialParametersPS[0].w
#define	MaterialDiffuseColor        			MaterialParametersPS[1].xyz
#define	MaterialSpecularColor       			MaterialParametersPS[2].xyz
#define	MaterialSpecularFactor					MaterialParametersPS[2].w
#define	MaterialEmmisiveColor       			MaterialParametersPS[3].xyz
#define	MaterialEmmisiveFactor					MaterialParametersPS[3].w
#define	MaterialReflectionFactor				MaterialParametersPS[4].x
#define	MaterialRefractionFactor    			MaterialParametersPS[4].y
#define	MaterialDetailMapTiling     			MaterialParametersPS[4].zw

/*#define MaterialDistortionFactor				MaterialParametersPS[5].x
#define MaterialDistortionAmount				MaterialParametersPS[5].y*/

#define MaterialSubSurfaceScatteringFactor		MaterialParametersPS[5].x
#define MaterialAlphaCullLimit					MaterialParametersPS[5].y

#define MaterialRefractionIndex					0
#define ScreenToTextureParams					PipelineParamatersPS[0]

bool EnableSkin : register(vs, b0);
bool ShadowReciever : register(ps, b0);

// Textures
/////////////////////////////////////////////////////////////////////////////////////////

// Pipeline Textures
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
sampler2D SubsurfaceScatteringMap : register(s16);

struct ZEFixedMaterial_VSInput 
{
	float4 Position : POSITION0;
	float3 Normal : NORMAL0;
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Binormal : BINORMAL0;
		float3 Tangent : TANGENT0;
	#endif
	
	float2 Texcoord             : TEXCOORD0;

	//#if defined(ZE_SHADER_SKIN_TRANSFORM)
		int4 BoneIndices        : BLENDINDICES0;
		float4 BoneWeights      : BLENDWEIGHT0;
	//#endif
};

#define ZE_SHADER_VERTEX_INPUT_TYPE ZEFixedMaterial_VSInput
#include "SkinTransform.hlsl"

// G-Buffer Pass
/////////////////////////////////////////////////////////////////////////////////////////

// Vertex Shader
struct ZEFixedMaterial_GBuffer_VSOutput 
{
	float4 Position_ : POSITION0;
	float3 Position : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float2 Texcoord : TEXCOORD2;
	
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Binormal : TEXCOORD3;
		float3 Tangent : TEXCOORD4;
	#endif
};


ZEFixedMaterial_GBuffer_VSOutput ZEFixedMaterial_GBuffer_VertexShader(ZEFixedMaterial_VSInput Input)
{
	ZEFixedMaterial_GBuffer_VSOutput Output;

	if (EnableSkin)
		SkinTransform(Input);

	// Pipeline 
	Output.Position_ = mul(Input.Position, WorldViewProjMatrix);
	
	Output.Position = mul(Input.Position, WorldViewMatrix).xyz;
	Output.Normal.xyz = mul(Input.Normal, WorldViewInvTrpsMatrix).xyz;
	#if defined(ZE_SHADER_TANGENT_SPACE)
		Output.Tangent = mul(Input.Tangent, WorldViewInvTrpsMatrix).xyz;
		Output.Binormal = mul(Input.Binormal, WorldViewInvTrpsMatrix).xyz;
	#endif
	
	Output.Texcoord = Input.Texcoord;
	return Output;
}

// Pixel Shader
struct ZEFixedMaterial_GBuffer_PSInput
{
	float Side : VFACE;
	float3 Position : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	#if defined(ZE_SHADER_SPECULAR_GLOSS_MAP) || defined(ZE_SHADER_NORMAL_MAP) || defined(ZE_SHADER_ALPHA_CULL)
		float2 Texcoord : TEXCOORD2;
	#endif
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Binormal : TEXCOORD3;
		float3 Tangent : TEXCOORD4;
	#endif
};

ZEGBuffer ZEFixedMaterial_GBuffer_PixelShader(ZEFixedMaterial_GBuffer_PSInput Input)
{
	ZEGBuffer GBuffer = (ZEGBuffer)0;
	
	//GBDoParallax(Input);

	
	#ifdef ZE_SHADER_ALPHA_CULL
		#ifdef ZE_SHADER_OPACITY_MAP
			float Alpha = MaterialOpacity * tex2D(OpacityMap, Input.Texcoord).r;
		#elif defined(ZE_SHADER_OPACITY_BASE_ALPHA)
			float Alpha = MaterialOpacity * tex2D(BaseMap, Input.Texcoord).a;
		#else 
			float Alpha = MaterialOpacity;
		#endif
			
		if (Alpha <= MaterialAlphaCullLimit)
		{
			discard;
			return GBuffer;
		}
	#endif
	
		
	ZEGBuffer_SetViewPosition(GBuffer, Input.Position);

	#if defined(ZE_SHADER_NORMAL_MAP)
		float3 Normal = tex2D(NormalMap, Input.Texcoord) * 2.0f - 1.0f;
		Normal = normalize(Normal.x * Input.Tangent + Normal.y * Input.Binormal + Normal.z * Input.Normal);
		ZEGBuffer_SetViewNormal(GBuffer, Normal * Input.Side);	
	#else
		ZEGBuffer_SetViewNormal(GBuffer, Input.Normal * Input.Side);
	#endif	
	
	#if defined(ZE_SHADER_SPECULAR_GLOSS_MAP)
		ZEGBuffer_SetSpecularGlossiness(GBuffer, MaterialSpecularFactor * tex2D(GlossMap, Input.Texcoord).x);
	#else
		ZEGBuffer_SetSpecularGlossiness(GBuffer, MaterialSpecularFactor);
	#endif
	
	#if defined(ZE_SHADER_SUBSURFACE_SCATTERING_MAP)
		ZEGBuffer_SetSubSurfaceScattering(GBuffer, tex2D(SubSurfaceScatteringMap, MaterialSubSurfaceScatteringFactor * Input.Texcoord).x);
	#else
		ZEGBuffer_SetSubSurfaceScatteringFactor(GBuffer, MaterialSubSurfaceScatteringFactor);
	#endif

	return GBuffer;
}

// Forward Pass
/////////////////////////////////////////////////////////////////////////////////////////
struct ZEFixedMaterial_ForwardPass_VSOutput
{
	float4 Position : POSITION0;
	float2 Texcoord : TEXCOORD0;
	
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

ZEFixedMaterial_ForwardPass_VSOutput ZEFixedMaterial_ForwardPass_VertexShader(ZEFixedMaterial_VSInput Input)
{
	ZEFixedMaterial_ForwardPass_VSOutput Output;
	
	if (EnableSkin)
		SkinTransform(Input);

	// Pipeline 
	Output.Position = mul(Input.Position, WorldViewProjMatrix);
	Output.Texcoord = Input.Texcoord;
	
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

struct ZEFixedMaterial_ForwardPass_PSOutput
{
	float4	Color : COLOR0;
};

struct ZEFixedMaterial_ForwardPass_PSInput
{
	float3 ScreenPosition : VPOS;
	float2 Texcoord : TEXCOORD0;
	
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

ZEFixedMaterial_ForwardPass_PSOutput ZEFixedMaterial_ForwardPass_PixelShader(ZEFixedMaterial_ForwardPass_PSInput Input)
{
	ZEFixedMaterial_ForwardPass_PSOutput Output;
	Output.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	#ifdef ZE_SHADER_OPACITY
		#ifdef ZE_SHADER_OPACITY_MAP
				Output.Color.a = MaterialOpacity * tex2D(OpacityMap, Input.Texcoord).r;
		#elif defined(ZE_SHADER_OPACITY_BASE_ALPHA)
				Output.Color.a = MaterialOpacity * tex2D(BaseMap, Input.Texcoord).a;
		#else 
			Output.Color.a = MaterialOpacity;
		#endif
	#endif
	
/*	#ifdef ZE_SHADER_ALPHA_CULL
		if (Output.Color.a <= MaterialAlphaCullLimit)
		{
			discard;
			return Output;
		}		
	#endif*/
	
	float2 ScreenPosition = Input.ScreenPosition * ScreenToTextureParams.xy + ScreenToTextureParams.zw;		

	#ifdef ZE_SHADER_AMBIENT
		float3 AmbientColor = MaterialAmbientColor;
		#ifdef ZE_SHADER_BASE_MAP
			AmbientColor *= tex2D(BaseMap, Input.Texcoord).rgb;
		#endif
		//#ifdef ZE_SHADER_SSAO
			//AmbientColor *= tex2D(SSAOBuffer, ScreenPosition).r;
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
	
		DiffuseColor *= ZELBuffer_GetDiffuse(ScreenPosition);
		
		Output.Color.rgb += DiffuseColor;
	#endif
	
	#ifdef ZE_SHADER_SPECULAR
		float3 SpecularColor = MaterialSpecularColor;
		#ifdef ZE_SHADER_SPECULAR_MAP
			SpecularColor *= tex2D(SpecularMap, Input.Texcoord).rgb;
		#endif
		
		SpecularColor *= ZELBuffer_GetSpecular(ScreenPosition);
		
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
		
	return Output;
}
