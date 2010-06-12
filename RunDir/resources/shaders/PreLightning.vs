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
float4x4  ViewMatrix					: register(c4);
float4x4  ViewInvTrpsMatrix				: register(c8);
float		FarZ						: register(c13);
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
	float4 NormalDepth               : TEXCOORD5;

	
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output;
	// Pre Pipeline

	// Vertex Transform;
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

	// Pipeline 
	Output.Position = mul(Position, WorldViewProjMatrix);
	Output.NormalDepth.xyz = 0.5f * mul(Normal, ViewInvTrpsMatrix).xyz - 0.5f;
	Output.NormalDepth.w = mul(Position, ViewMatrix).x / FarZ;

	// Parameter Process
	Output.Texcoord = Input.Texcoord;

	// Lightning Effect
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

	// Post Pipeline

	return Output;
}