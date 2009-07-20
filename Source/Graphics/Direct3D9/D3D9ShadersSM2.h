//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9ShadersSM2.h
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

#define ZESHADER_COMPONENTMAPPING \
	"#ifdef ZESHADER_COMPONENT0\n"\
	"#define ZESHADER_SKINTRANFORMATION\n"\
	"#endif\n"\
	"#ifdef ZESHADER_COMPONENT1\n"\
	"#define ZESHADER_DIFFUSEMAP\n"\
	"#endif\n"\
	"#ifdef ZESHADER_COMPONENT2\n"\
	"#define ZESHADER_NORMALMAP\n"\
	"#endif\n"\
	"#ifndef ZESHADER_COMPONENT3\n"\
	"#define ZESHADER_SPECULARMAP\n"\
	"#endif\n"\
	"#ifdef ZESHADER_COMPONENT4\n"\
	"#define ZESHADER_EMMISIVEMAP\n"\
	"#endif\n"\
	"#ifdef ZESHADER_COMPONENT5\n"\
	"#define ZESHADER_OCAPASITYMAP\n"\
	"#endif\n"\
	"#ifdef ZESHADER_COMPONENT6\n"\
	"#define ZESHADER_DETAILDIFFUSEMAP\n"\
	"#endif\n"\
	"#ifdef ZESHADER_COMPONENT7\n"\
	"#define ZESHADER_DETAILNORMALMAP\n"\
	"#endif\n"\
	"#ifdef ZESHADER_COMPONENT8\n"\
	"#define ZESHADER_REFLECTION\n"\
	"#endif\n"\
	"#ifdef ZESHADER_COMPONENT9\n"\
	"#define ZESHADER_REFRACTION\n"\
	"#endif\n"\
	"#ifdef ZESHADER_COMPONENT10\n"\
	"#define ZESHADER_LIGHTMAP\n"\
	"#endif\n"\
	"#ifdef ZESHADER_COMPONENT11\n"\
	"#define ZESHADER_DISTORTIONMAP\n"\
	"#endif\n"






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning //
// VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char PreLightPassVS[] =
	ZESHADER_COMPONENTMAPPING
	"float4x4  WorldViewProjMatrix        : register(c0);\n"
	"float4x4  WorldMatrix                : register(c4);\n"
	"float4x4  WorldInvTrpsMatrix         : register(c8);\n"
	"float4x4  WorldInvMatrix             : register(c12);\n"

	"float4    ViewPosition               : register(c16);\n"
	"float     MaterialRefractionIndex    : register(c17);\n"

	"float4x4  BoneMatrices[58]           : register(c18);\n"

	"struct VS_INPUT \n\n"
	"{\n\n"
	   "float4 Position             : POSITION0;\n\n"
	   "float3 Normal               : NORMAL0;\n\n"
	   "float2 Texcoord             : TEXCOORD0;\n\n"
	   "#ifdef ZESHADER_LIGHTMAP\n\n"
		  "float2 LightMapTexcoord  : TEXCOORD1;\n\n"
	   "#endif\n\n"
	   "#ifdef ZESHADER_SKINNED\n\n"
		  "int4 BoneIndices         : BLENDINDICES0;\n"
		  "float4 BoneWeights       : BLENDWEIGHT0;\n"
	   "#endif\n"
	"};\n"

	"struct VS_OUTPUT \n"
	"{\n"
	   "float4 Position                : POSITION0;\n"
	   "float2 Texcoord                : TEXCOORD0;\n"
	   "#ifdef ZESHADER_LIGHTMAP\n"
		  "float2 LightMapTexcoord     : TEXCOORD1;\n"
	   "#endif\n"
	   
	   "#ifdef ZESHADER_HEMISPHERELIGHTNING\n"
		  "#ifdef ZESHADER_TANGENTSPACE\n"
			 "float3 SkyVector            : TEXCOORD2;\n"
		  "#else\n"
			 "float3 Normal               : TEXCOORD2;\n"
		  "#endif\n"
	   "#endif\n"
	      
	   "#ifdef ZESHADER_REFLECTION\n"
		  "float3 ReflectionVector     : TEXCOORD3;\n"
	   "#endif\n"
	   "#ifdef ZESHADER_REFRACTION\n"
		  "float3 RefractionVector     : TEXCOORD4;\n"
	   "#endif\n"
	"};\n"

	"VS_OUTPUT vs_main(VS_INPUT Input)\n"
	"{\n"
	   "VS_OUTPUT Output;\n"
	   
	   "float4x4 PositionTransform;\n"
	   "float4x4 WorldPositionTransform;\n"
	   "float3x3 NormalTransform;\n"

	   "#ifdef ZESHADER_SKINTRANSFORMATION\n"
		  "float4x4 AbsoluteTransform = Input.BoneWeights[0] * BoneMatrices[Input.BoneIndices[0]];\n"
		  "for (int I = 1; I < 4; I++)\n"
			 "if (Input.BoneWeights[0] > 0.0f)\n"
				"AbsoluteTransform += Input.BoneWeights[I] * BoneMatrices[Input.BoneIndices[I]];\n"
	            
		  "PositionTransform = mul(WorldViewProjMatrix, AbsoluteTransform);\n"
		  "NormalTransform = (float3x3)mul(WorldInvTrpsMatrix, AbsoluteTransform);\n"
		  "Output.Position = mul(Input.Position, PositionTransform);\n"
		  "WorldPositionTransform = mul(WorldMatrix, AbsoluteTransform);\n"
	   "#else\n"
		  "PositionTransform = WorldViewProjMatrix;\n"
		  "NormalTransform = (float3x3)WorldInvTrpsMatrix;\n"
		  "WorldPositionTransform = WorldMatrix;\n"
	   "#endif\n"
	      
	   "Output.Position = mul(Input.Position, PositionTransform);\n"
	   "float4 WorldPosition = mul(Input.Position, WorldPositionTransform);\n"
	   "float4 CameraDirection = normalize(ViewPosition - WorldPosition);\n"
	   
	   "#ifdef ZESHADER_REFLECTION\n"
	   "Output.ReflectionVector = reflect(-CameraDirection, Input.Normal);\n"
	   "#endif\n"
	   
	   "#ifdef ZESHADER_REFRACTION\n"
	   "Output.RefractionVector = refract(-CameraDirection, Input.Normal, MaterialRefractionIndex);\n"
	   "#endif\n"
	   
	   "#ifdef ZESHADER_LIGHTMAP\n"
	   "Output.LightMapTexcoord = Input.LightMapTexcoord;\n"
	   "#endif\n"
	   
	   "Output.Texcoord = Input.Texcoord;\n"
	   
	   "return Output;\n"
	"}";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning  Pre-Lightning //
// PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char PreLightPassPS[] =
	ZESHADER_COMPONENTMAPPING
	"float4 MaterialAmbientColor       : register(c0);\n"
	"float4 MaterialDiffuseColor       : register(c1);\n"
	"float4 MaterialSpecularColor      : register(c2);\n"
	"float4 MaterialEmmisiveColor      : register(c3);\n"
	"float  MaterialEmmisiveFactor     : register(c4);\n"
	"float  MaterialSpecularFactor     : register(c5);\n"
	"float  MaterialTransparancy       : register(c6);\n"
	"float  MaterialReflectionFactor   : register(c7);\n"
	"float  MaterialRefractionFactor   : register(c8);\n"
	"float2 MaterialDetailMapTiling    : register(c9);\n"

	"float4    LightColor                     : register(c12);\n"
	"float     LightIntensity                 : register(c13);\n"

	"sampler   DiffuseMap                     : register(s0);\n"
	"sampler   NormalMap                      : register(s1);\n"
	"sampler   SpecularMap                    : register(s2);\n"
	"sampler   EmmisiveMap                    : register(s3);\n"
	"sampler   OcapacityMap                   : register(s4);\n"
	"sampler   DetailMap                      : register(s5);\n"
	"sampler   EnvironmentMap                 : register(s6);\n"
	"sampler   LightMap                       : register(s7);\n"
	"sampler   ShadowMap                      : register(s8);\n"

	"struct PS_INPUT\n"
	"{\n"
	   "float2 Texcoord                : TEXCOORD0;\n"
	   "#ifdef ZESHADER_LIGHTMAP\n"
		  "float2 LightMapTexcoord     : TEXCOORD1;\n"
	   "#endif\n"
	   "#ifdef ZESHADER_REFLECTION\n"
		  "float3 ReflectionVector     : TEXCOORD3;\n"
	   "#endif\n"
	   "#ifdef ZESHADER_REFRACTION\n"
		  "float3 RefractionVector     : TEXCOORD4;\n"
	   "#endif\n"
	"};\n"

	"float4 ps_main(PS_INPUT Input) : COLOR0\n"
	"{\n"
	   "float4 Color; Color.xyz = 0.0f;\n"

	   "Color = (MaterialAmbientColor + MaterialEmmisiveColor);\n"

	   "#ifdef ZESHADER_DIFFUSEMAP\n"
		  "Color *= tex2D(DiffuseMap, Input.Texcoord);\n"
	   "#endif\n"
	   
	   "#ifdef ZESHADER_LIGHTMAP\n"
		  "Color *= float4(tex2D(LightMap, Input.Texcoord).rgb, 1.0f);\n"
	   "#endif\n"
	   
	   "#ifdef ZESHADER_EMMISIVEMAP\n"
		  "Color += MaterialEmmisiveFactor * (tex2D(EmmisiveMap, Input.Texcoord) + MaterialEmmisiveColor);\n"
	   "#endif\n"
	   
	   "#ifdef ZESHADER_REFLECTION\n"
		  "Color += MaterialReflectionFactor * texCUBE(EnvironmentMap, normalize(Input.ReflectionVector));\n"
	   "#endif\n"
	   
	   "#ifdef ZESHADER_REFRACTION\n"
		  "Color += MaterialRefractionFactor * texCUBE(EnvironmentMap, normalize(Input.RefractionVector));\n"
	   "#endif\n"

	   "return float4(Color.xyz, MaterialTransparancy);\n"
	"}";













