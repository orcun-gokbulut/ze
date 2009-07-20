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
float     MaterialRefractionIndex		: register(c17);

//First light 4 vector 1 matrix
float4    LightPosition					: register(c24);
float3    LightAttenuationFactors		: register(c25);
float4x4  LightMapMatrix				: register(c28);

// Bone matrices rest of the space
float4x4  BoneMatrices[58]				: register(c32);

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

VS_OUTPUT vs_main(VS_INPUT Input)
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
				Position = mul(Input.Position, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
				Normal += mul(Input.Normal, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
			}	
	#else
		Position = Input.Position;
		Normal = Input.Normal;
	#endif

	Output.Position = mul(Position, WorldViewProjMatrix);

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