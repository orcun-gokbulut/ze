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

struct ZERNDeferredShading_VertexShader_Input
{
	float3	Position			: POSITION0;
	float2	Texcoord			: TEXCOORD0;
	float3	Normal				: NORMAL0;
	float4	Color				: COLOR0;	
};

struct ZERNDeferredShading_VertexShader_Output
{
	float4	Position			: SV_Position;
	uint	InstanceID			: SV_InstanceID;
};

struct ZERNDeferredShading_PixelShader_Input
{
	float4	PositionViewport	: SV_Position;
	uint	InstanceID			: SV_InstanceID;
};

ZERNShading_Surface ZERNDeferredShading_GetSurfaceDataFromGBuffers(float2 PositionViewport, uint SampleIndex = 0)
{
	ZERNShading_Surface Surface;
	Surface.PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, ZERNGBuffer_GetDimensions(), ZERNGBuffer_GetDepth(PositionViewport.xy, SampleIndex));
	Surface.NormalView = ZERNGBuffer_GetViewNormal(PositionViewport.xy, SampleIndex);
	Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(PositionViewport.xy, SampleIndex);
	Surface.SubsurfaceScattering = ZERNGBuffer_GetSubsurfaceScattering(PositionViewport.xy, SampleIndex);
	Surface.Specular = ZERNGBuffer_GetSpecularColor(PositionViewport.xy, SampleIndex);
	Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(PositionViewport.xy, SampleIndex);
	
	return Surface;
}

float3 ZERNDeferredShading_Shade(ZERNShading_Surface Surface, uint InstanceID)
{
	float3 ResultColor = 0.0f;
	
	#if defined ZERN_RENDER_DIRECTIONAL_LIGHT
		ResultColor = ZERNShading_DirectionalShading(ZERNShading_DirectionalLights[InstanceID], Surface);
	#elif defined ZERN_RENDER_PROJECTIVE_LIGHT
		ResultColor = ZERNShading_ProjectiveShading(ZERNShading_ProjectiveLights[InstanceID], Surface);
	#elif defined ZERN_RENDER_POINT_LIGHT
		ResultColor = ZERNShading_PointShading(ZERNShading_PointLights[InstanceID], Surface);
	#endif
	
	return ResultColor;
}

ZERNDeferredShading_VertexShader_Output ZERNDeferredShading_VertexShader_LightingStage(ZERNDeferredShading_VertexShader_Input Input, uint InstanceID : SV_InstanceID)
{
	ZERNDeferredShading_VertexShader_Output Output;
	
	#if defined ZERN_RENDER_DIRECTIONAL_LIGHT
		Output.Position = float4(Input.Position, 1.0f);
	#elif defined ZERN_RENDER_PROJECTIVE_LIGHT
		float4 PositionWorld = mul(ZERNShading_ProjectiveLights[InstanceID].WorldMatrix, float4(Input.Position, 1.0f));
		Output.Position = ZERNTransformations_WorldToProjection(PositionWorld);
	#elif defined ZERN_RENDER_POINT_LIGHT
		float4 PositionWorld = mul(ZERNShading_PointLights[InstanceID].WorldMatrix, float4(Input.Position, 1.0f));
		Output.Position = ZERNTransformations_WorldToProjection(PositionWorld);
	#endif
	
	Output.InstanceID = InstanceID;
	
	return Output;
};

float3 ZERNDeferredShading_PixelShader_LightingStage(ZERNDeferredShading_PixelShader_Input Input) : SV_Target0
{
	ZERNShading_Surface Surface = ZERNDeferredShading_GetSurfaceDataFromGBuffers(Input.PositionViewport.xy);
	return ZERNDeferredShading_Shade(Surface, Input.InstanceID);
}

float3 ZERNDeferredShading_PixelShader_PerSample_LightingStage(ZERNDeferredShading_PixelShader_Input Input, uint SampleIndex : SV_SampleIndex) : SV_Target0
{
	ZERNShading_Surface Surface = ZERNDeferredShading_GetSurfaceDataFromGBuffers(Input.PositionViewport.xy, SampleIndex);
	return ZERNDeferredShading_Shade(Surface, Input.InstanceID);
}

float3 ZERNDeferredShading_Accumulate_Emissive_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	return ZERNGBuffer_GetEmissiveColor(PositionViewport.xy);
}

void ZERNDeferredShading_EdgeDetection_PixelShader_Main(float4 PositionViewport : SV_Position)
{
	ZERNShading_Surface Surfaces[SAMPLE_COUNT];
	[unroll]
    for (uint I = 0; I < SAMPLE_COUNT; I++)
	{
		Surfaces[I].PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, ZERNGBuffer_GetDimensions(), ZERNGBuffer_GetDepth(PositionViewport.xy, I));
		Surfaces[I].NormalView = ZERNGBuffer_GetViewNormal(PositionViewport.xy, I);
	}
	
	bool EdgePixel = false;
	[unroll]
	for (uint J = 1; J < SAMPLE_COUNT; J++)
	{
		if (abs(Surfaces[J].PositionView.z - Surfaces[0].PositionView.z) > 0.1f ||
			dot(Surfaces[J].NormalView, Surfaces[0].NormalView) < 0.99f)
			EdgePixel = true;	
	}
	
	if (!EdgePixel)
		discard;
}

#endif
