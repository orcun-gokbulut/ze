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

float4x4  WorldViewProjMatrix          : register(c0);
float4x4  BoneMatrices[58]             : register(c32);

struct VS_INPUT
{
	float4 Position			: POSITION0;
	float3 Normal			: NORMAL0;
	#ifdef ZESHADER_TRANSPARANT
		float2 Texcoord		: Texcoord;
	#endif
	#ifdef ZESHADER_SKINTRANSFORM
		int4 BoneIndices     : BLENDINDICES0;
		float4 BoneWeights   : BLENDWEIGHT0;
	#endif
};

struct VS_OUTPUT
{
	float4 Position          : POSITION0;
	float2 Depth			 : TEXCOORD0;
	#ifdef ZESHADER_TRANSPARANT
	float2 Texcoord			: TEXCOORD1;
	#endif
};

float MaterialOpasity;
sampler DiffuseMap;
sampler OpasityMap;

VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output;

	float4 Position;

	#ifdef ZESHADER_SKINTRANSFORM
		Position = float4(0.0f, 0.0f, 0.0f, 0.0f);
		for (int I = 0; I < 4; I++)
			if (Input.BoneWeights[I] > 0.0f)
				Position = mul(Input.Position, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
	#else
		Position = Input.Position;
	#endif

	Output.Position = mul(Position, WorldViewProjMatrix);
	Output.Depth.xy = Output.Position.zw;
	
	#ifdef ZESHADER_TRANSPARANT
	Output.Texcoord = Input.Texcoord;
	#endif	

	return(Output);
}