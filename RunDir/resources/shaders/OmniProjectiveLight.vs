/*ZE_POST_PROCESSOR_START(License)*/
//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  MapResource.h - Zinek Engine v0.05.00 Build 1024 Source Code                                 
// -------------------------------------------------------------------------------- //
//  Copyright (c) 2007-2009 Y. Orçun GÖKBULUT. All rights reserved.                 //
//                                                                                  //
//                 READ TERMS BELLOW BEFORE TAKING ANY ACTION !                     //
//                                                                                  //
//  These coded instructions, statements, and computer programs contain             //
//  unpublished proprietary information belongs to Y. Orçun GÖKBULUT and they are   //
//  protected by international copyright laws. This file is intellectual property   //
//  of the Y. Orçun GÖKBULUT. Storing this file, using this file, changing content  //
//  of this file, copying or duplicating this file, compiling this file, publishing // 
//  this file to anykind media or network, editing  this file, selling this file,   //
//  booking this file, reading this file are probited without notice and written    //
//  permision of Y. Orçun GÖKBULUT.                                                 //
//  These terms at above can only be changed by Y. Orçun GÖKBULUT.                  //
//                                                                                  //
//  Contact Information:                                                            //
//     Adress : Zinek Code House & Game Studio                                      //                                             
//              Aydinlar Mahallesi.                                                 //
//              Mimar Sokak. 33/5                                                   //
//              06450  Ankara/TURKEY                                                //
//     Phone  : +90 (533) 734 21 22                                                 //
//     E-Mail : contact@zinekengine.com                                             //
//     WWW    : http://www.zinekengine.com                                          //
//                                                                                  //
//*                                                                                *//
//  DESCRIPTION         :                                                           //
//  AUTHOR(S)           : Y. Orçun GÖKBULUT                                         //
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
float4    LightPosition					: register(c24);
float3    LightAttenuationFactors		: register(c25);
float4x4  LightRotationMatrix			: register(c28);

// Bone matrices rest of the space
float4x4  BoneMatrices[58]				: register(c32);

struct VSInput 
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

struct VSOutput 
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
								out float LightAttenuation)
{
}

VSOutput vs_main( VSInput Input )
{
	VSOutput Output;
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
				Position += mul(Input.Position, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
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