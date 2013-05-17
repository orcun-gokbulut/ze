//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ChannelDisorientationProcessor.hlsl
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

sampler2D 	ColorBuffer		: register (s0);

float2 		VSParameters	: register(vs, c0);

#define		PixelSize		VSParameters.xy

float4 		PSParameters0	: register(ps, c0);
float4		PSParameters1	: register(ps, c1);
float4		PSParameters2	: register(ps, c2);

#define		RedOffset		PSParameters0.xy
#define		GreenOffset		PSParameters0.zw
#define		BlueOffset		PSParameters1.xy
#define		AlphaOffset		PSParameters1.zw


// Vertex Shader
struct VS_INPUT
{
	float4 Position	: POSITION0;
	
};

struct VS_OUTPUT 
{
	float4 Position	: POSITION0;
	float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
	float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 PixelColor : COLOR0;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0.0f;
   
	Output.Position	= sign(Input.Position);

	Output.TexCoord.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
	Output.TexCoord.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);
	
	return Output;
}


PS_OUTPUT ps_main( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	Output.PixelColor.r = tex2D(ColorBuffer, Input.TexCoord + RedOffset).r;
	Output.PixelColor.g = tex2D(ColorBuffer, Input.TexCoord + GreenOffset).g;
	Output.PixelColor.b = tex2D(ColorBuffer, Input.TexCoord + BlueOffset).b;
	Output.PixelColor.a = tex2D(ColorBuffer, Input.TexCoord + AlphaOffset).a;
	
	return Output;
}