////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight //
// VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX  VERTEX //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char PointLightVS[] =
	ZESHADER_COMPONENTMAPPING

	"float4x4  WorldViewProjMatrix          : register(c0);\n"
	"float4x4  WorldMatrix                  : register(c4);\n"
	"float4x4  WorldInvTrspMatrix           : register(c8);\n"
	"float4x4  WorldInvMatrix               : register(c12);\n"

	"float4    ViewPosition                 : register(c16);\n"

	"float4    LightPosition                : register(c24);\n"
	"float3    LightAttenuationFactors      : register(c25);\n"

	"float4x4  BoneMatrices[58]             : register(c32);\n"

	"struct VS_INPUT \n"
	"{\n"
	   "float4 Position    : POSITION0;\n"
	   "float3 Normal      : NORMAL0;\n"
	   "float3 Tangent     : TANGENT0;\n"
	   "float3 Binormal    : BINORMAL0;\n"
	   "float2 Texcoord    : TEXCOORD0;\n"
	"};\n"

	"struct VS_OUTPUT \n"
	"{\n"
	   "float4 Position           : POSITION0;\n"
	   "#ifndef ZESHADER_NORMALMAP\n"
	   "float3 Normal             : TEXCOORD0;\n"
	   "#endif\n"
	   "float2 Texcoord           : TEXCOORD1;\n"
	   "float3 ViewDirection      : TEXCOORD2;\n"
	   "float4 LightDirection     : TEXCOORD3;\n"
	   "#ifdef ZESHADER_SHADOWMAP\n"
	   "float2 ShadowMapCoord     : TEXCOORD4;\n"
	   "#endif\n"
	"};\n"


	"inline void CalculatePointLight(\n"
		  "in float3 Position, \n"
		  "in float3 LightPosition,\n"
		  "in float3 LightAttenuationFactors,\n"
		  "in float3x3 TangentTransform,\n"
		  "out float3 LightDirection,\n"
		  "out float3 LightAttenuation)\n"
	"{\n"
	   "float3 LightDisplacement = LightPosition - Position;\n"
	   "float LightDistance = length(LightDisplacement);\n"
	   "#ifdef ZESHADER_NORMALMAP\n"
	   "LightDirection = mul(LightDisplacement, TangentTransform);\n"
	   "#else\n"
	   "LightDirection = LightDisplacement;\n"
	   "#endif\n"
	   "LightAttenuation = 1.0f / (LightAttenuationFactors[0] + LightAttenuationFactors[1] * LightDistance + LightAttenuationFactors[2] * LightDistance * LightDistance);\n"
	"}\n"

	"VS_OUTPUT vs_main( VS_INPUT Input )\n"
	"{\n"
	   "VS_OUTPUT Output;\n"
	   "Output.Position = mul(Input.Position, WorldViewProjMatrix);\n"
	   "float4 Position = mul(Input.Position, WorldMatrix);\n"

	   "float3x3 TangentTransform;\n"
	   "#ifdef ZESHADER_NORMALMAP\n"
	   "TangentTransform[0] = mul(Input.Tangent, WorldInvTrspMatrix);\n"
	   "TangentTransform[1] = mul(Input.Binormal, WorldInvTrspMatrix);\n"
	   "TangentTransform[2] = mul(Input.Normal, WorldInvTrspMatrix);\n"
	   "#else\n"
	   "Output.Normal = mul(Input.Normal, WorldInvTrspMatrix);\n"
	   "#endif\n"
	   "Output.Texcoord = Input.Texcoord;\n"

	   "float4 ViewDisplacement = ViewPosition - Position;\n"
	   
	   "#ifdef ZESHADER_NORMALMAP\n"
	   "Output.ViewDirection = mul(ViewDisplacement, TangentTransform);\n"
	   "#else\n"
	   "Output.ViewDirection = ViewDisplacement;\n"
	   "#endif\n"

	   "CalculatePointLight(Position, LightPosition, LightAttenuationFactors, TangentTransform, Output.LightDirection.xyz, Output.LightDirection.w);\n"

	   "return(Output);\n"
	"}";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight //
// PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char PointLightPS[] =
	ZESHADER_COMPONENTMAPPING
	"float4    MaterialAmbientColor       : register(c0);\n"
	"float4    MaterialDiffuseColor       : register(c1);\n"
	"float4    MaterialSpecularColor      : register(c2);\n"
	"float4    MaterialEmmisiveColor      : register(c3);\n"
	"float     MaterialEmmisiveFactor     : register(c4);\n"
	"float     MaterialSpecularFactor     : register(c5);\n"
	"float     MaterialTransparancy       : register(c6);\n"
	"float     MaterialReflectionFactor   : register(c7);\n"
	"float     MaterialRefractionFactor   : register(c8);\n"
	"float2    MaterialDetailMapTiling    : register(c9);\n"

	"float4    LightColor                 : register(c12);\n"
	"float     LightIntensity             : register(c13);\n"

	"sampler   DiffuseMap                     : register(s0);\n"
	"sampler   NormalMap                      : register(s1);\n"
	"sampler   SpecularMap                    : register(s2);\n"
	"sampler   EmmisiveMap                    : register(s3);\n"
	"sampler   OcapacityMap                   : register(s4);\n"
	"sampler   DetailMap                      : register(s5);\n"
	"sampler   EnvironmentMap                 : register(s6);\n"
	"sampler   LightMap                       : register(s7);\n"
	"sampler   ShadowMap                      : register(s8);\n"

	"struct PS_INPUT\n"
	"{\n"
	   "#ifndef ZESHADER_NORMALMAP\n"
	   "float3 Normal             : TEXCOORD0;\n"
	   "#endif\n"
	   "float2 Texcoord           : TEXCOORD1;\n"
	   "float3 ViewDirection      : TEXCOORD2;\n"
	   "float4 LightDirection     : TEXCOORD3;\n"
	   "#ifdef ZESHADER_SHADOWMAP\n"
	   "float2 ShadowMapCoord     : TEXCOORD4;\n"
	   "#endif\n"
	"};\n"

	"inline void CalculateShadow(in sampler ShadowMap)\n"
	"{\n"
	"}\n"

	"inline void CalculatePointLight(\n"
		  "in float3 Normal,\n"
		  "in float3 LightDirectionUN,\n"
		  "in float  DistanceAttenuation,\n"
		  "in float3 ViewDirection,\n"
		  "in float LightIntensity,\n"
		  "in half4 LightColor,\n"
		  "in float2 Texcoord,\n"
		  "inout half4 OutputColor)\n"
	"{\n"
	   "float3 LightDirection = normalize(LightDirectionUN);\n"
	   "float3 HalfVector = normalize(ViewDirection + LightDirection);\n"
	   
	   "half DiffuseAngularAttenuation = saturate(dot(LightDirection, Normal));\n"
	 
	   "#ifdef ZESHADER_DIFFUSEMAP\n"
		  "#ifdef ZESHADER_DETAILMAP\n"
		  "half4 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor * tex2D(DiffuseMap, Texcoord) * tex2D(DetailMap, Texcoord * MaterialDetailMapTiling);\n"
		  "#else\n"
		  "half4 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor * tex2D(DiffuseMap, Texcoord);\n"
		  "#endif\n"
	  "#else\n"
		  "#ifdef ZESHADER_DETAILMAP\n"
		  "half4 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor * tex2D(DetailMap, Texcoord * MaterialDetailMapTiling);\n"
		  "#else\n"
		  "half4 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor;\n"
		  "#endif\n"
	   "#endif\n"
	   
	   "half SpecularAngularAttenuation = pow(saturate(dot(HalfVector, Normal)), MaterialSpecularFactor);\n"

	   "#ifdef ZESHADER_SPECULARMAP\n"
	   "half4 SpecularColor = SpecularAngularAttenuation * MaterialSpecularColor * tex2D(SpecularMap, Texcoord);\n"
	   "#else\n"
	   "half4 SpecularColor = SpecularAngularAttenuation * MaterialSpecularColor;\n"
	   "#endif\n"
	   
	   "OutputColor += (DiffuseColor + SpecularColor) * LightColor * (DistanceAttenuation * LightIntensity);\n"
	"}\n"

	"half4 ps_main(PS_INPUT Input) : COLOR0\n"
	"{\n"
	   "#ifdef ZESHADER_NORMALMAP\n"
	   "float3 Normal = (2.0f * tex2D(NormalMap, Input.Texcoord) - 1.0f);\n"
	   "#else\n"
	   "float3 Normal = normalize(Input.Normal);\n"
	   "#endif   \n"
	   "half4 OutputColor = half4(0.0f, 0.0f, 0.0f, 1.0f);\n"
	   "float3 ViewDirection = normalize(Input.ViewDirection);\n"

	   "CalculatePointLight(Normal, Input.LightDirection.xyz, Input.LightDirection.w, ViewDirection, LightIntensity, LightColor, Input.Texcoord, OutputColor);\n"
	   
	   "return OutputColor;\n"
	"}";



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight  PointLight //
// PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL  PIXEL     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char DirectionalLightVS[] =
	ZESHADER_COMPONENTMAPPING
	"float4x4  WorldViewProjMatrix          : register(c0);\n"
	"float4x4  WorldMatrix                  : register(c4);\n"
	"float4x4  WorldInvTrspMatrix           : register(c8);\n"
	"float4x4  WorldInvMatrix               : register(c12);\n"

	"float4    ViewPosition                 : register(c16);\n"
	"float4    LightDirection               : register(c24);\n"

	"float4x4  BoneMatrices[58]             : register(c32);\n"

	"struct VS_INPUT \n"
	"{\n"
	   "float4 Position    : POSITION0;\n"
	   "float3 Normal      : NORMAL0;\n"
	   "float3 Tangent     : TANGENT0;\n"
	   "float3 Binormal    : BINORMAL0;\n"
	   "float2 Texcoord    : TEXCOORD0;\n"
	"};\n"

	"struct VS_OUTPUT \n"
	"{\n"
	   "float4 Position           : POSITION0;\n"
	   "#ifndef ZESHADER_NORMALMAP\n"
	   "float3 Normal             : TEXCOORD0;\n"
	   "#endif\n"
	   "float2 Texcoord           : TEXCOORD1;\n"
	   "float3 ViewDirection      : TEXCOORD2;\n"
	   "float3 LightDirection     : TEXCOORD3;\n"
	   "#ifdef ZESHADER_SHADOWMAP\n"
	   "float2 ShadowMapCoord     : TEXCOORD4;\n"
	   "#endif\n"
	 "};\n"

	"VS_OUTPUT vs_main( VS_INPUT Input )\n"
	"{\n"
	   "VS_OUTPUT Output;\n"
	   "Output.Position = mul(Input.Position, WorldViewProjMatrix);\n"
	   "float4 Position = mul(Input.Position, WorldMatrix);\n"

	   "float3x3 TangentTransform; \n"
	   "#ifdef ZESHADER_NORMALMAP\n"
	   "TangentTransform[0] = mul(Input.Tangent, WorldInvTrspMatrix);\n"
	   "TangentTransform[1] = mul(Input.Binormal, WorldInvTrspMatrix);\n"
	   "TangentTransform[2] = mul(Input.Normal, WorldInvTrspMatrix);\n"
	   "#else\n"
	   "Output.Normal = mul(Input.Normal, WorldInvTrspMatrix);\n"
	   "#endif\n"
	   "Output.Texcoord = Input.Texcoord;\n"

	   "float4 ViewDisplacement = ViewPosition - Position;\n"
	   
	   "#ifdef ZESHADER_NORMALMAP\n"
	   "Output.ViewDirection = mul(ViewDisplacement, TangentTransform);\n"
	   "Output.LightDirection = mul(LightDirection, TangentTransform);\n"
	   "#else\n"
	   "Output.ViewDirection = ViewDisplacement;\n"
	   "Output.LightDirection = LightDirection;\n"
	   "#endif\n"

