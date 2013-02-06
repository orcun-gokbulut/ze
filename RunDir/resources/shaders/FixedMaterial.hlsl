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


// Samplers

SamplerState BaseMapSampler			: register(s5);
SamplerState NormalMapSampler		: register(s6);
SamplerState ParallaxMapSampler		: register(s7);
SamplerState SpecularMapSampler		: register(s8);
SamplerState OpacityMapSampler		: register(s9);
SamplerState EmmisiveMapSampler		: register(s10);
SamplerState EnvironmentMapSampler	: register(s11);
SamplerState LightMapSampler		: register(s12);
SamplerState DistortionMapSampler	: register(s13);
SamplerState DetailBaseMapSampler	: register(s14);
SamplerState DetailNormalMapSampler	: register(s15);
SamplerState SubSurfScattMapSampler	: register(s16);
SamplerState SSAOPointSampler		: register(s17);


// Textures
Texture2D<float4>	BaseMap			: register(t5);
Texture2D<float3>	NormalMap		: register(t6);
Texture2D<float4>	ParallaxMap		: register(t7);
Texture2D<float3>	SpecularMap		: register(t8);
Texture2D<float>	OpacityMap		: register(t9);
Texture2D<float3>	EmmisiveMap		: register(t10);
TextureCube<float3> EnvironmentMap	: register(t11);
Texture2D<float3>	LightMap		: register(t12);
Texture2D<float4>	DistortionMap	: register(t13);
Texture2D<float>	DetailBaseMap	: register(t14);
Texture2D<float3>	DetailNormalMap	: register(t15);
Texture2D<float>	SubSurfScattMap	: register(t16);
Texture2D<float>	SSAOBuffer		: register(t17);

struct ZEFixedMaterial_VSInput 
{
		float3 Position		: POSITION;
		float3 Normal		: NORMAL;
	//#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Tangent		: TANGENT;
		float3 Binormal		: BINORMAL;
	//#endif
		float2 Texcoord		: TEXCOORD;
	//#if defined(ZE_SHADER_SKIN_TRANSFORM)
	//	int4 BoneIndices	: BLENDINDICES0;
	//	float4 BoneWeights	: BLENDWEIGHT0;
	//#endif
		//float4 Color		: VERTEXCOLOR;
};

/*
#define ZE_SHADER_VERTEX_INPUT_TYPE ZEFixedMaterial_VSInput
#include "SkinTransform.hlsl"
*/

cbuffer CommonVSData : register(b0)
{
	float4x4	WorldMatrix				: packoffset(c0);
	float4x4	WorldViewMatrix			: packoffset(c4);
	float4x4	WorldViewProjMatrix		: packoffset(c8);
	float4x4	WorldViewInvTrpsMatrix	: packoffset(c12);
	float4		CameraPosition			: packoffset(c16);
	bool		EnableSkin				: packoffset(c17.x);
	// 12 bytes padding needed here
	//float4x4	BoneMatrices[58]		: packoffset(c18);

};

// G-Buffer Pass
/////////////////////////////////////////////////////////////////////////////////////////

// Vertex Shader
struct ZEFixedMaterial_GBuffer_VSOutput 
{
		float3 Position		: TEXCOORD0;
		float3 Normal		: TEXCOORD1;
		float2 Texcoord		: TEXCOORD2;

	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Tangent		: TEXCOORD3;
		float3 Binormal		: TEXCOORD4;
	#endif
		float4 Position_	: SV_Position;
};

ZEFixedMaterial_GBuffer_VSOutput ZEFixedMaterial_GBuffer_VertexShader(ZEFixedMaterial_VSInput Input)
{
	ZEFixedMaterial_GBuffer_VSOutput Output;

	//if (EnableSkin)
		//SkinTransform(Input);

	Output.Position_ = mul(WorldViewProjMatrix, float4(Input.Position, 1.0f));

	Output.Position = mul(WorldViewMatrix, float4(Input.Position, 1.0f)).xyz;
	Output.Normal = mul((float3x3)WorldViewInvTrpsMatrix, Input.Normal);

	#if defined(ZE_SHADER_TANGENT_SPACE)
		Output.Tangent = mul((float3x3)WorldViewInvTrpsMatrix, Input.Tangent);
		Output.Binormal = mul((float3x3)WorldViewInvTrpsMatrix, Input.Binormal);
	#endif
	
	Output.Texcoord = Input.Texcoord;

	return Output;
}

