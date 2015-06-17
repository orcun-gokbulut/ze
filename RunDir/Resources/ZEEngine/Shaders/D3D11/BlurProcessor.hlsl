//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - BlurProcessor.hlsl
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

//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  BlurProcessor.hlsl - Zinek Engine v0.05.00 Build 1024 Source Code				//
// -------------------------------------------------------------------------------- //
//  Copyright (c) 2007-2009 Y. Or�un G�KBULUT. All rights reserved.                 //
//                                                                                  //
//                 READ TERMS BELLOW BEFORE TAKING ANY ACTION !                     //
//                                                                                  //
//  These coded instructions, statements, and computer programs contain             //
//  unpublished proprietary information belongs to Y. Or�un G�KBULUT and they are   //
//  protected by international copyright laws. This file is intellectual property   //
//  of the Y. Or�un G�KBULUT. Storing this file, using this file, changing content  //
//  of this file, copying or duplicating this file, compiling this file, publishing //
//  this file to anykind media or network, editing  this file, selling this file,   //
//  booking this file, reading this file are probited without notice and written    //
//  permision of Y. Or�un G�KBULUT.                                                 //
//  These terms at above can only be changed by Y. Or�un G�KBULUT.                  //
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
//  AUTHOR(S)           : Y. Or�un G�KBULUT                                         //
//*                                                                                *//
//////////////////////////////////////////////////////////////////////////////////////

// Vertex Shader Input Struct
struct VS_INPUT
{
	float4 Position  : POSITION0;
	float2 Texcoord  : TEXCOORD0;

};

// Vertex Shader Output Struct
struct VS_OUTPUT 
{
	float4 	Position   : POSITION0;
	float2 	Texcoord   : TEXCOORD0;
};

// Pixel Shader Input Struct
struct PS_INPUT
{
	float2 	TexCoord  : TEXCOORD0;   
};

// Pixel Shader Output Struct
struct PS_OUTPUT
{
	float4 PixelColor : COLOR0;
};

/******************************************/
/*					Blur				  */
/******************************************/ 

sampler2D 			TextureInput	:	register(s0);
const float2 		PixelSize		:	register(vs, c0);
const float4		BlurKernel[7]	:	register(ps, c0);


VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;
	
	Output.Position = float4(sign(Input.Position).xy, 0.0f, 1.0f);
	Output.Texcoord.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
	Output.Texcoord.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);

	return Output;
}

PS_OUTPUT ps_main_horizontal( PS_INPUT Input )
{
	PS_OUTPUT Output;
	Output.PixelColor = (float4)0.0f;

	for (int I = 0; I < 7; I++)
	{
		Output.PixelColor += BlurKernel[I].x * tex2D(TextureInput, Input.TexCoord + float2(BlurKernel[I].y, 0.0f));
	}
	
	return Output;
}

PS_OUTPUT ps_main_vertical( PS_INPUT Input )
{	
	PS_OUTPUT Output;
	Output.PixelColor = (float4)0.0f;

	for (int I = 0; I < 7; I++)
	{
		Output.PixelColor += BlurKernel[I].x * tex2D(TextureInput, Input.TexCoord + float2(0.0f, BlurKernel[I].y));
	}
	
	return Output;
}

/******************************************/
/*              Color Blend               */
/******************************************/ 
// Uses vertex shader of blur pass

sampler2D TextureInputOne : register(s0);
sampler2D TextureInputTwo : register(s1);

float BlendFactor : register(ps, c0);

PS_OUTPUT ps_main_lerp( PS_INPUT Input )
{	
	PS_OUTPUT Output;
	Output.PixelColor = (float4)0.0f;

	float4 FirstColor = tex2D(TextureInputOne, Input.TexCoord);
	float4 SecondColor = tex2D(TextureInputTwo, Input.TexCoord);
	Output.PixelColor = lerp(FirstColor, SecondColor, BlendFactor);
	
	return Output;
}



