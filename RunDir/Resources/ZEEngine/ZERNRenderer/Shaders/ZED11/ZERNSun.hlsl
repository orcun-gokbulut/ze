//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSun.hlsl
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

#ifndef __ZERN_SUN_H__
#define __ZERN_SUN_H__

cbuffer ZERNSun_Constants				: register(b9)
{
	float2	ZERNSun_PositionScreen;
	float2	ZERNSun_SizeScreen;
	
	float3	ZERNSun_Color;
	float	ZERNSun_Reserved;
};

struct ZERNSun_VertexShader_Output
{
	float4 PositionProjection			: SV_Position;
	float2 PositionProjectionXY			: TEXCOORD0;
};

struct ZERNSun_PixelShader_Input
{
	float4 PositionProjection			: SV_Position;
	float2 PositionProjectionXY			: TEXCOORD0;
};

ZERNSun_VertexShader_Output ZERNSun_VertexShader_Main(uint VertexID : SV_VertexID)
{
	float2 Vertex = ((VertexID & uint2(2, 1)) << uint2(0, 1)) - 1.0f;
	
	float2 VertexPositionScreen = ZERNSun_PositionScreen + Vertex * ZERNSun_SizeScreen;
	
	ZERNSun_VertexShader_Output Output;
	Output.PositionProjection = float4(VertexPositionScreen, 0.0f, 1.0f);
	Output.PositionProjectionXY = VertexPositionScreen;
	
	return Output;
}

float3 ZERNSun_PixelShader_Main(ZERNSun_PixelShader_Input Input) : SV_Target0
{	
	float2 VectorScreen = (Input.PositionProjectionXY - ZERNSun_PositionScreen) / ZERNSun_SizeScreen;
	if (dot(VectorScreen, VectorScreen) <= 1.0f)
		return ZERNSun_Color;
	
	discard;
	return 0.0f;
}

#endif
