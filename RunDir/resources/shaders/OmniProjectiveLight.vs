/*ZEHEADER_START*/
//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  Core.h - Zinek Engine v0.04.02 Build 905 Source Code                                 
// -------------------------------------------------------------------------------- //
//  Copyright (c) 2007-2008 Zinek Engine group.                                     //
//  All rights reserved.                                                            //
//                                                                                  //
//                 READ TERMS BELLOW BEFORE TAKING ANY ACTION !                     //
//                                                                                  //
//  These coded instructions, statements, and computer programs contain             //
//  unpublished proprietary information written by Zinek Engine group and they are  //
//  protected by international copyright laws. This file is intellectual property   //
//  of the Zinek Engine group and author(s). Storing this file, using this file,    //
//  compiling this file, publishing this file to anykind media or network, editing  //
//  this file, selling this file, reading this file is probited without notice and  //
//  written permision of Y. Orcun Gokbulut. These terms at above can only be        //
//  changed by Y. Orcun GOKBULUT.                                                   //
//  author(s).                                                                      //
//*                                                                                *//
//  DESCRIPTION         :                                                           //
//  AUTHOR(S)           : Y. Orcun GOKBULUT                                         //
//*                                                                                *//
//////////////////////////////////////////////////////////////////////////////////////
/*ZEHEADER_END*/

#ifdef ZESHADER_COMPONENT0
#define ZESHADER_SKINTRANFORMATION
#endif
#ifdef ZESHADER_COMPONENT1
#define ZESHADER_DIFFUSEMAP
#endif
#ifdef ZESHADER_COMPONENT2
#define ZESHADER_NORMALMAP
#endif
#ifdef ZESHADER_COMPONENT3
#define ZESHADER_SPECULARMAP
#endif
#ifdef ZESHADER_COMPONENT4
#define ZESHADER_EMMISIVEMAP
#endif
#ifdef ZESHADER_COMPONENT5
#define ZESHADER_OCAPASITYMAP
#endif
#ifdef ZESHADER_COMPONENT6
#define ZESHADER_DETAILDIFFUSEMAP
#endif
#ifdef ZESHADER_COMPONENT7
#define ZESHADER_DETAILNORMALMAP
#endif
#ifdef ZESHADER_COMPONENT8
#define ZESHADER_REFLECTION
#endif
#ifdef ZESHADER_COMPONENT9
#define ZESHADER_REFRACTION
#endif
#ifdef ZESHADER_COMPONENT10
#define ZESHADER_LIGHTMAP
#endif
#ifdef ZESHADER_COMPONENT11
#define ZESHADER_DISTORTIONMAP
#endif

// Transformation matrices 5 matrices
float4x4  WorldViewProjMatrix			: register(c0);
float4x4  WorldMatrix					: register(c4);
float4x4  WorldInvTrpsMatrix			: register(c8);
float4x4  WorldInvMatrix				: register(c12);

// Other general constants 4 vectors
float4    ViewPosition					: register(c16);

//First light 4 vector 1 matrix
float4    LightPosition					: register(c24);
float3    LightAttenuationFactors		: register(c25);
float4x4  LightRotationMatrix			: register(c28);

// Bone matrices rest of the space
float4x4  BoneMatrices[58]				: register(c32);

struct VS_INPUT 
{
	float4 Position    : POSITION0;
	float3 Normal      : NORMAL0;
	float3 Tangent     : TANGENT0;
	float3 Binormal    : BINORMAL0;
	float2 Texcoord    : TEXCOORD0;
	#ifdef ZESHADER_SKINTRANSFORM
		int4 BoneIndices        : BLENDINDICES0;
		float4 BoneWeights      : BLENDWEIGHT0;
	#endif
};

struct VS_OUTPUT 
{
	float4 Position           : POSITION0;
	#ifndef ZESHADER_NORMALMAP
		float3 Normal             : TEXCOORD0;
	#endif

	float2 Texcoord           : TEXCOORD1;
	float3 ViewDirection      : TEXCOORD2;
	float4 LightDirection     : TEXCOORD3;
	float3 CubeTexcoord		  : TEXCOORD4;
	#ifdef ZESHADER_SHADOWMAP
//		float2 ShadowMapCoord     : TEXCOORD4;
	#endif
};

inline void CalculatePointLight(
								in float3 Position, 
								in float3 LightPosition,
								in float3 LightAttenuationFactors,
								in float3x3 TangentTransform,
								out float3 LightDirection,
								out float3 LightAttenuation)
{
}

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;
	float4 Position;
	float3 Normal;
	float3 Tangent;
	float3 Binormal;

	#ifdef ZESHADER_SKINTRANSFORM
		Position = float4(0.0f, 0.0f, 0.0f, 0.0f);
		Normal = float3(0.0f, 0.0f, 0.0f);
		#ifdef ZESHADER_NORMALMAP
			Tangent = float3(0.0f, 0.0f, 0.0f);
			Binormal = float3(0.0f, 0.0f, 0.0f);
		#endif
		for (int I = 0; I < 4; I++)
			if (Input.BoneWeights[I] > 0.0f)
			{
				Position = mul(Input.Position, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
				Normal += mul(Input.Normal, (float3x3)BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];

				#ifdef ZESHADER_NORMALMAP
					Tangent += mul(Input.Tangent, (float3x3)BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
					Binormal += mul(Input.Binormal, (float3x3)BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
				#endif
			}	
	#else
		Position = Input.Position;
		Normal = Input.Normal;
		#ifdef ZESHADER_NORMALMAP
			Tangent = Input.Tangent;
			Binormal = Input.Binormal;
		#endif
	#endif
	
	Output.Position = mul(Position, WorldViewProjMatrix);
	float4 WorldPosition = mul(Position, WorldMatrix);

	float4 ViewDisplacement = ViewPosition - WorldPosition;
	
	float3x3 TangentTransform;
	#ifdef ZESHADER_NORMALMAP
		TangentTransform[0] = mul(Tangent, WorldInvTrpsMatrix);
		TangentTransform[1] = mul(Binormal, WorldInvTrpsMatrix);
		TangentTransform[2] = mul(Normal, WorldInvTrpsMatrix);
		Output.ViewDirection = mul(TangentTransform, ViewDisplacement);
	#else
		Output.Normal = mul(Normal, WorldInvTrpsMatrix);
		Output.ViewDirection = ViewDisplacement;
	#endif

	Output.Texcoord = Input.Texcoord;

	float3 LightDisplacement = LightPosition - WorldPosition;
	float LightDistance = length(LightDisplacement);

	#ifdef ZESHADER_NORMALMAP
		Output.LightDirection.xyz = mul(TangentTransform, LightDisplacement);
	#else
		Output.LightDirection.xyz = LightDisplacement;
	#endif

	//Attenuation
	Output.LightDirection.w = 1.0f / (LightAttenuationFactors[0] + LightAttenuationFactors[1] * LightDistance + LightAttenuationFactors[2] * LightDistance * LightDistance);
	
	Output.CubeTexcoord = mul(LightDisplacement, LightRotationMatrix);

	return(Output);
}