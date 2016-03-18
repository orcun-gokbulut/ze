//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNDebug.hlsl
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

#ifndef __ZERN_DEBUG_H__
#define __ZERN_DEBUG_H__

#include "ZERNTransformations.hlsl"

struct ZERNDebug_VertexShader_Input
{
	float3		Position	: POSITION0;
	float3		Normal		: NORMAL0;	
};

struct ZERNDebug_GeometryShader_Input
{
	float3		Position	: POSITION0;
	float3		Normal		: NORMAL0;
};

struct ZERNDebug_PixelShader_Input
{
	float4		Position	: SV_Position;
	float3		Color		: COLOR0;
};

cbuffer ZERNDebug_Constant_Draw_Transform	: register(ZERN_SHADER_CONSTANT_DRAW_TRANSFORM)
{
	float4x4	ZERNDebug_WorldTransform;
};

cbuffer ZERNDebug_Constants					: register(b8)
{
	bool		ZERNDebug_ShowNormalVectors;
	bool		ZERNDebug_ShowBoundingBox;
	bool		ZERNDebug_ShowWireframe;
	bool		ZERNDebug_CullBackface;
};

ZERNDebug_GeometryShader_Input ZERNDebug_VertexShader_Main(ZERNDebug_VertexShader_Input Input)
{
	ZERNDebug_GeometryShader_Input Output;
	
	Output.Position = mul(ZERNDebug_WorldTransform, float4(Input.Position, 1.0f)).xyz;
	Output.Normal = mul(ZERNDebug_WorldTransform, float4(Input.Normal, 0.0f)).xyz;
	
	return Output;
}

[maxvertexcount(9)]
void ZERNDebug_GeometryShader_Main(triangle ZERNDebug_GeometryShader_Input Input[3], inout LineStream<ZERNDebug_PixelShader_Input> OutputStream)
{
	if (ZERNDebug_CullBackface)
	{
		float3 SurfaceNormal = normalize(cross(Input[1].Position - Input[0].Position, Input[2].Position - Input[0].Position));
		float3 SurfacePosition = 0.333333f * (Input[0].Position + Input[1].Position + Input[2].Position);
		float3 ViewDirection = normalize(ZERNView_Position - SurfacePosition);
		
		if (dot(SurfaceNormal, ViewDirection) <= 0.0f)
			return;
	}
	
	ZERNDebug_PixelShader_Input Output;
	
	if (ZERNDebug_ShowNormalVectors)
	{
		Output.Color = float3(0.0f, 1.0f, 0.0f);
		
		float LineLength = 0.2f;
		
		float3 LineStartWorld = Input[0].Position;
		float3 LineEndWorld = LineStartWorld + normalize(Input[0].Normal) * LineLength;
		
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();
		
		LineStartWorld = Input[1].Position;
		LineEndWorld = LineStartWorld + normalize(Input[1].Normal) * LineLength;
		
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();
		
		LineStartWorld = Input[2].Position;
		LineEndWorld = LineStartWorld + normalize(Input[2].Normal) * LineLength;
		
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();
	}
	
	if (ZERNDebug_ShowWireframe)
	{
		Output.Color = float3(0.0f, 0.0f, 1.0f);
		
		Output.Position = ZERNTransformations_WorldToProjection(float4(Input[0].Position, 1.0f));
		OutputStream.Append(Output);
		
		Output.Position = ZERNTransformations_WorldToProjection(float4(Input[1].Position, 1.0f));
		OutputStream.Append(Output);
		
		Output.Position = ZERNTransformations_WorldToProjection(float4(Input[2].Position, 1.0f));
		OutputStream.Append(Output);
		
		OutputStream.RestartStrip();
	}
}

float3 ZERNDebug_BoundingBox_VertexShader_Main(float3 MinMax : POSITION0) : POSITION0
{
	return MinMax;
}

[maxvertexcount(18)]
void ZERNDebug_BoundingBox_GeometryShader_Main(line float3 MinMax[2] : POSITION0, inout LineStream<ZERNDebug_PixelShader_Input> OutputStream)
{
	ZERNDebug_PixelShader_Input Output;
	
	Output.Color = float3(1.0f, 0.0f, 0.0f);
	
	float3 Min = MinMax[0];
	float3 Max = MinMax[1];
	
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Min.x, Min.y, Min.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Min.x, Max.y, Min.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Max.x, Max.y, Min.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Max.x, Min.y, Min.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Min.x, Min.y, Min.z), 1.0f));
	OutputStream.Append(Output);
	OutputStream.RestartStrip();
	
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Min.x, Min.y, Max.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Min.x, Max.y, Max.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Max.x, Max.y, Max.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Max.x, Min.y, Max.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Min.x, Min.y, Max.z), 1.0f));
	OutputStream.Append(Output);
	OutputStream.RestartStrip();
	
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Min.x, Max.y, Min.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Min.x, Max.y, Max.z), 1.0f));
	OutputStream.Append(Output);
	OutputStream.RestartStrip();
	
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Max.x, Max.y, Min.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Max.x, Max.y, Max.z), 1.0f));
	OutputStream.Append(Output);
	OutputStream.RestartStrip();
	
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Max.x, Min.y, Min.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Max.x, Min.y, Max.z), 1.0f));
	OutputStream.Append(Output);
	OutputStream.RestartStrip();
	
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Min.x, Min.y, Min.z), 1.0f));
	OutputStream.Append(Output);
	Output.Position = ZERNTransformations_WorldToProjection(float4(float3(Min.x, Min.y, Max.z), 1.0f));
	OutputStream.Append(Output);
	OutputStream.RestartStrip();
}

float3 ZERNDebug_PixelShader_Main(ZERNDebug_PixelShader_Input Input) : SV_Target0
{
	return Input.Color;
}

#endif
