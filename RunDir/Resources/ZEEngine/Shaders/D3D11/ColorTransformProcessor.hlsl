//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ColorTransformProcessor.hlsl
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

// Samplers
sampler2D 	ColorTexture	 		: register(s6);

float2 		PixelSize				: register(c0);
float4x4	TransformMatrix			: register(c0);

struct VS_INPUT
{
	float4 Position		: POSITION0;
	float2 Texcoord		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 	Position	: POSITION0;
	float2 	Texcoord	: TEXCOORD0;
};

struct PS_INPUT
{
	float2 	TexCoord	: TEXCOORD0;   
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;
	
	Output.Position = float4(sign(Input.Position).xy, 0.0f, 1.0f);
	Output.Texcoord.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
	Output.Texcoord.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);

	return Output;
}

float4 ps_main( PS_INPUT Input ) : COLOR0
{	
	float4 SampleColor = tex2D(ColorTexture, Input.TexCoord);
	float4 TransformedColor = mul(TransformMatrix, SampleColor);

	return float4(TransformedColor.rgb, 1.0f);
}
