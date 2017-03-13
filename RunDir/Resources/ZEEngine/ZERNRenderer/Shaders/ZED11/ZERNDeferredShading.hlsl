//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNDeferredShading.hlsl
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

#ifndef __ZERN_DEFERRED_SHADING_H__
#define __ZERN_DEFERRED_SHADING_H__

#include "ZERNGBuffer.hlsl"
#include "ZERNScreenCover.hlsl"
#include "ZERNTransformations.hlsl"
#include "ZERNShading.hlsl"

struct ZERNDeferredShading_VertexShader_Output
{
	float4	Position			: SV_Position;
	uint	InstanceID			: SV_InstanceID;
};

struct ZERNDeferredShading_PixelShader_Input
{
	float4	PositionViewport	: SV_Position;
	#ifndef ZERN_TILED_DEFERRED
		uint	InstanceID			: SV_InstanceID;
	#endif
};


cbuffer ZERNDeferredShading_InstancedLights_Transform_Constants		: register(b11)
{
	float4x4	ZERNDeferredShading_InstancedLights_Transforms[MAX_DEFERRED_LIGHT];
};

float3 ZERNDeferredShading_Shade(ZERNShading_Surface Surface, uint InstanceID)
{
	float3 ResultColor = 0.0f;
	
	#if defined ZERN_RENDER_SPOT_LIGHT
		ResultColor = ZERNShading_SpotShading(ZERNShading_SpotLights[InstanceID], Surface);
	#elif defined ZERN_RENDER_POINT_LIGHT
		ResultColor = ZERNShading_PointShading(ZERNShading_PointLights[InstanceID], Surface);
	#elif defined ZERN_RENDER_PROJECTIVE_LIGHT
		ResultColor = ZERNShading_ProjectiveShading(ZERNShading_ProjectiveLightConstant, Surface);
	#endif
	
	return ResultColor;
}

ZERNDeferredShading_VertexShader_Output ZERNDeferredShading_VertexShader_LightingStage(float3 Position : POSITION0, uint InstanceID : SV_InstanceID)
{
	ZERNDeferredShading_VertexShader_Output Output;

	float4 PositionWorld = mul(ZERNDeferredShading_InstancedLights_Transforms[InstanceID], float4(Position, 1.0f));

	Output.Position = ZERNTransformations_WorldToProjection(PositionWorld);
	Output.InstanceID = InstanceID;

	return Output;
};

float3 ZERNDeferredShading_PixelShader_LightingStage(ZERNDeferredShading_PixelShader_Input Input) : SV_Target0
{
	ZERNShading_Surface Surface = ZERNShading_GetSurfaceFromGBuffers(Input.PositionViewport.xy);
	
	#if defined ZERN_TILED_DEFERRED
		float3 ResultColor = ZERNShading_Shade(Input.PositionViewport.xy, Surface);
	#else
		float3 ResultColor = ZERNDeferredShading_Shade(Surface, Input.InstanceID);
	#endif
	
	return ResultColor;
}

float3 ZERNDeferredShading_PixelShader_PerSample_LightingStage(ZERNDeferredShading_PixelShader_Input Input, uint SampleIndex : SV_SampleIndex) : SV_Target0
{
	ZERNShading_Surface Surface = ZERNShading_GetSurfaceFromGBuffers(Input.PositionViewport.xy, SampleIndex);

	#if defined ZERN_TILED_DEFERRED
		float3 ResultColor = ZERNShading_Shade(Input.PositionViewport.xy, Surface);
	#else
		float3 ResultColor = ZERNDeferredShading_Shade(Surface, Input.InstanceID);
	#endif
	
	return ResultColor;
}

void ZERNDeferredShading_EdgeDetection_PixelShader_Main(float4 PositionViewport : SV_Position)
{
	ZERNShading_Surface Surfaces[SAMPLE_COUNT];
	
	[unroll]
	for (uint I = 0; I < SAMPLE_COUNT; I++)
		Surfaces[I] = ZERNShading_GetSurfaceFromGBuffers(PositionViewport.xy, I);

	if (!ZERNShading_EdgeDetection_MSAA(Surfaces))
		discard;
}

float3 ZERNDeferredShading_DebugEdgeDetection_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	return float3(1.0f, 0.0f, 0.0f);
}

#endif
