//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Downsample2X.hlsl
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

const float2	PixelSize;

sampler2D 		RenderTarget;

struct VS_INPUT
{
   float4 Position  : POSITION;
};

// Texture Coordinates
// [0] [1]
// [2] [3]

struct VS_OUTPUT 
{
   float4 Position   : POSITION0;
   float2 TexCoord0  : TEXCOORD0;
   float2 TexCoord1  : TEXCOORD1;
   float2 TexCoord2  : TEXCOORD2;
   float2 TexCoord3  : TEXCOORD3;
};

struct PS_INPUT
{
   float2 TexCoord0  : TEXCOORD0;
   float2 TexCoord1  : TEXCOORD1;
   float2 TexCoord2  : TEXCOORD2;
   float2 TexCoord3  : TEXCOORD3;
};

struct PS_OUTPUT
{
   float4 PixelColor : COLOR0;
};


VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;

   Output.Position = float4(sign(Input.Position).xy, 0.0f, 1.0f);
   
   Output.TexCoord0.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
   Output.TexCoord0.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);
   
   Output.TexCoord1.x = Output.TexCoord0.x + PixelSize.x;
   Output.TexCoord1.y = Output.TexCoord0.y - 0.0f;
   
   Output.TexCoord2.x = Output.TexCoord0.x + 0.0f;
   Output.TexCoord2.y = Output.TexCoord0.y - PixelSize.y;
   
   Output.TexCoord3.x = Output.TexCoord0.x + PixelSize.x;
   Output.TexCoord3.y = Output.TexCoord0.y - PixelSize.y;
   
   return Output;
}


PS_OUTPUT ps_main( PS_INPUT Input )
{
   PS_OUTPUT Output;
   
   float4 PixelColorSum = (float4)0.0f;
   
   PixelColorSum += tex2D(RenderTarget, Input.TexCoord0);
   PixelColorSum += tex2D(RenderTarget, Input.TexCoord1);
   PixelColorSum += tex2D(RenderTarget, Input.TexCoord2);
   PixelColorSum += tex2D(RenderTarget, Input.TexCoord3);
   
   Output.PixelColor = PixelColorSum * 0.25f;
   
   return Output;
}

