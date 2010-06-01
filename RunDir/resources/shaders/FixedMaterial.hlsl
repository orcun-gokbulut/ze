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

// Vertex Shader Parameters
///////////////////////////////////////////////////////////////////////////////////////
// Transformation matrices 5 matrices
float4x4  WorldViewProjMatrix			: register(c0);
float4x4  WorldMatrix					: register(c4);
float4x4  WorldInvTrpsMatrix			: register(c8);
float4x4  WorldInvMatrix				: register(c12);

// Other general constants 4 vectors
float4    ViewPosition					: register(c16);
float     MaterialRefractionIndex		: register(c17);

//First light 4 vector 1 matrix
float4    LightPosition					: register(c24);
float3    LightAttenuationFactors		: register(c25);
float4x4  LightMapMatrix				: register(c28);

// Bone matrices rest of the space
float4x4  BoneMatrices[58]				: register(c32);


// Prelighning Pass
///////////////////////////////////////////////////////////////////////////////////////
struct VS_INPUT 
{
	float4 Position             : POSITION0;
	float3 Normal               : NORMAL0;
	float2 Texcoord             : TEXCOORD0;
	#ifdef ZESHADER_LIGHTMAP
		float2 LightMapTexcoord : TEXCOORD1;
	#endif

	#ifdef ZESHADER_SKINTRANSFORM
		int4 BoneIndices        : BLENDINDICES0;
		float4 BoneWeights      : BLENDWEIGHT0;
	#endif
};

struct VS_OUTPUT 
{
	float4 Position                : POSITION0;
	float2 Texcoord                : TEXCOORD0;
	#ifdef ZESHADER_LIGHTMAP
		float2 LightMapTexcoord     : TEXCOORD1;
	#endif
	#ifdef ZESHADER_HEMISPHERELIGHTNING
		#ifdef ZESHADER_TANGENTSPACE
			float3 SkyVector            : TEXCOORD2;
		#else
			float3 Normal               : TEXCOORD2;
		#endif
	#endif

	#ifdef ZESHADER_REFLECTION
		float3 ReflectionVector     : TEXCOORD3;
	#endif

	#ifdef ZESHADER_REFRACTION
		float3 RefractionVector     : TEXCOORD4;
	#endif
};

