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

float4x4  WorldViewProjMatrix          : register(c0);
float4x4  BoneMatrices[58]             : register(c32);

struct VSInput
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

struct VSOutput
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

VSOutput vs_main(VSInput Input)
{
	VSOutput Output;

	float4 Position;

	#ifdef ZESHADER_SKINTRANSFORM
		Position = float4(0.0f, 0.0f, 0.0f, 0.0f);
		for (int I = 0; I < 4; I++)
			if (Input.BoneWeights[I] > 0.0f)
				Position += mul(Input.Position, BoneMatrices[Input.BoneIndices[I]]) * Input.BoneWeights[I];
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