// Pixel Shader
struct ZEFixedMaterial_GBuffer_PSInput
{
		float3 Position		: TEXCOORD0;
		float3 Normal		: TEXCOORD1;

	#if defined(ZE_SHADER_SPECULAR_GLOSS_MAP) || defined(ZE_SHADER_NORMAL_MAP) || defined(ZE_SHADER_ALPHA_CULL) || defined(ZE_SHADER_DETAIL_NORMAL_MAP)
		float2 Texcoord		: TEXCOORD2;
	#endif

	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Tangent		: TEXCOORD3;
		float3 Binormal		: TEXCOORD4;
	#endif
		bool Side			: SV_IsFrontFace;
};

cbuffer GPassPixelShaderData : register(b0)
{
	struct
	{
		float	Opacity;	
		float	AlphaCullLimit;
		float2	DetailMapTiling;
		float	DetailDistance;
		float	DetailFadeDistance;
		float	SpecularShininess;
		float	SubSurfScattFactor;

	} GPDataPS	: packoffset(c0);
};

ZEGBuffer ZEFixedMaterial_GBuffer_PixelShader(ZEFixedMaterial_GBuffer_PSInput Input)
{
	ZEGBuffer GBuffer = (ZEGBuffer)0;
	
	//GBDoParallax(Input);

	#ifdef ZE_SHADER_ALPHA_CULL
		#ifdef ZE_SHADER_OPACITY_MAP
			float Alpha = GPDataPS.Opacity * OpacityMap.Sample(OpacityMapSampler, Input.Texcoord);
		#elif defined(ZE_SHADER_OPACITY_BASE_ALPHA)
			float Alpha = GPDataPS.Opacity * BaseMap.Sample(BaseMapSampler, Input.Texcoord).a;
		#else 
			float Alpha = GPDataPS.Opacity;
		#endif
		
		if (Alpha <= GPDataPS.AlphaCullLimit)
		{
			discard;
			return GBuffer;
		}
	#endif
	
	ZEGBuffer_SetViewPosition(GBuffer, Input.Position);

	float3 Normal = Input.Normal;
	#if defined(ZE_SHADER_NORMAL_MAP)
		Normal = NormalMap.Sample(NormalMapSampler, Input.Texcoord) * 2.0f - 1.0f;
		Normal = Normal.x * Input.Tangent + Normal.y * Input.Binormal + Normal.z * Input.Normal;
		Normal *= Input.Side ? 1.0f : -1.0f;
	#else
		Normal *=  Input.Side;
	#endif

	#ifdef ZE_SHADER_DETAIL_NORMAL_MAP
		float3 DetailNormal = (float3)0.0f;
		DetailNormal = DetailNormalMap.Sample(DetailNormalMapSampler, Input.Texcoord * GPDataPS.DetailMapTiling) * 2.0f - 1.0f;
		DetailNormal = DetailNormal.x * Input.Tangent + DetailNormal.y * Input.Binormal + DetailNormal.z * Input.Normal;
		DetailNormal = DetailNormal * Input.Side ? 1.0f : -1.0f;

		float Temp = Input.Position.z - GPDataPS.DetailDistance;
		float LerpAmount =  Temp < 0.0f ? 0.0f : saturate(Temp / GPDataPS.DetailFadeDistance);
		Normal = lerp(DetailNormal + Normal, Normal, LerpAmount);
	#endif
	
	ZEGBuffer_SetViewNormal(GBuffer, Normal);

	#if defined(ZE_SHADER_SPECULAR_GLOSS_MAP)
		ZEGBuffer_SetSpecularGlossiness(GBuffer, GlossMap.Sample(GlossMapSampler, Input.Texcoord));
	#else
		ZEGBuffer_SetSpecularGlossiness(GBuffer, GPDataPS.SpecularShininess);
	#endif
	
	#if defined(ZE_SHADER_SUBSURFACE_SCATTERING_MAP)
		ZEGBuffer_SetSubSurfaceScattering(GBuffer, SubSurfScattMap.Sample(SubSurfScattMapSampler, Input.Texcoord) * GPDataPS.SubSurfScattFactor);
	#else
		ZEGBuffer_SetSubSurfaceScatteringFactor(GBuffer, GPDataPS.SubSurfScattFactor);
	#endif

	return GBuffer;
}


