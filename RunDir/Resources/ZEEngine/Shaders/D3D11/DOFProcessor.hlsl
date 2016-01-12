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
// -------------------------------------------------------------------------------//
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
sampler2D 						ColorBuffer 			: register(s0);
sampler2D 						DepthBuffer 			: register(s1);
sampler2D 						ColorBufferBlurred		: register(s2);

float2							PixelSize				: register(c0);
float4							DOFParameters0			: register(ps, c0);
float4							DOFParameters1			: register(ps, c1);

#define NearClamp				DOFParameters0.x
#define NearDistance			DOFParameters0.y
#define FarClamp				DOFParameters0.z
#define FarDistance				DOFParameters0.w
#define FocusDistance			DOFParameters1.x
#define FullFocusRange			DOFParameters1.y

// Vertex Shader Input Struct
struct VS_INPUT
{
	float3 Position  : POSITION0;
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

/************************************************************/
/*						Vertex Shader Common				*/
/************************************************************/

VS_OUTPUT vs_main_common( VS_INPUT Input )
{
	VS_OUTPUT Output = (VS_OUTPUT)0.0f;
	
	Output.Position = float4(sign(Input.Position).xy, 0.0f, 1.0f);
	Output.Texcoord.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
	Output.Texcoord.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);

	return Output;
}

/************************************************************/
/*					Down/Up sample and blur					*/
/************************************************************/

static const float2 SampleOffsets[9] = 
{
	{-1.0f, -1.0f},	{+0.0f, -1.0f},	{+1.0f, -1.0f},
	{-1.0f, +0.0f},	{+0.0f, +0.0f},	{+1.0f, +0.0f},
	{-1.0f, +1.0f},	{+0.0f, +1.0f},	{+1.0f, +1.0f}
};

PS_OUTPUT ps_main_down_up_sample_blur( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	for (int I  = 0; I < 9; ++I)
	{
		Output.PixelColor += tex2D(ColorBuffer, Input.TexCoord + SampleOffsets[I] * PixelSize);
	}
	Output.PixelColor /= 9.0f ;

	return Output;
}

/************************************************************/
/*						Depth of Field						*/
/************************************************************/

PS_OUTPUT ps_main_dof( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;
	
	float4	Color	= tex2D(ColorBuffer, Input.TexCoord);
	float	Depth	= tex2D(DepthBuffer, Input.TexCoord).r;
	float4	Blurred	= tex2D(ColorBufferBlurred, Input.TexCoord);

	float HalfFocusRange = FullFocusRange / 2.0f;
	float FullFocusEnd = FocusDistance + HalfFocusRange;
	float FullFocusStart = FocusDistance - HalfFocusRange;
	
	float BlurFactor = 0.0f;
	if (Depth < FullFocusStart)
	{
		BlurFactor = (FullFocusStart - Depth) / (FullFocusStart - NearDistance);
		BlurFactor = clamp(BlurFactor, 0, NearClamp);
	}
	if(Depth > FullFocusEnd)
	{
		BlurFactor = (Depth - FullFocusEnd) / (FarDistance - FullFocusEnd);
		BlurFactor = clamp(BlurFactor, 0, FarClamp);
	}
	
	Output.PixelColor = lerp(Color, Blurred, BlurFactor);

	return Output;
}
