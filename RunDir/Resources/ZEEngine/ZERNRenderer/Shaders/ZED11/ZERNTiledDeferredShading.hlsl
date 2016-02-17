//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNTiledDeferredShading.hlsl
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

#ifndef __ZERN_TILED_DEFERRED_SHADING_H__
#define __ZERN_TILED_DEFERRED_SHADING_H__

#include "ZERNGBuffer.hlsl"
#include "ZERNScreenCover.hlsl"
#include "ZERNTransformations.hlsl"
#include "ZERNShading.hlsl"

#define MAX_LIGHTS				255
#define TILE_SIZE_IN_PIXELS		16
	
// INPUT OUTPUTS
///////////////////////////////////////////////////////////////////////////////

struct ZERNTiledDeferredShading_TileInfo
{
	uint											LightIndices[MAX_LIGHTS];
	uint											LightCount;
};

StructuredBuffer<ZERNShading_Light>					ZERNTiledDeferredShading_Lights		:	register(t5);
StructuredBuffer<ZERNTiledDeferredShading_TileInfo>	ZERNTiledDeferredShading_TileInfos	:	register(t6);

float3 ZERNTiledDeferredShading_PointLighting(ZERNShading_Light PointLight, ZERNShading_Surface Surface)
{
	float3 LightVectorView = PointLight.PositionView - Surface.PositionView;
	float LightDistanceView = length(LightVectorView);
	
	PointLight.DirectionView = LightVectorView / LightDistanceView;
	
	float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(PointLight, Surface);
	float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(PointLight, Surface);
	
	float DistanceAttenuation = 1.0f / dot(PointLight.Attenuation, float3(1.0f, LightDistanceView, LightDistanceView * LightDistanceView));
	
	return (ResultDiffuse + ResultSpecular) * PointLight.Intensity * DistanceAttenuation;
}

float3 ZERNTiledDeferredShading_Lighting(uint TileIndex, ZERNShading_Surface Surface)
{
	float3 ResultColor = {0.0f, 0.0f, 0.0f};
	uint LightCount = ZERNTiledDeferredShading_TileInfos[TileIndex].LightCount;
	
	if(LightCount > 0)
	{
		for(uint I = 0; I < LightCount; I++)
		{
			uint LightId = ZERNTiledDeferredShading_TileInfos[TileIndex].LightIndices[I];
			ResultColor += ZERNTiledDeferredShading_PointLighting(ZERNTiledDeferredShading_Lights[LightId], Surface);
		}
	}
	
	return saturate(ResultColor);
}

// LIGHTING STAGE - PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float3 ZERNTiledDeferredShading_PixelShader_LightingStage(float4 PositionViewport : SV_Position) : SV_Target0
{	
	float2 GBufferDimensions = ZERNGBuffer_GetDimensions();
	float DepthHomogeneous = ZERNGBuffer_GetDepth(PositionViewport.xy);
	
	ZERNShading_Surface Surface;
	Surface.PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, GBufferDimensions, DepthHomogeneous);
	Surface.NormalView = ZERNGBuffer_GetViewNormal(PositionViewport.xy);
	Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(PositionViewport.xy);
	Surface.Specular = ZERNGBuffer_GetSpecularColor(PositionViewport.xy);
	Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(PositionViewport.xy);
	
	uint2 TileId = PositionViewport.xy / TILE_SIZE_IN_PIXELS;
	uint TileCountX = (GBufferDimensions.x + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
	uint TileIndex = TileId.y * TileCountX + TileId.x;
	
	return ZERNTiledDeferredShading_Lighting(TileIndex, Surface);
}

#endif
