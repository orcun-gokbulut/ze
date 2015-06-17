//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - BlurMaskProcessor.hlsl
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
sampler2D	BlurBuffer		: register (s2);
sampler2D 	BlurMaskBuffer	: register (s1);

float4 		VSParameters	: register(vs, c0);
// Pass 1-2-3-4
#define		PixelSize		VSParameters.xy

float4		PSParameters	: register(ps, c0);

// Pass 4: mask
#define		BlurFactor		PSParameters.x
#define		BlurColor		PSParameters.yzw

// Pass 2: Blur
#define		PixelSizeBlur	PSParameters.xy


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

VS_OUTPUT vs_main_common(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0.0f;
   
	Output.Position	= sign(Input.Position);

	Output.TexCoord.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
	Output.TexCoord.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);

	return Output;
}

// Used for up / down sampling.
PS_OUTPUT ps_main_sample( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	Output.PixelColor = tex2D(ColorBuffer, Input.TexCoord);

	return Output;
}

static const float4 Weights[9] = 
{
	-1.0f, -1.0f, 0.0f, 1.0f / 9.0f,
	 0.0f, -1.0f, 0.0f, 1.0f / 9.0f,
	 1.0f, -1.0f, 0.0f, 1.0f / 9.0f,

	-1.0f,  0.0f, 0.0f, 1.0f / 9.0f,
	 0.0f,  0.0f, 0.0f, 1.0f / 9.0f,
	 1.0f,  0.0f, 0.0f, 1.0f / 9.0f,

	-1.0f,  1.0f, 0.0f, 1.0f / 9.0f,
	 0.0f,  1.0f, 0.0f, 1.0f / 9.0f,
	 1.0f,  1.0f, 0.0f, 1.0f / 9.0f,
};

// Used for blurring
PS_OUTPUT ps_main_blur( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	for (int I = 0; I < 9; ++I)
	{
		Output.PixelColor += tex2D(ColorBuffer, Input.TexCoord + Weights[I].xy * PixelSizeBlur) * Weights[I].w;
	}

	return Output;
}

PS_OUTPUT ps_main_blur_mask( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	float4 ColorSample = tex2D(ColorBuffer, Input.TexCoord);
	float4 BlurredColor = tex2D(BlurBuffer, Input.TexCoord);
	float4 WeightSample = tex2D(BlurMaskBuffer, Input.TexCoord);
	/*
	[branch]
	if (WeightSample >= 0.999f)
	{
		Output.PixelColor = ColorSample;
		return Output;
	}
	*/
	BlurredColor.xyz *= BlurColor.xyz;
	float4 WeightedBlur = lerp(ColorSample, BlurredColor, 1.0f - WeightSample.x);
	Output.PixelColor = lerp(ColorSample, WeightedBlur, BlurFactor);

	return Output;
}
