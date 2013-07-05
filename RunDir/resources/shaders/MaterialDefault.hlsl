//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - MaterialDefault.hlsl
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

#include "PipelineConstants.hlsl"
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

#if defined(ZE_SHADER_NORMAL_MAP) || defined(ZE_SHADER_PARALLAX_MAP) || defined(ZE_SHADER_DETAIL_NORMAL_MAP)
	#define ZE_SHADER_TANGENT_SPACE
#endif

#if defined(ZE_SHADER_SPECULAR_MAP) || defined(ZE_SHADER_BASE_MAP) || defined(ZE_SHADER_ALPHA_CULL) ||			\
	defined(ZE_SHADER_NORMAL_MAP) || defined(ZE_SHADER_PARALLAX_MAP) || defined(ZE_SHADER_DETAIL_NORMAL_MAP) ||	\
	defined(ZE_SHADER_OPACITY_MAP) || defined(ZE_SHADER_EMISSIVE_MAP) || defined(ZE_SHADER_DETAIL_BASE_MAP)
	#define ZE_SHADER_SAMPLE_TEXCOORD
#endif

struct ZEMaterialDefault_VSInput 
{
		float3 Position		: POSITION0;
		float3 Normal		: NORMAL0;

	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Tangent		: TANGENT0;
		float3 Binormal		: BINORMAL0;
	#endif

	#if defined(ZE_SHADER_SAMPLE_TEXCOORD)
		float2 TexCoord		: TEXCOORD0;
	#endif

	#if defined(ZE_SHADER_SKIN_TRANSFORM)
		int4 BoneIndices	: BLENDINDEX0;
		float4 BoneWeights	: BLENDWEIGHT0;
	#endif

	#if defined(ZE_SHADER_VERTEX_COLOR)
		float4 VertexColor	: VERTEXCOLOR0;
	#endif
};

#ifdef ZE_SHADER_SKIN_TRANSFORM
	cbuffer ZESkinningBufferVS : register(b1)
	{
		float4x4	BoneMatrices[ZE_SHADER_BONE_MATRIX_COUNT];
	};
#endif

#define ZE_SHADER_VERTEX_INPUT_TYPE ZEMaterialDefault_VSInput
#include "SkinTransform.hlsl"

cbuffer ZETransformationBuffer : register(b0)
{
	float4x4	WorldMatrix;
	float4x4	ViewMatrix;
	float4x4	ProjectionMatrix;
};

cbuffer ZEMaterialDefaultProperties : register(b0)
{
	float		Opacity;
	float		AlphaCullLimit;	
	float		SpecularShininess;
	float		SubSurfScatFactor;
		
	float2		DetailMapTiling;
	float		DetailDistanceFull;
	float		DetailDistanceFade;
		
	float3		AmbientColor;
	float		AmbientFactor;
		
	float3		DiffuseColor;
	float		DiffuseFactor;
		
	float3		EmissiveColor;
	float		EmissiveFactor;
		
	float3		SpecularColor;
	float		SpecularFactor;
		
	float		RefractionIndex;
	float		RefractionFactor;
	float		ReflectionFactor;
};

// Geometry Pass
/////////////////////////////////////////////////////////////////////////////////////////
struct ZEMaterialDefault_GeometryPass_VSOutput
{
		float3 Position		: VIEWPOSITION0;
		float3 Normal		: VIEWNORMAL0;

	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Tangent		: TANGENT0;
		float3 Binormal		: BINORMAL0;
	#endif
	
	#if defined(ZE_SHADER_SAMPLE_TEXCOORD)
		float2 TexCoord		: TEXCOORD0;
	#endif

		float4 Position_	: SV_Position;
};

