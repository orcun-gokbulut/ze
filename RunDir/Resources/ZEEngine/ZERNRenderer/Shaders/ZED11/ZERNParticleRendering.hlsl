//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNParticleRendering.hlsl
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

#ifndef __ZERN_PARTICLE_RENDERING_HLSL__
#define __ZERN_PARTICLE_RENDERING_HLSL__

#include "ZERNView.hlsl"

struct ZERNParticleRendering_Instance_Attributes
{
	float3	Position;
	float	Size;
};

struct ZERNParticleRendering_HullShader_Input
{
	float3	Position	:	POSITION0;
	float2	TexCoord	:	TEXCOORD0;
	float4	Color		:	COLOR0;
};

struct ZERNParticleRendering_HullShader_ConstantData_Output
{
	float Edges[4]		:	SV_TessFactor;
	float Inside[2]		:	SV_InsideTessFactor;
};

struct ZERNParticleRendering_DomainShader_Input
{
	float3	Position	:	POSITION0;
	float2	TexCoord	:	TEXCOORD0;
	float4	Color		:	COLOR0;
};

struct ZERNParticleRendering_PixelShader_Input
{
	float4	Position	:	SV_Position;
	float2	TexCoord	:	TEXCOORD0;
	float4	Color		:	COLOR0;
};

cbuffer ZERNParticleRendering_Constants : register(ZERN_SHADER_CONSTANT_DRAW_TRANSFORM)
{
	float4x4 ZERNParticleRendering_WorldMatrix;
};

StructuredBuffer<ZERNParticleRendering_Instance_Attributes> Instances : register(t0);

ZERNParticleRendering_HullShader_Input ZERNParticleRendering_VertexShader_Main(uint VertexID : SV_VertexID, uint InstanceID : SV_InstanceID)
{
	float3 Center = Instances[InstanceID].Position;
	float2 Corner = Instances[InstanceID].Size * ((uint2(VertexID, VertexID + 1) & uint2(2, 2)) + float2(-1.0f, -1.0f));
	
	ZERNParticleRendering_HullShader_Input Output;
	Output.Position = Center + mul((float3x3)ZERNView_InvViewTransform, float3(Corner, 0.0f));
	Output.TexCoord = float2(0.0f, 0.0f);
	Output.Color = float4(1.0f, 1.0f, 0.0f, 1.0f);

	return Output;
}

ZERNParticleRendering_HullShader_ConstantData_Output ZERNParticleRendering_HullShader_Constant(InputPatch<ZERNParticleRendering_HullShader_Input, 4> Patch, uint PatchID : SV_PrimitiveID)
{
	ZERNParticleRendering_HullShader_ConstantData_Output Output;
	
	Output.Edges[0] = 1.0f;
	Output.Edges[1] = 1.0f;
	Output.Edges[2] = 1.0f;
	Output.Edges[3] = 1.0f;
	
	Output.Inside[0] = 1.0f;
	Output.Inside[1] = 1.0f;
	
	return Output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ZERNParticleRendering_HullShader_Constant")]
ZERNParticleRendering_DomainShader_Input ZERNParticleRendering_HullShader_Main(InputPatch<ZERNParticleRendering_HullShader_Input, 4> Patch, uint I : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID)
{
	ZERNParticleRendering_DomainShader_Input Output;
	
	Output.Position	= Patch[I].Position;
	Output.TexCoord	= Patch[I].TexCoord;
	Output.Color	= Patch[I].Color;
	
	return Output;
}

[domain("quad")]
ZERNParticleRendering_PixelShader_Input ZERNParticleRendering_DomainShader_Main(ZERNParticleRendering_HullShader_ConstantData_Output Input, float2 UV : SV_DomainLocation, const OutputPatch<ZERNParticleRendering_DomainShader_Input, 4> Patch)
{
	ZERNParticleRendering_PixelShader_Input Output;
	
	//Bilinear Interpolation
	float3 VertexPosition1 = lerp(Patch[0].Position, Patch[1].Position, UV.x);
	float3 VertexPosition2 = lerp(Patch[2].Position, Patch[3].Position, UV.x); 
	float3 VertexPosition  = lerp(VertexPosition1, VertexPosition2, UV.y);
	
	float4x4 WorldViewProjectionTransform = mul(ZERNView_ProjectionTransform, ZERNView_ViewTransform);
	WorldViewProjectionTransform = mul(WorldViewProjectionTransform, ZERNParticleRendering_WorldMatrix);
		
	Output.Position = mul(WorldViewProjectionTransform, float4(VertexPosition, 1.0f));
	Output.TexCoord = float2(0.0f, 0.0f);
	Output.Color = float4(1.0f, 1.0f, 0.0f, 1.0f);

	return Output;
}

float4 ZERNParticleRendering_PixelShader_Main(ZERNParticleRendering_PixelShader_Input Input) : SV_Target0
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

#endif