VS_OUTPUT vs_prelightning(VS_INPUT Input)
{
	VS_OUTPUT Output;
	float4 Position;
	float3 Normal;

	#ifdef ZESHADER_SKINTRANSFORM
		Position = float4(0.0f, 0.0f, 0.0f, 0.0f);
		Normal = float3(0.0f, 0.0f, 0.0f);
		for (int I = 0; I < 4; I++)
			if (Input.BoneWeights[I] > 0.0f)
			{
				Position += mul(Input.Position, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
				Normal += mul(Input.Normal, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
			}	
	#else
		Position = Input.Position;
		Normal = Input.Normal;
	#endif

	Output.Position = mul(Position, WorldViewProjMatrix);

	#if defined(ZESHADER_REFLECTION) || defined(ZESHADER_REFRACTION)
		float3 CameraDirection = normalize(Position - ViewPosition);
	#endif
	
	#ifdef ZESHADER_REFLECTION
		Output.ReflectionVector = reflect(-CameraDirection, Normal);
	#endif
	
	#ifdef ZESHADER_REFRACTION
		Output.RefractionVector = refract(-CameraDirection, Normal, MaterialRefractionIndex);
	#endif

	#ifdef ZESHADER_LIGHTMAP
		Output.LightMapTexcoord = Input.LightMapTexcoord;
	#endif

	Output.Texcoord = Input.Texcoord;
	
	return Output;
}

float4	  MaterialParams0			 : register(c0);
float4	  MaterialParams1			 : register(c1);
float4	  MaterialParams2			 : register(c2);
float4	  MaterialParams3			 : register(c3);
float4	  MaterialParams4			 : register(c4);

#define	  MaterialAmbientColor         MaterialParams0.xyz
#define	  MaterialOpacity			   MaterialParams0.w
#define	  MaterialDiffuseColor         MaterialParams1.xyz
#define	  MaterialSpecularColor        MaterialParams2.xyz
#define	  MaterialSpecularFactor	   MaterialParams2.w
#define	  MaterialEmmisiveColor        MaterialParams3.xyz
#define	  MaterialEmmisiveFactor	   MaterialParams3.w
#define	  MaterialReflectionFactor	   MaterialParams4.x
#define	  MaterialRefractionFactor     MaterialParams4.y;
#define	  MaterialDetailMapTiling      MaterialParams4.zw;

float4    LightColor                 : register(c12);
float     LightIntensity             : register(c13);

// General Material Properties 
sampler   DiffuseMap                 : register(s0);
sampler   NormalMap                  : register(s1);
sampler	  ParallaxMap				 : register(s2);
sampler   SpecularMap                : register(s3);
sampler   OpacityMap				 : register(s4);
sampler   DetailDiffuseMap           : register(s5);
sampler   DetailNormalMap            : register(s6);

// PreLightning Material Properties
sampler   EmmisiveMap                : register(s7);
sampler	  ReflectionMap				 : register(s8);
sampler   RefractionMap              : register(s9);
sampler   LightMap                   : register(s10);
sampler	  DistortionMap				 : register(s11);

struct PS_INPUT
{
	float2 Texcoord             : TEXCOORD0;
	#ifdef ZESHADER_LIGHTMAP
	float2 LightMapTexcoord     : TEXCOORD1;
	#endif
	#ifdef ZESHADER_REFLECTION
	float3 ReflectionVector     : TEXCOORD3;
	#endif
	#ifdef ZESHADER_REFRACTION
	float3 RefractionVector     : TEXCOORD4;
	#endif
	#ifdef ZESHADER_DISTORTION
	float2 ScreenCoordinates    : TEXCOORD5;
	#endif
};

float4 ps_prelightning(PS_INPUT Input) : COLOR0
{
	float4 OutputColor = float4(0.0f, 0.0f ,0.0f ,1.0f);

	#ifdef ZESHADER_AMBIENT
		OutputColor.rgb += MaterialAmbientColor;
	#endif
	
	#ifdef ZESHADER_EMMISIVE
		#ifdef ZESHADER_EMMISIVEMAP
			OutpuColor.rgb += MaterialEmmisiveColor * tex2D(EmmisiveMap, Input.Texcoord);
		#else
			OutputColor.rgb += MaterialEmmisiveColor;
		#endif
	#endif

	#ifdef ZESHADER_DIFFUSEMAP
		OutputColor.rgb *= tex2D(DiffuseMap, Input.Texcoord).rgb;
	#endif

	#ifdef ZESHADER_LIGHTMAP
		OutputColor.rgb *= float4(tex2D(LightMap, Input.Texcoord).rgb, 1.0f);
	#endif

	#ifdef ZESHADER_REFLECTION
		OutputColor.rgb += MaterialReflectionFactor * texCUBE(EnvironmentMap, normalize(Input.ReflectionVector)).rgb;
	#endif

	#ifdef ZESHADER_REFRACTION
		OutputColor.rgb += MaterialRefractionFactor * texCUBE(EnvironmentMap, normalize(Input.RefractionVector)).rgb;
	#endif
	
	#ifdef ZESHADER_DISTORTION
		float2 DistortionTexCoord = tex2D(DistortionMap, Input.Texcoord);
		OutputColor.rgb += DistortionFactor * tex2D(ScreenMap, ScreenCoordinates + DistortionTexCoord);
	#endif

	#ifdef ZESHADER_OPASITY
		#ifdef ZESHADER_OPACITY_MAP
			#ifdef ZESHADER_OPACITY_CONSTANT
				OutputColor.a = MaterialOpacity * tex2D(OpacityMap, Input.Texcoord).r;
			#else
				OutputColor.a = tex2D(DiffuseMap, Input.Texcoord).r;
			#endif
		#elif defined(ZESHADER_OPACITY_DIFFUSEALPHA
			#ifdef ZESHADER_OPACITY_CONSTANT
				OutputColor.a = MaterialOpacity * tex2D(DiffuseMap, Input.Texcoord).a;
			#else
				OutputColor.a = tex2D(DiffuseMap, Input.Texcoord).a;
			#endif
		#elif 
			OutputColor.a = MaterialOpacity;
		#endif
	#endif

	return OutputColor;
}

struct PS_INPUT
{
	#ifndef ZESHADER_NORMALMAP
		float3 Normal             : TEXCOORD0;
	#endif

	float2 Texcoord           : TEXCOORD1;
	float3 ViewDirection      : TEXCOORD2;
	float4 LightDirection     : TEXCOORD3;

	#ifdef ZESHADER_SHADOWMAP
	//float3 ShadowMapCoord     : TEXCOORD4;
	#endif
};

float4 ps_main(PS_INPUT Input) : COLOR0
{
	float4 OutputColor;

	#ifdef ZESHADER_NORMALMAP
		float3 Normal = (2.0f * tex2D(NormalMap, Input.Texcoord) - 1.0f);
	#else
		float3 Normal = normalize(Input.Normal);
	#endif 

	// Transparancy
	#ifdef ZESHADER_OPASITYMAP
		OutputColor.a = MaterialOpasity * tex2D(OpacityMap, Input.Texcoord).r;
	#else
		#ifdef ZESHADER_DIFFUSEMAP
			OutputColor.a = MaterialOpasity * tex2D(DiffuseMap, Input.Texcoord).a;
		#else
			OutputColor.a = MaterialOpasity;
		#endif
	#endif
		
	float3 ViewDirection = normalize(Input.ViewDirection);
	float3 LightDirection = normalize(Input.LightDirection.xyz);

	float3 HalfVector = normalize(ViewDirection + LightDirection);
	float DiffuseAngularAttenuation = saturate(dot(LightDirection, Normal));
	/*if (DiffuseAngularAttenuation > 0.0f)
	{*/	
		#ifdef ZESHADER_DIFFUSEMAP
			#ifdef ZESHADER_DETAILMAP
				float3 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor * tex2D(DiffuseMap, Input.Texcoord).rgb * tex2D(DetailMap, Input.Texcoord * MaterialDetailMapTiling);
			#else
				float3 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor * tex2D(DiffuseMap, Input.Texcoord).rgb;
			#endif
		#else
			#ifdef ZESHADER_DETAILMAP
				float3 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor * tex2D(DetailMap, Input.Texcoord * MaterialDetailMapTiling);
			#else
				float3 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor;
			#endif
		#endif

		float SpecularAngularAttenuation;

		SpecularAngularAttenuation = pow(saturate(dot(HalfVector, Normal)), MaterialSpecularFactor);
		#ifdef ZESHADER_SPECULARMAP
			float3 SpecularColor = SpecularAngularAttenuation * MaterialSpecularColor * tex2D(SpecularMap, Input.Texcoord).rgb;
		#else
			float3 SpecularColor = SpecularAngularAttenuation * MaterialSpecularColor;
		#endif
		OutputColor.rgb = (DiffuseColor + SpecularColor) * LightColor * (Input.LightDirection.w * LightIntensity);
	/*}
	else
		OutputColor.rgb = float3(0.0f, 0.0f, 0.0f);	*/

	return OutputColor;

}
