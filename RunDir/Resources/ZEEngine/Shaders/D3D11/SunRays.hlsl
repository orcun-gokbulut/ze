//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SunRays.hlsl
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

/************************************************************/
/*					Shader Constants						*/
/************************************************************/
sampler2D	InputTexture : register(s0);
float4		Parameter0	 : register(c0);
float4		Parameter1	 : register(c1);

// Vertex Shader Constants
#define PixelSizeHalf Parameter0.xy
#define SunPositionScreen Parameter0.zw
#define Density Parameter1.x

// Pixel Shader Constants
#define Intensity Parameter0.x

/************************************************************/
/*				Shader Input Output Structures				*/
/************************************************************/
struct VSInput
{
	float3 Position	: POSITION0;
};

struct VSOutput
{
	float4 Position  : POSITION0;
	float4 Texcoord0 : TEXCOORD0;
	float4 Texcoord1 : TEXCOORD1;
};

struct PSInput
{
	float4 Texcoord0 : TEXCOORD0;
	float4 Texcoord1 : TEXCOORD1;
};

struct PSOutput
{
	float4 Color	: COLOR0;
};


/************************************************************/
/*						Vertex Shader Common				*/
/************************************************************/
VSOutput VSMain(VSInput Input)
{
	VSOutput Output = (VSOutput)0.0f;

	Output.Position = float4(sign(Input.Position).xy, 0.0f, 1.0f);

	float2 PositionTexture = Output.Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) - PixelSizeHalf;
	float2 SunPositionTexture = SunPositionScreen * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) - PixelSizeHalf;

	float2 Displacement = (PositionTexture - SunPositionTexture) / Density;
	
	Output.Texcoord0.xy = SunPositionTexture + Displacement * 0.75f;
	Output.Texcoord0.zw = SunPositionTexture + Displacement * 0.50f;
	Output.Texcoord1.xy = SunPositionTexture + Displacement * 0.25f;
	Output.Texcoord1.zw = SunPositionTexture + Displacement;

	return Output;
}


/************************************************************/
/*				Pixel Shader Blur Horizontal				*/
/************************************************************/
float4 PSMain(PSInput Input) : COLOR0
{
	float4 Output;
	Output =  1.00f * tex2D(InputTexture, Input.Texcoord0.xy);
	Output += 0.75f * tex2D(InputTexture, Input.Texcoord0.zw);
	Output += 0.50f * tex2D(InputTexture, Input.Texcoord1.xy);
	Output += 0.25f * tex2D(InputTexture, Input.Texcoord1.zw);
	return Intensity * Output * 0.25f;
}
