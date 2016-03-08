//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSunMaterial.hlsl
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

#ifndef __ZERN_SUN_MATERIAL_H__
#define __ZERN_SUN_MATERIAL_H__

#include "ZERNTransformations.hlsl"

cbuffer ZERNSunMaterial_Constants	: register(b8)
{
	float2	ZERNSunMaterial_SunPositionScreen;
	float2	ZERNSunMaterial_SunSizeScreen;
};

struct ZERNSunMaterial_RenderSun_VertexShader_Output
{
	float4 PositionProjection	: SV_Position;
	float2 PositionProjectionXY	: TEXCOORD0;
};

struct ZERNSunMaterial_RenderSun_PixelShader_Input
{
	float4 PositionProjection	: SV_Position;
	float2 PositionProjectionXY	: TEXCOORD0;
};

ZERNSunMaterial_RenderSun_VertexShader_Output ZERNSunMaterial_RenderSun_VertexShader_Main(uint VertexID : SV_VertexID)
{
	float2 Vertex = ((VertexID & uint2(2, 1)) << uint2(0, 1)) - 1.0f;
	
	float2 SunVertexPositionScreen = ZERNSunMaterial_SunPositionScreen + Vertex * ZERNSunMaterial_SunSizeScreen;
	
	ZERNSunMaterial_RenderSun_VertexShader_Output Output;
	Output.PositionProjection = float4(SunVertexPositionScreen, 0.0f, 1.0f);
	Output.PositionProjectionXY = SunVertexPositionScreen;
	
	return Output;
}

float4 ZERNSunMaterial_RenderSun_PixelShader_Main(ZERNSunMaterial_RenderSun_PixelShader_Input Input) : SV_Target0
{	
	float2 VectorScreen = (Input.PositionProjectionXY - ZERNSunMaterial_SunPositionScreen) / ZERNSunMaterial_SunSizeScreen;
	return dot(VectorScreen, VectorScreen) <= 1.0f ? 1.0f : 0.0f;
}

#endif
