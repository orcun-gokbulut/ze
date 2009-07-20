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

#if defined(ZESHADER_COMPONENT0)
#define ZESHADER_SKINTRANFORMATION
#endif
#if defined(ZESHADER_COMPONENT1) 
#define ZESHADER_DIFFUSEMAP
#endif
#if defined(ZESHADER_COMPONENT2)
#define ZESHADER_NORMALMAP
#endif
#if defined(ZESHADER_COMPONENT3)
#define ZESHADER_SPECULARMAP
#endif
#if defined(ZESHADER_COMPONENT4)
#define ZESHADER_EMMISIVEMAP
#endif
#if defined(ZESHADER_COMPONENT5)
#define ZESHADER_OCAPASITYMAP
#endif
#if defined(ZESHADER_COMPONENT6)
#define ZESHADER_DETAILDIFFUSEMAP
#endif
#if defined(ZESHADER_COMPONENT7)
#define ZESHADER_DETAILNORMALMAP
#endif
#if defined(ZESHADER_COMPONENT8)
#define ZESHADER_REFLECTION
#endif
#if defined(ZESHADER_COMPONENT9)
#define ZESHADER_REFRACTION
#endif
#if defined(ZESHADER_COMPONENT10)
#define ZESHADER_LIGHTMAP
#endif
#if defined(ZESHADER_COMPONENT11)
#define ZESHADER_DISTORTIONMAP
#endif

// Transformation matrices 5 matrices
float4x4  WorldViewProjMatrix			: register(c0);
float4x4  WorldMatrix					: register(c4);
float4x4  WorldInvTrpsMatrix			: register(c8);
float4x4  WorldInvMatrix				: register(c12);

// Other general constants 8 vectors
float4    ViewPosition					: register(c16);

//First light 4 vector 1 matrix
float4    LightDirection				: register(c24);
float3    LightAttenuationFactors		: register(c25);
float4x4  LightMapMatrix				: register(c28);

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
	float3 LightDirection     : TEXCOORD3;

	#ifdef ZESHADER_SHADOWMAP
	//float2 ShadowMapCoord     : TEXCOORD4;
	#endif
};

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
				Normal += mul(Input.Normal, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];

				#ifdef ZESHADER_NORMALMAP
					Tangent += mul(Input.Tangent, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
					Binormal += mul(Input.Binormal, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
				#endif
			}	
	#else
		Position = Input.Position;
		Normal = Input.Normal;
		#ifdef ZESHADER_NORMALMAP
			Tangent = Input.Tangent;
			Binormal = Input.Binormal;;
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
		Output.LightDirection = mul(TangentTransform, LightDirection);
	#else
		Output.Normal = mul(Normal, WorldInvTrpsMatrix);
		Output.ViewDirection = ViewDisplacement;
		Output.LightDirection = LightDirection;
	#endif
	
	Output.Texcoord = Input.Texcoord;

	return(Output);
}