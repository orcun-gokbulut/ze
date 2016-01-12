//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - TextureResizeProcessor.hlsl
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

sampler2D 	TextureInput	 : register(s0);
float2		PixelSize 		 : register(vs, c0);
float2		Weights[21] 	 : register(ps, c0);

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

// Vertex Shader Main
VS_OUTPUT vs_main_generic( VS_INPUT Input )
{
	VS_OUTPUT Output;

	Output.Position = Input.Position;
	Output.Texcoord = Input.Texcoord + 0.5f * PixelSize;

	return Output;
}

// Pixel Shader Main horizontal
// Samples in x axis
PS_OUTPUT ps_main_horizontal( PS_INPUT Input )
{
	PS_OUTPUT Output;
	
	Output.PixelColor = (float4)0.0f;

	for(int I = 0; I < 21; I++)
	{
		Output.PixelColor += Weights[I].x * tex2D(TextureInput, Input.TexCoord + float2(Weights[I].y, 0.0f));
	}

	return Output;
}


// Pixel Shader Main vertical
// Samples in y axis
PS_OUTPUT ps_main_vertical( PS_INPUT Input )
{
	PS_OUTPUT Output;
	
	Output.PixelColor = (float4)0.0f;
   
	for(int I = 0; I < 21; I++)
	{
		Output.PixelColor += Weights[I].x * tex2D(TextureInput, Input.TexCoord + float2(0.0f, Weights[I].y));
	}

	return Output;
}
