//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZoomBlurProcessor.hlsl
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



// PV Parameters
float4 		VSParameters	: register(vs, c0);

#define		PixelSize		VSParameters.xy


// PS Parameters
float4		PSParameters0	: register(ps, c0);
float4		PSParameters1	: register(ps, c1);

#define		Start				PSParameters0.x
#define		Strength			PSParameters0.y
#define		TransitionOffset	PSParameters0.z

#define		Center				PSParameters1.xy


struct VS_INPUT
{
	float4 Position	: POSITION0;
	float2 TexCoord : TEXCOORD0;
	
};

struct VS_OUTPUT 
{
	float4 Position : POSITION0;
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


static const float OffsetTable[16] = 
{
   0.0f / 15.0f,
   1.0f / 15.0f,
   2.0f / 15.0f,
   3.0f / 15.0f,
   4.0f / 15.0f,
   5.0f / 15.0f,
   6.0f / 15.0f,
   7.0f / 15.0f,
   8.0f / 15.0f,
   9.0f / 15.0f,
   10.0f / 15.0f,
   11.0f / 15.0f,
   12.0f / 15.0f,
   13.0f / 15.0f,
   14.0f / 15.0f,
   15.0f / 15.0f,
};

PS_OUTPUT ps_main( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	// Get center to pixel vector
	float2 BlurDir = Input.TexCoord - Center;
 
	float Distance = length(BlurDir);
	float4 PixColor = tex2D(ColorBuffer, Input.TexCoord);
   
	[branch]
	if (Distance < Start)
	{
		Output.PixelColor = PixColor;
		return Output;
	}
 
	for(int I = 0; I < 16; I++)
	{
		float SampleDistance = (1.0f - Strength) + Strength * OffsetTable[I];
     
		float2 Coord = Center + BlurDir * SampleDistance; 
		Output.PixelColor += tex2D(ColorBuffer, Coord);
	}
 
	Output.PixelColor /= 16.0f;
 
	float LerpAmount =  saturate((Distance - Start) / TransitionOffset);
	Output.PixelColor = lerp(PixColor, Output.PixelColor, LerpAmount);

	return Output;
}
