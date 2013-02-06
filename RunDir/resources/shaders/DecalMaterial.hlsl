//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - DecalMaterial.hlsl
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

#include "GBuffer.hlsl"
#include "LBuffer.hlsl"
#include "FixedMaterialComponents.hlsl"

sampler2D	DiffuseMap			: register (s4);
sampler2D	NormalMap			: register (s5);
sampler2D	SpecularMap			: register (s6);
sampler2D	OpacityMap			: register (s7);
sampler2D	EmissiveMap			: register (s8);

float4 PixelSize				: register (ps, c1);	// xy: Pixel Size, zw: Half Pizel Size
float3 CameraPosition			: register (ps, c2) 
								: register (vs, c2);

float4x4 WorldMatrix			: register (vs, c7);
float4x4 ViewProjMatrix			: register (vs, c15);
float4x4 DecalViewProjTex		: register (ps, c23);
float4x4 WorldViewInvTrps		: register (vs, c27);

float3 MaterialAmbientColor		: register (ps, c31);
float3 MaterialDiffuseColor		: register (ps, c32);
float3 MaterialSpecularColor	: register (ps, c33);
float3 MaterialEmissiveColor	: register (ps, c34);

float4 PackedParameters			: register (ps, c35);

#define		MaterialOpacity				PackedParameters.x
#define		MaterialSpecularShininess	PackedParameters.y
#define		MaterialAlphaCullLimit		PackedParameters.z
#define		FadeAmount					PackedParameters.w 


// -----------------------------------------------------------
// DECAL PROJECTION G-BUFFER PASS
// -----------------------------------------------------------

struct VS_INPUT_0
{
		float4 Position			: POSITION0;
		float3 Normal			: NORMAL0;
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Binormal			: BINORMAL0;
		float3 Tangent			: TANGENT0;
	#endif
};

struct VS_OUTPUT_0
{
		float4 Position			: POSITION0;
		float3 Normal			: TEXCOORD0;
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Binormal			: TEXCOORD1;
		float3 Tangent			: TEXCOORD2;
	#endif

		// Camera to vertex world space vector
		float3 WorldViewVec		: TEXCOORD4;
};

struct PS_INPUT_0
{
		float Side				: VFACE;
		float4 ScreenPosition	: VPOS;
		float3 Normal			: TEXCOORD0;
	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Binormal			: TEXCOORD1;
		float3 Tangent			: TEXCOORD2;
	#endif

		// Camera to pixel world space vector
		float3 WorldViewVec		: TEXCOORD4;	
};

struct PS_OUTPUT_0
{
		// Output to normal buffer
		float4 Normal			: COLOR0;

};


VS_OUTPUT_0 vs_main_g_pass( VS_INPUT_0 Input )
{
	VS_OUTPUT_0 Output = (VS_OUTPUT_0)0.0f;
  
	// World space view vector (camera to vertex)
	float4 WorldPos = mul(WorldMatrix, Input.Position);
	Output.WorldViewVec = WorldPos.xyz - CameraPosition;
	Output.Position = mul(ViewProjMatrix, WorldPos);
	
	// Transform Normal
	Output.Normal = mul((float3x3)WorldViewInvTrps, Input.Normal);
	
	// Transform Tangent, Binormal
	#if defined(ZE_SHADER_TANGENT_SPACE)
		Output.Tangent = mul((float3x3)WorldViewInvTrps, Input.Tangent);
		Output.Binormal = mul((float3x3)WorldViewInvTrps, Input.Binormal);
	#endif

	return Output;
}

PS_OUTPUT_0 ps_main_g_pass( PS_INPUT_0 Input )
{
	PS_OUTPUT_0 Output = (PS_OUTPUT_0)0.0f;
	
	// Normalized screen coordinates
	float2 ScreenCoord = Input.ScreenPosition.xy * PixelSize.xy + PixelSize.zw;
	
	// Restore world position
	float Depth = ZEGBuffer_GetDepth(ScreenCoord);
	float3 PixelWorldPos = CameraPosition + normalize(Input.WorldViewVec) * Depth;

	// Transform decal world pos into decal texture space
	float4 DecalTextureCoord = mul(DecalViewProjTex, float4(PixelWorldPos, 1.0f));
	DecalTextureCoord /= DecalTextureCoord.w;

	// Clip out-of-boundary areas
	// @TODO: try clip() intrinsic funtion
	if (DecalTextureCoord.x < 0.0f || DecalTextureCoord.x > 1.0f)	discard;
	if (DecalTextureCoord.y < 0.0f || DecalTextureCoord.y > 1.0f)	discard;
	if (DecalTextureCoord.z < 0.0f || DecalTextureCoord.z > 1.0f)	discard;
	
	float Alpha = 0.0f;
	#ifdef ZE_SHADER_OPACITY_MAP
		Alpha = MaterialOpacity * tex2D(OpacityMap, DecalTextureCoord.xy).r;
	#elif defined(ZE_SHADER_OPACITY_BASE_ALPHA)
		Alpha = MaterialOpacity * tex2D(DiffuseMap, DecalTextureCoord.xy).a;
	#else 
		Alpha = MaterialOpacity;
	#endif
		
	//#ifdef ZE_SHADER_ALPHA_CULL
	// Alpha cull is hard coded since we cannot blend normals
		if (Alpha < 0.95f /*MaterialAlphaCullLimit*/)
			discard;
	//#endif

	// Normal Mapping
	float3 Normal = (float3)0.0f;
	#if defined(ZE_SHADER_NORMAL_MAP)
		Normal = tex2D(NormalMap, DecalTextureCoord.xy).rgb * 2.0f - 1.0f;
		Normal = Normal.x * Input.Tangent + Normal.y * Input.Binormal + Normal.z * Input.Normal;
		Normal *= Input.Side;
	#else
		Normal = Input.Normal * Input.Side;
	#endif

	Output.Normal.xyz = normalize(Normal) * 0.5f + 0.5f;
	Output.Normal.w = MaterialSpecularShininess;

	return Output;
}

