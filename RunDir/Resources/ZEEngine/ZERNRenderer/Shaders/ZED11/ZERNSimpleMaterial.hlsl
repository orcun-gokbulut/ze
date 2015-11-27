//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSimpleMaterial.hlsl
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

#ifndef __ZERN_SIMPLE_MATERIAL_H__
#define __ZERN_SIMPLE_MATERIAL_H__

#include "ZERNView.hlsl"
#include "ZERNShaderSlots.hlsl"

// SHADER RESOURCES
///////////////////////////////////////////////////////////////////////////////

cbuffer ZERNSimpleMaterial_Constants : register(ZERN_SHADER_CONSTANT_MATERIAL)
{
	float4 	ZERNSimpleMaterial_Color;
	bool 	ZERNSimpleMaterial_EnableTexture;
	float	ZERNSimpleMaterial_Reserved0;
	bool 	ZERNSimpleMaterial_EnableVertexColor;
	float	ZERNSimpleMaterial_Reserved1;
};

cbuffer ZERNSimpleMaterial_InstanceConstants : register(ZERN_SHADER_CONSTANT_DRAW_TRANSFORM)
{
	float4x4 ZERNSimpleMaterial_WorldTransform;
};

sampler		ZERNSimpleMaterial_Sampler			: register(s0);
Texture2D<float4> ZERNSimpleMaterial_Texture	: register(t0);


// INPUT OUTPUTS
///////////////////////////////////////////////////////////////////////////////

struct ZERNSimpleMaterial_VSInput 
{
	float3 Position			: POSITION0;
	float2 Textcoord		: TEXCOORD0;
	float3 Normal			: NORMAL0;
	float4 Color			: COLOR0;
};

struct ZERNSimpleMaterial_VSOutput 
{
	float4 Position			: SV_Position;
	float2 Texcoord			: TEXCOORD0;
	float4 Color			: COLOR0;
};

struct ZERNSimpleMaterial_PSInput
{
	float4 Position			: SV_Position;
	float2 Texcoord			: TEXCOORD0;
	float4 Color		    : COLOR0;
};


// FORWARD STAGE - VERTEX SHADER
///////////////////////////////////////////////////////////////////////////////

ZERNSimpleMaterial_VSOutput ZERNSimpleMaterial_VSMain_ForwardStage(ZERNSimpleMaterial_VSInput Input)
{
	ZERNSimpleMaterial_VSOutput Output;
	
	float4x4 WorldViewProjectionTransform = mul(ZERNView_ViewProjectionTransform, ZERNSimpleMaterial_WorldTransform);
	Output.Position = mul(WorldViewProjectionTransform, float4(Input.Position, 1.0f));
	Output.Texcoord = Input.Textcoord;
	
	Output.Color = ZERNSimpleMaterial_Color;
	
	if (ZERNSimpleMaterial_EnableVertexColor)
		Output.Color *= Input.Color;
	
	return Output;
}


// FORWARD STAGE - PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float4 ZERNSimpleMaterial_PSMain_ForwardStage(ZERNSimpleMaterial_PSInput Input) : SV_Target0
{
	float4 Color = Input.Color;

	if (ZERNSimpleMaterial_EnableTexture)
		Color *= ZERNSimpleMaterial_Texture.Sample(ZERNSimpleMaterial_Sampler, Input.Texcoord);	
		
	return Color;
}

#endif
