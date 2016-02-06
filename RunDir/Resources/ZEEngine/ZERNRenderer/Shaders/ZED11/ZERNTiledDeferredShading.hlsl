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

#define MAX_LIGHTS				511
#define TILE_SIZE_IN_PIXELS		32

#define ZE_LT_POINT				1
#define ZE_LT_DIRECTIONAL		2
	
// INPUT OUTPUTS
///////////////////////////////////////////////////////////////////////////////

struct ZERNTiledDeferredShading_Light
{
	float3											PositionWorld;
	float											Range;
	float3											Color;
	float											Intensity;
	float3  										Attenuation;
	float											Fov;
	float3											Direction;
	int												Type;
};

struct ZERNTiledDeferredShading_TileInfo
{
	uint											LightIndices[MAX_LIGHTS];
	uint											LightCount;
};

struct ZERNTiledDeferredShading_Surface
{
	float3											PositionView;
	float											Reserved;
	float3											NormalView;
	float											Reserved1;
	float3											Diffuse;
	float											Reserved2;
	float3											Specular;
	float											SpecularPower;
};

StructuredBuffer<ZERNTiledDeferredShading_Light>	ZERNTiledDeferredShading_Lights		:	register(t5);
StructuredBuffer<ZERNTiledDeferredShading_TileInfo>	ZERNTiledDeferredShading_TileInfos	:	register(t6);

float3 ZERNTiledDeferredShading_PointLighting(ZERNTiledDeferredShading_Light PointLight, ZERNTiledDeferredShading_Surface Surface)
{
	float3 LightPositionView = ZERNTransformations_WorldToView(float4(PointLight.PositionWorld, 1.0f));
	float3 LightDir = LightPositionView - Surface.PositionView;
	float LightDist = length(LightDir);
	LightDir = LightDir / LightDist;
	float NdotL = max(0.0f, dot(Surface.NormalView, LightDir));
	
	float3 ViewDir = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDir + LightDir);
	float NdotH = max(0.0f, dot(Surface.NormalView, HalfVector));
	float DistanceAttenuation = 1.0f / dot(PointLight.Attenuation, float3(1.0f, LightDist, LightDist * LightDist));
	
	float3 ResultSpecular = pow(NdotH, Surface.SpecularPower) * Surface.Specular;
	float3 ResultDiffuse = NdotL * PointLight.Color * Surface.Diffuse;
	
	return (ResultDiffuse + ResultSpecular) * PointLight.Intensity * DistanceAttenuation;
}

float3 ZERNTiledDeferredShading_DirectionalLighting(ZERNTiledDeferredShading_Light DirectionalLight, ZERNTiledDeferredShading_Surface Surface)
{
	float3 DirectionView = ZERNTransformations_WorldToView(float4(DirectionalLight.Direction, 0.0f));
	DirectionView = normalize(DirectionView);
	float NdotL = max(0.0f, dot(Surface.NormalView, DirectionView));
	
	float3 ViewDir = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDir + DirectionView);
	float NdotH = max(0.0f, dot(Surface.NormalView, HalfVector));
	
	float3 ResultSpecular = pow(NdotH, Surface.SpecularPower) * Surface.Specular;
	float3 ResultDiffuse = Surface.Diffuse * DirectionalLight.Color * NdotL;
	
	return (ResultDiffuse + ResultSpecular) * DirectionalLight.Intensity;
}

float3 ZERNTiledDeferredShading_Lighting(uint TileIndex, ZERNTiledDeferredShading_Surface Surface)
{
	uint LightCount = ZERNTiledDeferredShading_TileInfos[TileIndex].LightCount;
	float3 ResultColor = {0.0f, 0.0f, 0.0f};
	
	if(LightCount == 0)
	{
		ResultColor = Surface.Diffuse;
	}
	else
	{
		for(uint I = 0; I < LightCount; I++)
		{
			uint LightId = ZERNTiledDeferredShading_TileInfos[TileIndex].LightIndices[I];

			ZERNTiledDeferredShading_Light CurrentLight = ZERNTiledDeferredShading_Lights[LightId];
			
			if(CurrentLight.Type == ZE_LT_POINT)
			{
				ResultColor += ZERNTiledDeferredShading_PointLighting(CurrentLight, Surface);
			}
			else if(CurrentLight.Type == ZE_LT_DIRECTIONAL)
			{
				ResultColor += ZERNTiledDeferredShading_DirectionalLighting(CurrentLight, Surface);
			}
		}
	}
	
	return saturate(ResultColor);
}

// LIGHTING STAGE - PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float3 ZERNTiledDeferredShading_PixelShader_LightingStage(float4 PositionViewport : SV_Position) : SV_Target0
{	
	ZERNTiledDeferredShading_Surface Surface;
	
	float2 Dimensions = ZERNGBuffer_GetDimensions();
	float DepthClip = ZERNGBuffer_GetDepth(PositionViewport.xy);
	
	Surface.PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, Dimensions, DepthClip);
	Surface.NormalView = ZERNGBuffer_GetViewNormal(int2(PositionViewport.xy));
	Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(int2(PositionViewport.xy));
	Surface.Specular = ZERNGBuffer_GetSpecularColor(int2(PositionViewport.xy));
	Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(int2(PositionViewport.xy));
	
	uint2 TileId = PositionViewport.xy / TILE_SIZE_IN_PIXELS;
	uint TileCountX = (Dimensions.x + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
	uint TileIndex = TileId.y * TileCountX + TileId.x;
	
	return ZERNTiledDeferredShading_Lighting(TileIndex, Surface);
}

#endif