//	   "CalculatePointLight(Position, LightDirection, LightAttenuationFactors, TangentTransform, Output.LightDirection);\n"
//	   "CalculatePointLight(Position, LightDirection, LightAttenuationFactors, TangentTransform, Output.LightDirection);\n"

	   "return(Output);\n"
	"}";

const char DirectionalLightPS[] =
	ZESHADER_COMPONENTMAPPING
	"float4    MaterialAmbientColor       : register(c0);\n"
	"float4    MaterialDiffuseColor       : register(c1);\n"
	"float4    MaterialSpecularColor      : register(c2);\n"
	"float4    MaterialEmmisiveColor      : register(c3);\n"
	"float     MaterialEmmisiveFactor     : register(c4);\n"
	"float     MaterialSpecularFactor     : register(c5);\n"
	"float     MaterialTransparancy       : register(c6);\n"
	"float     MaterialReflectionFactor   : register(c7);\n"
	"float     MaterialRefractionFactor   : register(c8);\n"
	"float2    MaterialDetailMapTiling    : register(c9);\n"

	"float4    LightColor                 : register(c12);\n"
	"float     LightIntensity             : register(c13);\n"

	"sampler   DiffuseMap                     : register(s0);\n"
	"sampler   NormalMap                      : register(s1);\n"
	"sampler   SpecularMap                    : register(s2);\n"
	"sampler   EmmisiveMap                    : register(s3);\n"
	"sampler   OcapacityMap                   : register(s4);\n"
	"sampler   DetailMap                      : register(s5);\n"
	"sampler   EnvironmentMap                 : register(s6);\n"
	"sampler   LightMap                       : register(s7);\n"
	"sampler   ShadowMap                      : register(s8);\n"

	"struct PS_INPUT\n"
	"{\n"
	   "#ifndef ZESHADER_NORMALMAP\n"
	   "float3 Normal             : TEXCOORD0;\n"
	   "#endif\n"
	   "float2 Texcoord           : TEXCOORD1;\n"
	   "float3 ViewDirection      : TEXCOORD2;\n"
	   "float4 LightDirection     : TEXCOORD3;\n"
	   "#ifdef ZESHADER_SHADOWMAP\n"
	   "float2 ShadowMapCoord     : TEXCOORD4;\n"
	   "#endif\n"
	"};\n"

	"inline void CalculateShadow(in sampler ShadowMap)\n"
	"{\n"
	"}\n"

	"inline void CalculatePointLight(\n"
		  "in float3 Normal,\n"
		  "in float3 LightDirectionUN,\n"
