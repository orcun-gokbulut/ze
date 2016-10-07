//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSkyBox.hlsl
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

#ifndef __ZERN_SKY_BOX_H__
#define __ZERN_SKY_BOX_H__

#include "ZERNTransformations.hlsl"
#include "ZERNSamplers.hlsl"

cbuffer ZERNSkyBox_Constants						: register(b9)
{
	float3			ZERNSkyBox_Color;
	float			ZERNSkyBox_Reserved;	
};

cbuffer ZERNSkyBox_Constants_Transform				: register(ZERN_SHADER_CONSTANT_DRAW_TRANSFORM)
{
	float4x4		ZERNSkyBox_WorldTransform;
};

TextureCube<float3>	ZERNSkyBox_SkyTexture			: register(t5);

struct ZERNSkyBox_VertexShader_Input
{
	float3	Position			: POSITION0;
	float3	Normal				: NORMAL0;
	float2	Texcoord			: TEXCOORD0;
	float4	Color				: COLOR0;
};

struct ZERNSkyBox_PixelShader_Input
{
	float4			Position						: SV_Position;
	float3			CubeTexcoord					: TEXCOORD0;
};

ZERNSkyBox_PixelShader_Input ZERNSkyBox_VertexShader_Main(ZERNSkyBox_VertexShader_Input Input)
{
	ZERNSkyBox_PixelShader_Input Output;

	float4 PositionWorld = mul(ZERNSkyBox_WorldTransform, float4(Input.Position, 1.0f));
	Output.Position = ZERNTransformations_WorldToProjection(PositionWorld);
	Output.Position.z = 0.0f;
	Output.CubeTexcoord = Input.Position;
	
	return Output;
}

float3 ZERNSkyBox_PixelShader_Main(ZERNSkyBox_PixelShader_Input Input) : SV_Target0
{
	return ZERNSkyBox_Color * ZERNSkyBox_SkyTexture.SampleLevel(ZERNSampler_LinearWrap, Input.CubeTexcoord, 0.0f);
}

#endif
