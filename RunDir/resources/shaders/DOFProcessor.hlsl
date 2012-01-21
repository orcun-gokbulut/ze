//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - DOFProcessor.hlsl
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 YiÄŸit OrÃ§un GÃ–KBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "YiÄŸit  OrÃ§un  GÃ–KBULUT") and
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
  Name: YiÄŸit OrÃ§un GÃ–KBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  DOFProcessor.hlsl - Zinek Engine v0.05.00 Build 1024 Source Code				//
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

// Common parameters
sampler2D 		ColorBuffer 	: register(s0);		// Color buffer both used by blur and dof


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

/**************************	  Common vertex shader	  *****************************/

const float2	PixelSize		: register(vs, c0);			// Color buffer pixel size

// Vertex Shader Main
VS_OUTPUT vs_main_common( VS_INPUT Input )
{
	VS_OUTPUT Output;
	
	Output.Position = float4(sign(Input.Position).xy, 0.0f, 1.0f);
	Output.Texcoord.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
	Output.Texcoord.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);

	return Output;
}

/*************************	 Down sample pixel shaders	  *************************/

PS_OUTPUT ps_main_ds2x( PS_INPUT Input )
{
	PS_OUTPUT Output;
	Output.PixelColor = (float4)0.0f;

	Output.PixelColor = tex2D(ColorBuffer, Input.TexCoord);
	return Output;
}



/****************************	Gaussian blur shaders	***************************/

const float4		BlurKernel[7]	:	register(ps, c0);	// Kernel for gaussian blur

// Horizontal pass for gaussian blur
PS_OUTPUT ps_main_gauss_blur_horizontal( PS_INPUT Input )
{
	PS_OUTPUT Output;
	Output.PixelColor = (float4)0.0f;

	for (int I = 0; I < 7; I++)
	{
		Output.PixelColor += BlurKernel[I].x * tex2D(ColorBuffer, Input.TexCoord + float2(BlurKernel[I].y, 0.0f));
	}
	
	return Output;
}

// Vertical pass for gaussian blur
PS_OUTPUT ps_main_gauss_blur_vertical( PS_INPUT Input )
{	
	PS_OUTPUT Output;
	Output.PixelColor = (float4)0.0f;

	for (int I = 0; I < 7; I++)
	{
		Output.PixelColor += BlurKernel[I].x * tex2D(ColorBuffer, Input.TexCoord + float2(0.0f, BlurKernel[I].y));
	}
	
	return Output;
}

/*****************************	Dof pixel shaders	*******************************/

sampler2D 		DepthBuffer 			: register(s1);		// Depth buffer
sampler2D 		ColorBufferLowResBlur	: register(s2);		// 1/4 sized blurred color buffer

const float		FocusDistance	: register(ps, c0);
const float		NearDistance	: register(ps, c1);
const float		FarDistance		: register(ps, c2);
const float		NearClamp		: register(ps, c3);
const float		FarClamp		: register(ps, c4);


// Pixel shader main
PS_OUTPUT ps_main_dof( PS_INPUT Input )
{
	PS_OUTPUT Output;
	Output.PixelColor = (float4)0.0f;
	
	float	Depth			= tex2D(DepthBuffer, Input.TexCoord).r;
	float4	Color			= tex2D(ColorBuffer, Input.TexCoord);
	float4	ColorBlurLowRes = tex2D(ColorBufferLowResBlur, Input.TexCoord);
	
	//float BlurFactor = saturate(abs(Depth - FocusDistance) / FocusRange);
	float BlurFactor = 0.0f;
	
	if (Depth < FocusDistance)
	{
		BlurFactor = (FocusDistance - Depth) / (FocusDistance - NearDistance);
		BlurFactor = clamp(BlurFactor, 0, NearClamp);
	}
	else
	{
		BlurFactor = (Depth - FocusDistance) / (FarDistance - FocusDistance);
		BlurFactor = clamp(BlurFactor, 0, FarClamp);
	}
	
	Output.PixelColor = lerp(Color, ColorBlurLowRes, BlurFactor);

	return Output;
}

