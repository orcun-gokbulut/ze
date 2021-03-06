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
#include "ZERNDecodeNormals.hlsl"
#include "ZERNSkin.hlsl"

struct ZERNDebug_VertexShader_Input
{
	float3 Position			: POSITION0;

	#ifdef ZERN_DEBUG_SKIN_TRANSFORM
		uint4 BoneIndices	: BLENDINDICES;
	#endif

	int2 NormalEncoded		: NORMAL0;
	int2 TangentEncoded		: TANGENT0;
	float2 Texcoord			: TEXCOORD0;

	#ifdef ZERN_DEBUG_SKIN_TRANSFORM
		float4 BoneWeights	: BLENDWEIGHTS;
	#endif
};

struct ZERNDebug_GeometryShader_Input
{
	float3		Position	: POSITION0;
	float3		Normal		: NORMAL0;
	float3		Tangent		: TANGENT0;
	float3		Binormal	: BINORMAL0;
};

struct ZERNDebug_PixelShader_Input
{
	float4		Position	: SV_Position;
	float3		Color		: COLOR0;
};

struct ZERNDebug_CanvasVertex
{
	float3		Position	: POSITION0;
	float3		Normal		: NORMAL0;
	float2		Texcoord	: TEXCOORD0;
	float4		Color		: COLOR0;
};

cbuffer ZERNDebug_Constant_Draw_Transform	: register(ZERN_SHADER_CONSTANT_DRAW_TRANSFORM)
{
	float4x4	ZERNDebug_WorldTransform;
	float4x4	ZERNDebug_WorldTransformInverseTranspose;
	float4x4	ZERNDebug_PreSkinTransform;
	float4		ZERNDebug_ClippingPlane0;
	float4		ZERNDebug_ClippingPlane1;
	float4		ZERNDebug_ClippingPlane2;
	float4		ZERNDebug_ClippingPlane3;
};

cbuffer ZERNDebug_Constants					: register(b8)
{
	bool		ZERNDebug_ShowNormalVectors;
	bool		ZERNDebug_ShowBoundingBox;
	bool		ZERNDebug_ShowWireframe;
	bool		ZERNDebug_CullBackface;
};

static const float3 ZERNDebug_ColorWireframe	= float3(0.0f, 0.0f, 1.0f);
static const float3 ZERNDebug_ColorNormal		= float3(1.0f, 1.0f, 1.0f);
static const float3 ZERNDebug_ColorTangent		= float3(1.0f, 0.0f, 0.0f);
static const float3 ZERNDebug_ColorBinormal		= float3(0.0f, 1.0f, 0.0f);



ZERNDebug_GeometryShader_Input ZERNDebug_VertexShader_Main(ZERNDebug_VertexShader_Input Input)
{
	float3 Normal = DecodeNormal(Input.NormalEncoded);
	float3 Tangent;
	float3 Binormal;
	DecodeTangentBinormal(Input.TangentEncoded, Normal, Tangent, Binormal);
	
	ZERNDebug_GeometryShader_Input Output;
	
	#ifdef ZERN_DEBUG_SKIN_TRANSFORM
		float4x4 SkinTransform = ZERNSkin_GetSkinTransform(Input.BoneIndices, Input.BoneWeights);
		Input.Position = mul(SkinTransform, float4(Input.Position, 1.0f)).xyz;
		Normal = mul(SkinTransform, float4(Normal, 0.0f)).xyz;
		Tangent = mul(SkinTransform, float4(Tangent, 0.0f)).xyz;
		Binormal = mul(SkinTransform, float4(Binormal, 0.0f)).xyz;
	#endif
	
	Output.Position = mul(ZERNDebug_WorldTransform, float4(Input.Position, 1.0f)).xyz;
	Output.Normal = mul(ZERNDebug_WorldTransformInverseTranspose, float4(Normal, 0.0f)).xyz;
	Output.Tangent = mul(ZERNDebug_WorldTransform, float4(Tangent, 0.0f)).xyz;
	Output.Binormal = mul(ZERNDebug_WorldTransform, float4(Binormal, 0.0f)).xyz;
	
	return Output;
}

[maxvertexcount(22)]
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
	
	if (ZERNDebug_ShowWireframe)
	{
		Output.Color = ZERNDebug_ColorWireframe;
		
		Output.Position = ZERNTransformations_WorldToProjection(float4(Input[0].Position, 1.0f));
		OutputStream.Append(Output);
		
		Output.Position = ZERNTransformations_WorldToProjection(float4(Input[1].Position, 1.0f));
		OutputStream.Append(Output);
		
		Output.Position = ZERNTransformations_WorldToProjection(float4(Input[2].Position, 1.0f));
		OutputStream.Append(Output);
		
		Output.Position = ZERNTransformations_WorldToProjection(float4(Input[0].Position, 1.0f));
		OutputStream.Append(Output);
		
		OutputStream.RestartStrip();
	}
	
	if (ZERNDebug_ShowNormalVectors)
	{
		float LineLength = 0.2f;
		
		float3 LineStartWorld = Input[0].Position;
		float3 LineEndWorld = LineStartWorld + normalize(Input[0].Normal) * LineLength;
		
		Output.Color = ZERNDebug_ColorNormal;
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();
		
		LineEndWorld = LineStartWorld + normalize(Input[0].Tangent) * LineLength;
		
		Output.Color = ZERNDebug_ColorTangent;
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();
		
		LineEndWorld = LineStartWorld + normalize(Input[0].Binormal) * LineLength;
		
		Output.Color = ZERNDebug_ColorBinormal;
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();	
		
		
		
		LineStartWorld = Input[1].Position;
		LineEndWorld = LineStartWorld + normalize(Input[1].Normal) * LineLength;
		
		Output.Color = ZERNDebug_ColorNormal;
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();
		
		LineEndWorld = LineStartWorld + normalize(Input[1].Tangent) * LineLength;
		
		Output.Color = ZERNDebug_ColorTangent;
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();
		
		LineEndWorld = LineStartWorld + normalize(Input[1].Binormal) * LineLength;
		
		Output.Color = ZERNDebug_ColorBinormal;
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();	
		
		
		
		LineStartWorld = Input[2].Position;
		LineEndWorld = LineStartWorld + normalize(Input[2].Normal) * LineLength;
		
		Output.Color = ZERNDebug_ColorNormal;
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();
		
		LineEndWorld = LineStartWorld + normalize(Input[2].Tangent) * LineLength;
		
		Output.Color = ZERNDebug_ColorTangent;
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();
		
		LineEndWorld = LineStartWorld + normalize(Input[2].Binormal) * LineLength;
		
		Output.Color = ZERNDebug_ColorBinormal;
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineStartWorld, 1.0f));
		OutputStream.Append(Output);
		Output.Position = ZERNTransformations_WorldToProjection(float4(LineEndWorld, 1.0f));
		OutputStream.Append(Output);
		OutputStream.RestartStrip();
	}
}

ZERNDebug_PixelShader_Input ZERNDebug_Canvas_VertexShader_Main(ZERNDebug_CanvasVertex Input)
{
	ZERNDebug_PixelShader_Input Output;
	Output.Position = ZERNTransformations_WorldToProjection(float4(Input.Position, 1.0f));
	Output.Color = Input.Color.rgb;
	
	return Output;
}

float3 ZERNDebug_PixelShader_Main(ZERNDebug_PixelShader_Input Input) : SV_Target0
{
	return Input.Color;
}

#endif
