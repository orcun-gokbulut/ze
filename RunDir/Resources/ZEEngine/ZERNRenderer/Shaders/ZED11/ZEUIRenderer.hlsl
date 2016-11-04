//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIRenderer.hlsl
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

#ifndef __ZEUI_RENDERER_H__
#define __ZEUI_RENDERER_H__

#include "ZERNRenderer.hlsl"

Texture2D<float4>	ZEUIRenderer_Texture	: register(t0);
SamplerState		ZEUIRenderer_Sampler 	: register(s0);

struct ZEUIRenderer_VSInput 
{
	float2 Position             : POSITION0;
	float2 Texcoord             : TEXCOORD0;
	float4 Color                : TEXCOORD1;
};

struct ZEUIRenderer_VSOutput 
{
	float4 Position             : SV_Position;
	float2 Texcoord             : TEXCOORD0;
	float4 Color				: TEXCOORD1;
};

struct ZEUIRenderer_PSInput
{
	float4 Position				: SV_Position;
	float2 Texcoord             : TEXCOORD0;
	float4 Color			    : TEXCOORD1;
};

ZEUIRenderer_VSOutput ZEUIRenderer_VertexShader(ZEUIRenderer_VSInput Input)
{
	ZEUIRenderer_VSOutput Output;

	Output.Position = float4(mul((float3x3)ZERNRenderer_ScreenTransform, float3(Input.Position, 1.0f)).xy, 0.0f, 1.0f);
	Output.Texcoord = Input.Texcoord;
	Output.Color = Input.Color;
	
	return Output;
}

float4 ZEUIRenderer_PixelShader(ZEUIRenderer_PSInput Input) : SV_Target0
{
	uint Width;
	uint Height;
	ZEUIRenderer_Texture.GetDimensions(Width, Height);

	if (Width != 0)
		Input.Color *= ZEUIRenderer_Texture.Sample(ZEUIRenderer_Sampler, Input.Texcoord);

	return Input.Color;
}

float4 ZEUIRenderer_PixelShaderNonTextured(ZEUIRenderer_PSInput Input) : SV_Target0
{
	return Input.Color;
}

#endif