//		  "in float  DistanceAttenuation,\n"
		  "in float3 ViewDirection,\n"
		  "in float LightIntensity,\n"
		  "in half4 LightColor,\n"
		  "in float2 Texcoord,\n"
		  "inout half4 OutputColor)\n"
	"{\n"
	   "float3 LightDirection = normalize(LightDirectionUN);\n"
	   "float3 HalfVector = normalize(ViewDirection + LightDirection);\n"
	   
	   "half DiffuseAngularAttenuation = saturate(dot(LightDirection, Normal));\n"
	 
	   "#ifdef ZESHADER_DIFFUSEMAP\n"
		  "#ifdef ZESHADER_DETAILMAP\n"
		  "half4 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor * tex2D(DiffuseMap, Texcoord) * tex2D(DetailMap, Texcoord * MaterialDetailMapTiling);\n"
		  "#else\n"
		  "half4 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor * tex2D(DiffuseMap, Texcoord);\n"
		  "#endif\n"
	   "#else\n"
		  "#ifdef ZESHADER_DETAILMAP\n"
		  "half4 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor * tex2D(DetailMap, Texcoord * MaterialDetailMapTiling);\n"
		  "#else\n"
		  "half4 DiffuseColor =  DiffuseAngularAttenuation * MaterialDiffuseColor;\n"
		  "#endif\n"
	   "#endif\n"
	   
	   "half SpecularAngularAttenuation = pow(saturate(dot(HalfVector, Normal)), MaterialSpecularFactor);\n"

	   "#ifdef ZESHADER_SPECULARMAP\n"
	   "half4 SpecularColor = SpecularAngularAttenuation * MaterialSpecularColor * tex2D(SpecularMap, Texcoord);\n"
	   "#else\n"
	   "half4 SpecularColor = SpecularAngularAttenuation * MaterialSpecularColor;\n"
	   "#endif\n"
	   