// -----------------------------------------------------------
// DECAL PROJECTION FORWARD PASS
// -----------------------------------------------------------

struct VS_INPUT_1
{
	float4 Position			: POSITION0;
};

struct VS_OUTPUT_1
{
	float4 Position			: POSITION0;
	float3 ViewVector		: TEXCOORD0;
};

struct PS_INPUT_1
{
	float4 ScreenPosition	: VPOS;
	float3 ViewVector		: TEXCOORD0;
};

struct PS_OUTPUT_1
{
	float4 PixelColor		: COLOR0;
};

VS_OUTPUT_1 vs_main_forward_pass( VS_INPUT_1 Input )
{
	VS_OUTPUT_1 Output = (VS_OUTPUT_1)0.0f;
	
	float4 WorldPos = mul(WorldMatrix, Input.Position);
	Output.ViewVector = WorldPos.xyz - CameraPosition;
	Output.Position = mul(ViewProjMatrix, WorldPos);

	return Output;
}

PS_OUTPUT_1 ps_main_forward_pass( PS_INPUT_1 Input )
{
	PS_OUTPUT_1 Output = (PS_OUTPUT_1)0.0f;

	// Normalized screen coordinates
	float2 ScreenCoord = Input.ScreenPosition.xy * PixelSize.xy + PixelSize.zw;
	
	// Restore world position
	float Depth = ZEGBuffer_GetDepth(ScreenCoord);
	float3 PixelWorldPos = CameraPosition + normalize(Input.ViewVector) * Depth;

	// Transform decal world pos into decal texture space
	float4 DecalTextureCoord = mul(DecalViewProjTex, float4(PixelWorldPos, 1.0f));
	DecalTextureCoord /= DecalTextureCoord.w;

	// Clip out-of-boundary areas
	// @TODO: try clip() intrinsic method
	if (DecalTextureCoord.x < 0.0f || DecalTextureCoord.x > 1.0f)	discard;
	if (DecalTextureCoord.y < 0.0f || DecalTextureCoord.y > 1.0f)	discard;
	if (DecalTextureCoord.z < 0.0f || DecalTextureCoord.z > 1.0f)	discard;

	#ifdef ZE_SHADER_OPACITY_MAP
		Output.PixelColor.a = MaterialOpacity * FadeAmount * tex2D(OpacityMap, DecalTextureCoord.xy).r;
	#elif defined(ZE_SHADER_OPACITY_BASE_ALPHA)
		Output.PixelColor.a = MaterialOpacity * FadeAmount * tex2D(DiffuseMap, DecalTextureCoord.xy).a;
	#else 
		Output.PixelColor.a = MaterialOpacity * FadeAmount;
	#endif

	#ifdef ZE_SHADER_ALPHA_CULL
		if (Output.PixelColor.a < MaterialAlphaCullLimit)
			discard;
	#endif
	

	#ifdef ZE_SHADER_AMBIENT
		float3 AmbientColor = MaterialAmbientColor;
		//#ifdef ZE_SHADER_SSAO
			//AmbientColor *= tex2D(SSAOBuffer, ScreenCoord).r;
		//#endif
		Output.PixelColor.rgb = AmbientColor;
	#endif

	#ifdef ZE_SHADER_DIFFUSE
		float3 DiffuseColor = MaterialDiffuseColor;
		DiffuseColor *= ZELBuffer_GetDiffuse(ScreenCoord);
		Output.PixelColor.rgb += DiffuseColor;
	#endif

	#ifdef ZE_SHADER_BASE_MAP
		Output.PixelColor.rgb *= tex2D(DiffuseMap, DecalTextureCoord.xy).rgb;
	#endif

	#ifdef ZE_SHADER_SPECULAR
		float3 SpecularColor = MaterialSpecularColor;
		#ifdef ZE_SHADER_SPECULAR_MAP
			SpecularColor *= tex2D(SpecularMap, DecalTextureCoord.xy).rgb;
		#endif
		SpecularColor *= ZELBuffer_GetSpecular(ScreenCoord);
		Output.PixelColor.rgb += SpecularColor;
	#endif

	#ifdef ZE_SHADER_EMMISIVE
		float3 EmissiveColor = MaterialEmissiveColor;
		#ifdef ZE_SHADER_EMMISIVE_MAP
			EmissiveColor *= tex2D(EmissiveMap, DecalTextureCoord.xy).rgb;
		#endif
		Output.PixelColor.rgb += EmissiveColor;
	#endif

	return Output;
}
