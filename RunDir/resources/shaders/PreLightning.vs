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
float4x4 WorldViewProjMatrix : register(c0);
float4x4 WorldViewMatrix : register(c4);
float4x4 WorldViewInvTrpsMatrix : register(c8);
float FarZ : register(c13);

// Other general constants 4 vectors
float4 ViewPosition : register(c16);
float MaterialRefractionIndex : register(c17);

#define MaterialProperties
//First light 4 vector 1 matrix
float4    LightPosition : register(c24);
float3    LightAttenuationFactors : register(c25);
float4x4  LightMapMatrix : register(c28);

// Bone matrices rest of the space
float4x4  BoneMatrices[58] : register(c32);

struct VSInput 
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

struct VSOutput 
{
	float4 Position : POSITION0;
	float4 NormalDepth : TEXCOORD0;
	float2 Texcoord : TEXCOORD1;

	#ifdef ZESHADER_LIGHTMAP
		float2 LightMapTexcoord : TEXCOORD2;
	#endif
	#ifdef ZESHADER_REFLECTION
		float3 ReflectionVector : TEXCOORD3;
	#endif
	#ifdef ZESHADER_REFRACTION
		float3 RefractionVector : TEXCOORD4;
	#endif
	#ifdef ZESHADER_DISTORTION
		float3 Distortion
	#endif
	float3 _Position : TEXCOORD5;
};

#include "SkinTransform.hlsl"

VSOutput vs_main(VSInput Input)
{
	VSOutput Output;

	// Pre Pipeline

	// Vertex Transform;
	float4 Position;
	float3 Normal;

	SkinTransform(Input);

	// Pipeline 
	Output.Position = mul(Input.Position, WorldViewProjMatrix);
	Output.NormalDepth.xyz = mul(Input.Normal, WorldViewInvTrpsMatrix).xyz;
	Output.NormalDepth.w = mul(Input.Position, WorldViewMatrix).z;

	Output._Position = mul(Input.Position, WorldViewMatrix).xyz;
	
	// Parameter Process
	Output.Texcoord = Input.Texcoord;

	// Lightning Effect
	#if defined(ZESHADER_REFLECTION) || defined(ZESHADER_REFRACTION)
		float3 CameraDirection = normalize(Input.Position - ViewPosition);
	#endif
	
	#ifdef ZESHADER_REFLECTION
		Output.ReflectionVector = reflect(-CameraDirection, Input.Normal);
	#endif
	
	#ifdef ZESHADER_REFRACTION
		Output.RefractionVector = refract(-CameraDirection, Normal, MaterialRefractionIndex);
	#endif

	#ifdef ZESHADER_LIGHTMAP
		Output.LightMapTexcoord = Input.LightMapTexcoord;
	#endif

	// Post Pipeline

	return Output;
}