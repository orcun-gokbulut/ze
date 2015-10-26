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

#ifndef __ZERN_DEFERRED_SHADING_H__
#define __ZERN_DEFERRED_SHADING_H__

#include "ZERNGBuffer.hlsl"
#include "ZERNScreenCover.hlsl"

#define MAX_LIGHTS				511
#define TILE_SIZE_IN_PIXELS		16

#define ZE_LT_POINT				1
#define ZE_LT_DIRECTIONAL		2
	
// INPUT OUTPUTS
///////////////////////////////////////////////////////////////////////////////

struct ZERNTiledDeferredShading_Light
{
	float3	PositionWorld;
	float	Range;
	float3	Color;
	float	Intensity;
	float3  Attenuation;
	float	Fov;
	float3	Direction;
	int		Type;
};

struct ZERNTiledDeferredShading_TileInfo
{
	uint LightIndices[MAX_LIGHTS];
	uint LightCount;
};

struct ZERNTiledDeferredShading_Surface
{
	float3	PositionView;
	float	Reserved;
	float3	NormalView;
	float	Reserved1;
	float3	Diffuse;
	float	Reserved2;
	float3	Specular;
	float	SpecularPower;
};

StructuredBuffer<ZERNTiledDeferredShading_Light>	Lights		:	register(t5);
StructuredBuffer<ZERNTiledDeferredShading_TileInfo>	TileInfos	:	register(t6);

SamplerState ZERNTiledDeferredShading_ProjectionSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float3 ZERNTiledDeferredShading_PointLighting(ZERNTiledDeferredShading_Light PointLight, ZERNTiledDeferredShading_Surface Surface)
{
	float3 LightPositionView = mul(ZERNView_ViewTransform, float4(PointLight.PositionWorld, 1.0f)).xyz;
	float3 LightDir = LightPositionView - Surface.PositionView;
	float LightDist = length(LightDir);
	LightDir = LightDir / LightDist;
	float NdotL = max(0, dot(Surface.NormalView, LightDir));
	
	float3 ViewDir = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDir + LightDir);
	float NdotH = max(0, dot(Surface.NormalView, HalfVector));
	float DistanceAttenuation = 1.0f / dot(PointLight.Attenuation, float3(1.0f, LightDist, LightDist * LightDist));
	
	float3 ResultSpecular = pow(NdotH, Surface.SpecularPower) * Surface.Specular;
	float3 ResultDiffuse = NdotL * PointLight.Color * Surface.Diffuse;
	
	return (ResultDiffuse + ResultSpecular) * PointLight.Intensity * DistanceAttenuation;
}

float3 ZERNTiledDeferredShading_DirectionalLighting(ZERNTiledDeferredShading_Light DirectionalLight, ZERNTiledDeferredShading_Surface Surface)
{
	float3 DirectionView = mul(ZERNView_ViewTransform, float4(DirectionalLight.Direction, 0.0f)).xyz;
	DirectionView = normalize(DirectionView);
	float NdotL = max(0, dot(Surface.NormalView, DirectionView));
	
	float3 ViewDir = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDir + DirectionView);
	float NdotH = max(0, dot(Surface.NormalView, HalfVector));
	
	float3 ResultSpecular = pow(NdotH, Surface.SpecularPower) * Surface.Specular;
	float3 ResultDiffuse = Surface.Diffuse * DirectionalLight.Color * NdotL;
	
	return (ResultDiffuse + ResultSpecular) * DirectionalLight.Intensity;
}

float4 ZERNTiledDeferredShading_Lighting(uint TileId, ZERNTiledDeferredShading_Surface Surface)
{
	uint LightCount = TileInfos[TileId].LightCount;
	
	if(LightCount == 0)
		return float4(Surface.Diffuse, 1.0f);
	
	float3 ResultColor = {0.0f, 0.0f, 0.0f};
	
	for(uint I = 0; I < LightCount; ++I)
	{
		uint LightId = TileInfos[TileId].LightIndices[I];

		ZERNTiledDeferredShading_Light CurrentLight = Lights[LightId];
		
		if(CurrentLight.Type == ZE_LT_POINT)
		{
			ResultColor += ZERNTiledDeferredShading_PointLighting(CurrentLight, Surface);
		}
		else if(CurrentLight.Type == ZE_LT_DIRECTIONAL)
		{
			ResultColor += ZERNTiledDeferredShading_DirectionalLighting(CurrentLight, Surface);
		}
	}
	
	return float4(saturate(ResultColor), 1.0f);
}

float3 ZERNTiledDeferredShading_GetPositionViewFromDepth(float2 PositionScreen, float DepthView)
{
	float2 ScreenCoord = PositionScreen / float2(ZERNView_ProjectionTransform._11, ZERNView_ProjectionTransform._22);
	ScreenCoord = ScreenCoord * DepthView;
	
	return float3(ScreenCoord, DepthView);
}

// LIGHTING STAGE - PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float3 ZERNTiledDeferredShading_PixelShader_LightingStage(float4 PositionViewport : SV_Position) : SV_Target0
{	
	float Depth = ZERNGBuffer_GetDepth(uint2(PositionViewport.xy));
	float DepthView = ZERNView_ProjectionTransform._34 / (Depth - ZERNView_ProjectionTransform._33);
	
	float2 GBufferDimension;
	ZERNGBuffer_DepthBuffer.GetDimensions(GBufferDimension.x, GBufferDimension.y);
	
	float2 PositionScreen = (PositionViewport.xy - 0.5f) * (float2(2.0f, -2.0f) / GBufferDimension) + float2(-1.0f, 1.0f);
	
	ZERNTiledDeferredShading_Surface Surface;
	Surface.PositionView = ZERNTiledDeferredShading_GetPositionViewFromDepth(PositionScreen, DepthView);
	Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(int2(PositionViewport.xy));
	Surface.Specular = ZERNGBuffer_GetSpecularColor(int2(PositionViewport.xy));
	Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(int2(PositionViewport.xy));
	Surface.SpecularPower *= 64.0f;
	
	if(Surface.Diffuse.x != 0.0f)
	{
		uint TileX = PositionViewport.x / TILE_SIZE_IN_PIXELS;
		uint TileY = PositionViewport.y / TILE_SIZE_IN_PIXELS;
		uint TileCountX = (GBufferDimension.x + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
		uint TileId = TileY * TileCountX + TileX;
		
		Surface.NormalView = mul(ZERNView_ViewTransform, float4(0.0f, 1.0f, 0.0f, 0.0f)).xyz;
		Surface.NormalView = normalize(Surface.NormalView);
		return ZERNTiledDeferredShading_Lighting(TileId, Surface).xyz;
	}
	
	return float4(Surface.Diffuse, 1.0f).xyz;
}

#endif
