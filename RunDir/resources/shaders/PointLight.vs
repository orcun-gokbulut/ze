/*ZE_POST_PROCESSOR_START(License)*/
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
//  written permision of Y. Or�un G�KBULUT. These terms at above can only be        //
//  changed by Y. Or�un G�KBULUT.                                                   //
//  author(s).                                                                      //
//*                                                                                *//
//  DESCRIPTION         :                                                           //
//  AUTHOR(S)           : Y. Or�un G�KBULUT                                         //
//*                                                                                *//
//////////////////////////////////////////////////////////////////////////////////////
/*ZE_POST_PROCESSOR_END()*/

// Transformation matrices 5 matrices
float4x4  WorldViewProjMatrix			: register(c0);
float4x4  WorldMatrix					: register(c4);
float4x4  WorldInvTrpsMatrix			: register(c8);
float4x4  WorldInvMatrix				: register(c12);

// Other general constants 4 vectors
float4    ViewPosition					: register(c16);

//First light 4 vector 1 matrix
float3    LightPosition					: register(c24);
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
	float4 ViewDirection      : TEXCOORD2;
	float4 LightDirection     : TEXCOORD3;

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
								out float LightAttenuation,
								out float LightDistance)
{
	float3 LightDisplacement = LightPosition - Position;
	LightDistance = length(LightDisplacement);

	#ifdef ZESHADER_NORMALMAP
		LightDirection = mul(TangentTransform, LightDisplacement);
	#else
		LightDirection = LightDisplacement;
	#endif

	LightAttenuation = 1.0f / (LightAttenuationFactors[0] + LightAttenuationFactors[1] * LightDistance + LightAttenuationFactors[2] * LightDistance * LightDistance);
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
			Binormal = Input.Binormal;
		#endif
	#endif
	
	Output.Position = mul(Position, WorldViewProjMatrix);
	float4 WorldPosition = mul(Position, WorldMatrix);

	float3 ViewDisplacement = ViewPosition - WorldPosition;
	
	float3x3 TangentTransform;
	#ifdef ZESHADER_NORMALMAP
		TangentTransform[0] = mul(Tangent, WorldInvTrpsMatrix);
		TangentTransform[1] = mul(Binormal, WorldInvTrpsMatrix);
		TangentTransform[2] = mul(Normal, WorldInvTrpsMatrix);
		Output.ViewDirection.xyz = mul(TangentTransform, ViewDisplacement);
	#else
		Output.Normal = mul(Normal, WorldInvTrpsMatrix);
		Output.ViewDirection.xyz = ViewDisplacement;
	#endif

	Output.Texcoord = Input.Texcoord;
	
	CalculatePointLight(WorldPosition, LightPosition, LightAttenuationFactors, TangentTransform, Output.LightDirection.xyz, Output.LightDirection.w, Output.ViewDirection.w);

	return(Output);
}