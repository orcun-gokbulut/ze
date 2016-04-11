//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNMoon.hlsl
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

#ifndef __ZERN_MOON_H__
#define __ZERN_MOON_H__

cbuffer ZERNMoon_Constants						: register(b8)
{
	float2		ZERNMoon_PositionScreen;
	float2		ZERNMoon_SizeScreen;
	
	float		ZERNMoon_Phase;
	float3		ZERNMoon_Color;
};

struct ZERNMoon_VertexShader_Output
{
	float4		PositionProjection				: SV_Position;
	float2		PositionProjectionXY			: TEXCOORD0;
	float2		Texcoord						: TEXCOORD1;
};

struct ZERNMoon_PixelShader_Input
{
	float4		PositionProjection				: SV_Position;
	float2		PositionProjectionXY			: TEXCOORD0;
	float2		Texcoord						: TEXCOORD1;
};

SamplerState	ZERNMoon_SamplerLinearClamp		: register(s0);
Texture3D		ZERNMoon_MoonTexture			: register(t5);

ZERNMoon_VertexShader_Output ZERNMoon_VertexShader_Main(uint VertexID : SV_VertexID)
{
	float2 Vertex = ((VertexID & uint2(2, 1)) << uint2(0, 1)) - 1.0f;
	
	float2 VertexPositionScreen = ZERNMoon_PositionScreen + Vertex * ZERNMoon_SizeScreen;
	
	ZERNMoon_VertexShader_Output Output;
	Output.PositionProjection = float4(VertexPositionScreen, 0.0f, 1.0f);
	Output.PositionProjectionXY = VertexPositionScreen;
	Output.Texcoord = (uint2(VertexID, VertexID + 1) & uint2(2, 1)) >> uint2(1, 0);
	
	return Output;
}

float3 ZERNMoon_PixelShader_Main(ZERNMoon_PixelShader_Input Input) : SV_Target0
{	
	float4 PixelColor = ZERNMoon_MoonTexture.SampleLevel(ZERNMoon_SamplerLinearClamp, float3(Input.Texcoord, ZERNMoon_Phase), 0.0f);
	
	clip(PixelColor.a - 0.1f);
	
	return ZERNMoon_Color * PixelColor.rgb;
}

#endif
