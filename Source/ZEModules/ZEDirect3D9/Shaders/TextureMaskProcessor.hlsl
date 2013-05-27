//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - TextureMaskProcessor.hlsl
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
sampler2D 	MaskBuffer		: register (s1);

float4 		VSParameters	: register(vs, c0);

#define		PixelSizeRT		VSParameters.xy
#define		PixelSizeMask	VSParameters.zw

float4		PSParameters	: register(ps, c0);

#define		MaskFactor		PSParameters.x


struct VS_INPUT
{
	float4 Position	: POSITION0;
	float2 TexCoord : TEXCOORD0;
	
};

struct VS_OUTPUT 
{
	float4 Position : POSITION0;
	float2 TexCoordColor : TEXCOORD0;
	float2 TexCoordMask : TEXCOORD1;
};

struct PS_INPUT
{
	float2 TexCoordColor : TEXCOORD0;
	float2 TexCoordMask : TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 PixelColor : COLOR0;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0.0f;
   
	Output.Position	= sign(Input.Position);

	Output.TexCoordColor.x = 0.5f * (1.0f + Output.Position.x + PixelSizeRT.x);
	Output.TexCoordColor.y = 0.5f * (1.0f - Output.Position.y + PixelSizeRT.y);
	
	Output.TexCoordMask = Input.TexCoord + (PixelSizeMask * 0.5f);

	return Output;
}

PS_OUTPUT ps_main( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	Output.PixelColor = tex2D(ColorBuffer, Input.TexCoordColor);
	float3 Masked = tex2D(MaskBuffer, Input.TexCoordMask).rgb * Output.PixelColor.rgb;
	
	Output.PixelColor.rgb = lerp(Output.PixelColor.rgb, Masked, MaskFactor);

	return Output;
}