//	   "OutputColor += (DiffuseColor + SpecularColor) * LightColor * (DistanceAttenuation * LightIntensity);\n"
	   "OutputColor += (DiffuseColor + SpecularColor) * LightColor * LightIntensity;\n"
	"}\n"

	"half4 ps_main(PS_INPUT Input) : COLOR0\n"
	"{\n"
	   "#ifdef ZESHADER_NORMALMAP\n"
	   "float3 Normal = (2.0f * tex2D(NormalMap, Input.Texcoord) - 1.0f);\n"
	   "#else\n"
	   "float3 Normal = normalize(Input.Normal);\n"
	   "#endif\n"
	   "half4 OutputColor = half4(0.0f, 0.0f, 0.0f, 1.0f);\n"
	   "float3 ViewDirection = normalize(Input.ViewDirection);\n"

	   "CalculatePointLight(Normal, Input.LightDirection.xyz, ViewDirection, LightIntensity, LightColor, Input.Texcoord, OutputColor);\n"
	   
	   "return OutputColor;\n"
	"}";

const char ProjectiveLightVS[] =
	ZESHADER_COMPONENTMAPPING
	"float4x4  WorldViewProjMatrix          : register(c0);\n"
	"float4x4  WorldMatrix                  : register(c4);\n"
	"float4x4  WorldInvTrspMatrix           : register(c8);\n"
	"float4x4  WorldInvMatrix               : register(c12);\n"

	"float4    ViewPosition                 : register(c16);\n"
	"float4    LightDirection               : register(c24);\n"
	"float4x4  TextureMatrix				: register(c28);\n"
	"float4x4  BoneMatrices[58]             : register(c32);\n"

	"struct VS_INPUT \n"
	"{\n"
	   "float4 Position    : POSITION0;\n"
	   "float3 Normal      : NORMAL0;\n"
	   "float3 Tangent     : TANGENT0;\n"
	   "float3 Binormal    : BINORMAL0;\n"
	   "float2 Texcoord    : TEXCOORD0;\n"
	"};\n"

	"struct VS_OUTPUT \n"
	"{\n"
	   "float4 Position           : POSITION0;\n"
	   "#ifndef ZESHADER_NORMALMAP\n"
	   "float3 Normal             : TEXCOORD0;\n"
	   "#endif\n"
	   "float2 Texcoord           : TEXCOORD1;\n"
	   "float3 ViewDirection      : TEXCOORD2;\n"
	   "float3 LightDirection     : TEXCOORD3;\n"
	   "float2 LightCoord		  : TEXCOORD4;\n"
	 "};\n"

	"VS_OUTPUT vs_main( VS_INPUT Input )\n"
	"{\n"
	   "VS_OUTPUT Output;\n"
	   "Output.Position = mul(Input.Position, WorldViewProjMatrix);\n"
	   "float4 Position = mul(Input.Position, WorldMatrix);\n"

	   "float3x3 TangentTransform; \n"
	   "#ifdef ZESHADER_NORMALMAP\n"
	   "TangentTransform[0] = mul(Input.Tangent, WorldInvTrspMatrix);\n"
	   "TangentTransform[1] = mul(Input.Binormal, WorldInvTrspMatrix);\n"
	   "TangentTransform[2] = mul(Input.Normal, WorldInvTrspMatrix);\n"
	   "#else\n"
	   "Output.Normal = mul(Input.Normal, WorldInvTrspMatrix);\n"
	   "#endif\n"
	   "Output.Texcoord = Input.Texcoord;\n"

	   "float4 ViewDisplacement = ViewPosition - Position;\n"
	   
	   "#ifdef ZESHADER_NORMALMAP\n"
	   "Output.ViewDirection = mul(ViewDisplacement, TangentTransform);\n"
	   "Output.LightDirection = mul(LightDirection, TangentTransform);\n"
	   "#else\n"
	   "Output.ViewDirection = ViewDisplacement;\n"
	   "Output.LightDirection = LightDirection;\n"
	   "#endif\n"