ZEMaterialDefault_GeometryPass_VSOutput ZEMaterialDefault_GeometryPass_VertexShader(ZEMaterialDefault_VSInput Input)
{
	ZEMaterialDefault_GeometryPass_VSOutput Output;

	// Calculate skinned vertex
	SkinTransform(Input);

	// Calculate transofrmation matrices
	float4x4 WorldViewMatrix = mul(ViewMatrix, WorldMatrix);
	float4x4 WorldViewProjMatrix = mul(ProjectionMatrix, WorldViewMatrix);

	// Rasterizer position
	Output.Position_ = mul(WorldViewProjMatrix, float4(Input.Position, 1.0f));

	// Viewspace vertex normal and positions
	Output.Normal = mul((float3x3)WorldViewMatrix, Input.Normal);
	Output.Position = mul(WorldViewMatrix, float4(Input.Position, 1.0f)).xyz;
	
	// If normal map enabled
	#if defined(ZE_SHADER_TANGENT_SPACE)
		// Calculate view space tangent and binormal
		Output.Tangent = mul((float3x3)WorldViewMatrix, Input.Tangent);
		Output.Binormal = mul((float3x3)WorldViewMatrix, Input.Binormal);
	#endif
	
	// If we need to sample a texture
	#if defined(ZE_SHADER_SAMPLE_TEXCOORD)
		Output.TexCoord = Input.TexCoord;
	#endif

	return Output;
}

struct ZEMaterialDefault_GeometryPass_PSInput
{
		float3 Position		: VIEWPOSITION0;
		float3 Normal		: VIEWNORMAL0;

	#if defined(ZE_SHADER_TANGENT_SPACE)
		float3 Tangent		: TANGENT0;
		float3 Binormal		: BINORMAL0;
	#endif

	#if defined(ZE_SHADER_SAMPLE_TEXCOORD)
		float2 TexCoord		: TEXCOORD0;
	#endif

		bool Side			: SV_IsFrontFace;
};

ZEGBuffer ZEMaterialDefault_GeometryPass_PixelShader(ZEMaterialDefault_GeometryPass_PSInput Input)
{
	ZEGBuffer GBuffer = (ZEGBuffer)0.0f;

	// If alpha cull enabled.
	#ifdef ZE_SHADER_ALPHA_CULL
		float Alpha = Opacity;

		// Decide where to take opacity from.
		#ifdef ZE_SHADER_OPACITY_MAP
			Alpha *= OpacityMap.Sample(OpacityMapSampler, Input.TexCoord);
		#elif defined(ZE_SHADER_OPACITY_BASE_ALPHA)
			Alpha *= BaseMap.Sample(BaseMapSampler, Input.TexCoord).a;
		#endif
		
		// Cull based on alpha.
		if (Alpha <= AlphaCullLimit)
			discard;
	#endif
	
	// Set view space position to depth buffer
	ZEGBuffer_SetViewPosition(GBuffer, Input.Position);

	// If normal mapping enabled.
	float3 Normal = Input.Normal;
	#if defined(ZE_SHADER_NORMAL_MAP)
		// Transform local normals to view space normals.
		Normal = NormalMap.Sample(NormalMapSampler, Input.TexCoord) * 2.0f - 1.0f;
		Normal = Normal.x * Input.Tangent + Normal.y * Input.Binormal + Normal.z * Input.Normal;
		Normal *= Input.Side ? 1.0f : -1.0f;
	#else
		// Use geometry normals
		Normal *= Input.Side ? 1.0f : -1.0f;
	#endif

	// If detail normal map enabled
	#if defined(ZE_SHADER_DETAIL_NORMAL_MAP)
		// Transform normals to view space
		float3 DetailNormal = (float3)0.0f;
		DetailNormal = DetailNormalMap.Sample(DetailNormalMapSampler, Input.TexCoord * DetailMapTiling) * 2.0f - 1.0f;
		DetailNormal = DetailNormal.x * Input.Tangent + DetailNormal.y * Input.Binormal + DetailNormal.z * Input.Normal;
		DetailNormal = DetailNormal * Input.Side ? 1.0f : -1.0f;

		// Lerp with original normal based on distance
		float Temp = Input.Position.z - DetailDistanceFull;
		float LerpAmount =  Temp < 0.0f ? 0.0f : saturate(Temp / DetailDistanceFade);
		Normal = lerp(DetailNormal + Normal, Normal, LerpAmount);
	#endif
	
	// Set view space normal normal buffer
	ZEGBuffer_SetViewNormal(GBuffer, Normal);

	// If specular map enabled
	#if defined(ZE_SHADER_SPECULAR_GLOSS_MAP)
		ZEGBuffer_SetSpecularGlossiness(GBuffer, GlossMap.Sample(GlossMapSampler, Input.TexCoord));
	#else
		ZEGBuffer_SetSpecularGlossiness(GBuffer, SpecularShininess);
	#endif
	
	// If subsurface scattering map enabled
	#if defined(ZE_SHADER_SUBSURFACE_SCATTERING_MAP)
		float SubSurfScattSample = SubSurfScattMap.Sample(SubSurfScattMapSampler, Input.TexCoord);
		ZEGBuffer_SetSubSurfaceScattering(GBuffer, SubSurfScattSample * SubSurfScatFactor);
	#else
		ZEGBuffer_SetSubSurfaceScatteringFactor(GBuffer, SubSurfScatFactor);
	#endif

	return GBuffer;
}