// Forward Pass
/////////////////////////////////////////////////////////////////////////////////////////
struct ZEFixedMaterial_ForwardPass_VSOutput
{
		
		float2 Texcoord				: TEXCOORD0;

	#ifdef ZE_SHADER_DETAIL_MAP
		float2 DetailTexcoord		: TEXCOORD1;
	#endif

	#ifdef ZE_SHADER_LIGHT_MAP
		float2 LightMapTexcoord     : TEXCOORD2;
	#endif

	#if defined(ZE_SHADER_REFLECTION) || defined(ZE_SHADER_REFRACTION)
		float3 ViewVector			: TEXCOORD3;
	#endif

		//float4 Color				: TEXCOORD5;
		float4 Position				: SV_Position;
};


ZEFixedMaterial_ForwardPass_VSOutput ZEFixedMaterial_ForwardPass_VertexShader(ZEFixedMaterial_VSInput Input)
{
	ZEFixedMaterial_ForwardPass_VSOutput Output = (ZEFixedMaterial_ForwardPass_VSOutput)0.0f;
	
	//if (EnableSkin)
		//SkinTransform(Input);

	Output.Position = mul(WorldViewProjMatrix, float4(Input.Position, 1.0f));
	Output.Texcoord = Input.Texcoord;
	
	#if defined(ZE_SHADER_REFLECTION) || defined(ZE_SHADER_REFRACTION)
		float4 VertexWorldPos = mul(WorldMatrix, float4(Input.Position, 1.0f));
		Output.ViewVector = (CameraPosition - VertexWorldPos).xyz;
	#endif

	#ifdef ZE_SHADER_LIGHTMAP
		Output.LightMapTexcoord = Input.LightMapTexcoord;
	#endif
	
	//Output.Color = Input.Color;
		
	return Output;
}

struct ZEFixedMaterial_ForwardPass_PSOutput
{
	float4	Color	: SV_TARGET0;
};

struct ZEFixedMaterial_ForwardPass_PSInput
{
		
		float2 Texcoord				: TEXCOORD0;
	
	#ifdef ZE_SHADER_DETAIL_MAP
		float2 DetailTexcoord		: TEXCOORD1;
	#endif
	
	#ifdef ZE_SHADER_LIGHT_MAP
		float2 LightMapTexcoord     : TEXCOORD2;
	#endif
	
	#if defined(ZE_SHADER_REFLECTION) || defined(ZE_SHADER_REFRACTION)
		float3 ViewVector			: TEXCOORD3;
	#endif

		//float4 Color				: TEXCOORD5;
		float4 ScreenPosition		: SV_Position;
};

cbuffer ForwardPassPixelShaderData : register(b0)
{
	struct
	{
		float2		DetailMapTiling;
		float2		PixelSize;
		float		Opacity;
		float		AlphaCullLimit;
		float		DetailDistance;
		float		DetailFadeDistance;
		float3		AmbientColor;
		float		ReflectionFactor;
		float3		SpecularColor;
		float		RefractionFactor;
		float3		DiffuseColor;
		float		RefractionIndex;
		float3		EmissiveColor;
		float		SubSurfaceScatteringFactor;
		float4x4	InvViewMatrix;

	} FPDataPS : packoffset(c0);
};