//	   "CalculatePointLight(Position, LightDirection, LightAttenuationFactors, TangentTransform, Output.LightDirection);\n"
//	   "CalculatePointLight(Position, LightDirection, LightAttenuationFactors, TangentTransform, Output.LightDirection);\n"

	   "return(Output);\n"
	"}";

const char ShadowPassVS[] =
	ZESHADER_COMPONENTMAPPING

	"float4x4  WorldViewProjMatrix          : register(c0);\n"
	"float4x4  BoneMatrices[58]             : register(c32);\n"

	"struct VS_INPUT\n"
	"{\n"
	   "float4 Position    : POSITION0;\n"
	   "float3 Normal      : NORMAL0;\n"
	"};\n"

	"struct VS_OUTPUT\n"
	"{\n"
	   "float4 Position           : POSITION0;\n"
	   "float4 Position1          : TEXCOORD0;\n"
	"};\n"

	"VS_OUTPUT vs_main(VS_INPUT Input)\n"
	"{\n"
	   "VS_OUTPUT Output;\n"
	   "Output.Position1 = Output.Position = mul(Input.Position, WorldViewProjMatrix);\n"
	   "return(Output);\n"
	"}";

const char ShadowPassPS[] =
	ZESHADER_COMPONENTMAPPING
	"struct PS_INPUT\n"
	"{\n"
	   "float4 Position    : TEXCOORD0;\n"
	"};\n"


	"float4 ps_main(PS_INPUT Input) : COLOR0\n"
	"{\n"
		"return Input.Position.z / Input.Position.w;\n"
	"}";