// Forward Pass
/////////////////////////////////////////////////////////////////////////////////////////
struct  ZEMaterialDefault_AccumulationPass_VSOutput
{
	#if defined(ZE_SHADER_SAMPLE_TEXCOORD)
		float2 TexCoord			: TEXCOORD0;
	#endif

	#ifdef ZE_SHADER_DETAIL_MAP
		float2 DetailTexCoord	: TEXCOORD1;
	#endif

	#ifdef ZE_SHADER_LIGHT_MAP
		float2 LightMapTexCoord : TEXCOORD2;
	#endif

	#if defined(ZE_SHADER_REFLECTION) || defined(ZE_SHADER_REFRACTION)
		float3 ViewVector		: VIEWVECTOR0;
	#endif

	#if defined(ZE_SHADER_VERTEX_COLOR)
		float4 VertexColor		: VERTEXCOLOR0;
	#endif

		float4 Position			: SV_Position;
};

ZEMaterialDefault_AccumulationPass_VSOutput ZEMaterialDefault_AccumulationPass_VertexShader(ZEMaterialDefault_VSInput Input)
{
	ZEMaterialDefault_AccumulationPass_VSOutput Output;

	// Calculate skinned vertex
	SkinTransform(Input);

	// Calculate transofrmation matrices
	float4x4 WorldViewMatrix = mul(ViewMatrix, WorldMatrix);
	float4x4 WorldViewProjMatrix = mul(ProjectionMatrix, WorldViewMatrix);

	// Rasterizer position
	Output.Position = mul(WorldViewProjMatrix, float4(Input.Position, 1.0f));

	// If reflection or refraction enabled
	#if defined(ZE_SHADER_REFLECTION) || defined(ZE_SHADER_REFRACTION)
		// Calculate world space view vectors
		float4 VertexWorldPos = mul(WorldMatrix, float4(Input.Position, 1.0f));
		Output.ViewVector = (ZECameraWorldPos - VertexWorldPos).xyz;
	#endif

	// If we need to sample some texture
	#if defined(ZE_SHADER_SAMPLE_TEXCOORD)
		Output.TexCoord = Input.TexCoord;
	#endif

	// If lightmap enabled
	#ifdef ZE_SHADER_LIGHTMAP
		Output.LightMapTexCoord = Input.LightMapTexCoord;
	#endif
	
	// If vertex color enabled
	#if defined(ZE_SHADER_VERTEX_COLOR)
		Output.VertexColor = Input.VertexColor;
	#endif
	
	return Output;
}

struct ZEFixedMaterial_ForwardPass_PSOutput
{
	float4	Color	: SV_TARGET0;
};

struct ZEFixedMaterial_ForwardPass_PSInput
{
	#if defined(ZE_SHADER_SAMPLE_TEXCOORD)
		float2 TexCoord			: TEXCOORD0;
	#endif
	
	#ifdef ZE_SHADER_DETAIL_MAP
		float2 DetailTexCoord	: TEXCOORD1;
	#endif
	
	#ifdef ZE_SHADER_LIGHT_MAP
		float2 LightMapTexCoord	: TEXCOORD2;
	#endif
	
	#if defined(ZE_SHADER_REFLECTION) || defined(ZE_SHADER_REFRACTION)
		float3 ViewVector		: VIEWVECTOR0;
	#endif

	#if defined(ZE_SHADER_VERTEX_COLOR)
		float4 VertexColor		: VERTEXCOLOR0;
	#endif
		
		float4 ScreenPosition	: SV_Position;
};