ZEFixedMaterial_ForwardPass_PSOutput ZEFixedMaterial_ForwardPass_PixelShader(ZEFixedMaterial_ForwardPass_PSInput Input)
{
	ZEFixedMaterial_ForwardPass_PSOutput Output;
	Output.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	#ifdef ZE_SHADER_OPACITY
		#ifdef ZE_SHADER_OPACITY_MAP
				Output.Color.a = FPDataPS.Opacity * OpacityMap.Sample(OpacityMapSampler, Input.Texcoord);
		#elif defined(ZE_SHADER_OPACITY_BASE_ALPHA)
				Output.Color.a = FPDataPS.Opacity * BaseMap.Sample(BaseMapSampler, Input.Texcoord);
		#else
			Output.Color.a = FPDataPS.Opacity;
		#endif
	#endif
	
	#ifdef ZE_SHADER_ALPHA_CULL
		if (Output.Color.a <= FPDataPS.AlphaCullLimit)
		{
			discard;
			return Output;
		}
	#endif
	
	float2 ScreenPosition = Input.ScreenPosition.xy * FPDataPS.PixelSize;

	#ifdef ZE_SHADER_AMBIENT
		float3 AmbientColor = FPDataPS.AmbientColor;
		//#ifdef ZE_SHADER_SSAO
			//AmbientColor *= tex2D(SSAOBuffer, ScreenPosition).r;
		//#endif
		Output.Color.rgb = AmbientColor;
		#ifdef ZE_SHADER_LIGHT_MAP
			Output.Color.rgb *= LightMap.Sample(LightMapSampler, Input.Texcoord);
		#endif
	#endif

	#ifdef ZE_SHADER_DIFFUSE
		float3 DiffuseColor = FPDataPS.DiffuseColor;
		DiffuseColor *= ZELBuffer_GetDiffuse(ScreenPosition);
		Output.Color.rgb += DiffuseColor;
	#endif
	
	#ifdef ZE_SHADER_BASE_MAP
		Output.Color.rgb *= BaseMap.Sample(BaseMapSampler, Input.Texcoord).rgb;
	#endif
	
	#ifdef ZE_SHADER_DETAIL_BASE_MAP
		Output.Color.rgb *= (255.0f / 138.0f) * DetailBaseMap.Sample(DetailBaseMapSampler, Input.Texcoord * FPDataPS.DetailMapTiling);
	#endif

	#ifdef ZE_SHADER_SPECULAR
		float3 SpecularColor = FPDataPS.SpecularColor;
		#ifdef ZE_SHADER_SPECULAR_MAP
			SpecularColor *= SpecularMap.Sample(SpecularMapSampler, Input.Texcoord);
		#endif
		SpecularColor *= ZELBuffer_GetSpecular(ScreenPosition);
		Output.Color.rgb += SpecularColor;
	#endif

	#ifdef ZE_SHADER_EMMISIVE
		float3 EmmisiveColor = FPDataPS.EmissiveColor;
		#ifdef ZE_SHADER_EMMISIVE_MAP
			EmmisiveColor *= EmmisiveMap.Sample(EmmisiveMapSampler, Input.Texcoord);
		#endif
		Output.Color.rgb += EmmisiveColor;
	#endif

	#ifdef ZE_SHADER_VERTEX_COLOR
		//Output.Color *= Input.Color;
	#endif
	
	#if defined(ZE_SHADER_REFLECTION) || defined(ZE_SHADER_REFRACTION)
		float3 ViewNormal = ZEGBuffer_GetViewNormal(ScreenPosition);
		float3 WorldNormal = mul((float3x3)InvViewMatrix, ViewNormal);
		
		#ifdef ZE_SHADER_REFLECTION
			float3 ReflectionVector = reflect(-Input.ViewVector, WorldNormal);
			Output.Color.rgb += FPDataPS.ReflectionFactor * EnvironmentMap.Sample(EnvironmentMapSampler, normalize(ReflectionVector));
		#endif
	
		#ifdef ZE_SHADER_REFRACTION
			float3 RefractionVector = refract(-Input.ViewVector, WorldNormal, FPDataPS.RefractionIndex);
			Output.Color.rgb += FPDataPS.RefractionFactor * EnvironmentMap.Sample(EnvironmentMapSampler, RefractionVector);
		#endif
	#endif

	return Output;
}