ZEFixedMaterial_ForwardPass_PSOutput ZEMaterialDefault_AccumulationPass_PixelShader(ZEFixedMaterial_ForwardPass_PSInput Input)
{
	ZEFixedMaterial_ForwardPass_PSOutput Output = (ZEFixedMaterial_ForwardPass_PSOutput)0.0f;
	//Output.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	// If alpha cull enabled.
	#ifdef ZE_SHADER_ALPHA_CULL
		float Alpha = Opacity;

		// Decide where to take opacity from.
		#ifdef ZE_SHADER_OPACITY_MAP
			Alpha *= OpacityMap.Sample(OpacityMapSampler, Input.TexCoord);
		#elif defined(ZE_SHADER_OPACITY_BASE_ALPHA)
			Alpha *= BaseMap.Sample(BaseMapSampler, Input.TexCoord).a;
		#endif
		
		// Cull based on alpha.
		if (Alpha <= AlphaCullLimit)
			discard;
	#endif
	
	// Calculate sampling position for gbuffer
	float2 ScreenPosition = Input.ScreenPosition.xy * ZEViewportDimensionInv;

	// If ambient is enabled
	#ifdef ZE_SHADER_AMBIENT
		float3 MaterialAmbient = AmbientColor * AmbientFactor;
		
		//#ifdef ZE_SHADER_SSAO
			//AmbientColor *= tex2D(SSAOBuffer, ScreenPosition).r;
		//#endif
		
		// If light map is enabled
		#ifdef ZE_SHADER_LIGHT_MAP
			MaterialAmbient *= LightMap.Sample(LightMapSampler, Input.TexCoord);
		#endif

		Output.Color.rgb = AmbientColor;
	#endif

	// If diffuse color is enabled
	#ifdef ZE_SHADER_DIFFUSE
		float3 MaterialDiffuse = DiffuseColor * DiffuseFactor;
		MaterialDiffuse *= ZELBuffer_GetDiffuse(ScreenPosition);
		Output.Color.rgb += MaterialDiffuse;
	#endif
	
	// If base map enabled
	#ifdef ZE_SHADER_BASE_MAP
		float3 BaseMapSample = BaseMap.Sample(BaseMapSampler, Input.TexCoord).rgb;
		Output.Color.rgb *= BaseMapSample;
	#endif
	
	// If detail base map enabled
	#ifdef ZE_SHADER_DETAIL_BASE_MAP
		// Prevent over darkening by multiplying 255/average luminance
		float3 DetailBaseSmaple = DetailBaseMap.Sample(DetailBaseMapSampler, Input.TexCoord * DetailMapTiling);
		Output.Color.rgb *= (255.0f / 138.0f) * DetailBaseSmaple;
	#endif

	// If specular enabled
	#ifdef ZE_SHADER_SPECULAR
		float3 MaterialSpecular = SpecularColor * SpecularFactor;
		// If specular map enabled
		#ifdef ZE_SHADER_SPECULAR_MAP
			MaterialSpecular *= SpecularMap.Sample(SpecularMapSampler, Input.TexCoord);
		#endif
		MaterialSpecular *= ZELBuffer_GetSpecular(ScreenPosition);
		Output.Color.rgb += MaterialSpecular;
	#endif

	#ifdef ZE_SHADER_EMMISIVE
		float3 MaterialEmissive = EmissiveColor * EmissiveFactor;
		#ifdef ZE_SHADER_EMMISIVE_MAP
			MaterialEmissive *= EmmisiveMap.Sample(EmmisiveMapSampler, Input.TexCoord);
		#endif
		Output.Color.rgb += MaterialEmissive;
	#endif

	#ifdef ZE_SHADER_VERTEX_COLOR
		Output.Color *= Input.VertexColor;
	#endif

	#if defined(ZE_SHADER_REFLECTION) || defined(ZE_SHADER_REFRACTION)
		float3 ViewNormal = ZEGBuffer_GetViewNormal(ScreenPosition);
		float3 WorldNormal = mul((float3x3)InvViewMatrix, ViewNormal);
		
		#ifdef ZE_SHADER_REFLECTION
			float3 ReflectionVector = reflect(-Input.ViewVector, WorldNormal);
			float3 ReflectionSample = EnvironmentMap.Sample(EnvironmentMapSampler, normalize(ReflectionVector));
			Output.Color.rgb += ReflectionFactor * ReflectionSample;
		#endif
	
		#ifdef ZE_SHADER_REFRACTION
			float3 RefractionVector = refract(-Input.ViewVector, WorldNormal, RefractionIndex);
			float3 RefractionSample = EnvironmentMap.Sample(EnvironmentMapSampler, RefractionVector);
			Output.Color.rgb += RefractionFactor * RefractionSample;
		#endif
	#endif

	Output.Color.a = 1.0f;

	return Output;
}
