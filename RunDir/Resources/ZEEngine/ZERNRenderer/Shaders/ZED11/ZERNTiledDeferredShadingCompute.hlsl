//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNTiledDeferredShadingCompute.hlsl
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

#ifndef __ZERN_TILED_DEFERRED_SHADING_COMPUTE_HLSL__
#define __ZERN_TILED_DEFERRED_SHADING_COMPUTE_HLSL__

#include "ZERNShading.hlsl"
#include "ZERNGBuffer.hlsl"
#include "ZERNTransformations.hlsl"

#define TILE_DIMENSION	16
#define TILE_SIZE		(TILE_DIMENSION * TILE_DIMENSION)
#define MAX_LIGHT		255

cbuffer ZERNTiledDeferredShadingCompute_Constants							: register(b8)
{
	ZERNShading_Light	ZERNTiledDeferredShadingCompute_Lights[MAX_LIGHT];
	uint				ZERNTiledDeferredShadingCompute_LightCount;
	float3				ZERNTiledDeferredShadingCompute_Reserved;
};

RWTexture2D<float3>		ZERNTiledDeferredShadingCompute_OutputColorBuffer	: register(u0);

groupshared uint		TileMinDepth;
groupshared uint		TileMaxDepth;
			
groupshared uint		TileLightCount;
groupshared uint		TileLightIndices[MAX_LIGHT];

float3 ZERNTiledDeferredShadingCompute_PointLighting(ZERNShading_Light PointLight, ZERNShading_Surface Surface)
{
	float3 LightVectorView = PointLight.PositionView - Surface.PositionView;
	float LightDistanceView = length(LightVectorView);
	
	PointLight.DirectionView = LightVectorView / LightDistanceView;
	
	float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(PointLight, Surface);
	float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(PointLight, Surface);
	
	float DistanceAttenuation = 1.0f / dot(PointLight.Attenuation, float3(1.0f, LightDistanceView, LightDistanceView * LightDistanceView));
	
	return (ResultDiffuse + ResultSpecular) * PointLight.Intensity * DistanceAttenuation;
}

[numthreads(TILE_DIMENSION, TILE_DIMENSION, 1)]
void ZERNTiledDeferredShadingCompute_ComputeShader_Main(uint3 GroupId          : SV_GroupID,
														uint3 DispatchThreadId : SV_DispatchThreadID,
														uint3 GroupThreadId    : SV_GroupThreadID
														)
{	
	float2 PixelCoord = DispatchThreadId.xy;
	float2 GBufferDimensions = ZERNGBuffer_GetDimensions();
	float DepthHomogeneous = ZERNGBuffer_GetDepth(PixelCoord);
	
	ZERNShading_Surface Surface;
	Surface.PositionView = ZERNTransformations_ViewportToView(PixelCoord + 0.5f, GBufferDimensions, DepthHomogeneous);
	
	float MinDepth = ZERNView_FarZ;
	float MaxDepth = ZERNView_NearZ;
	
	float DepthView = Surface.PositionView.z;
	
	if(DepthView >= ZERNView_NearZ && DepthView <= ZERNView_FarZ)
	{
		MinDepth = min(MinDepth, DepthView);
		MaxDepth = max(MaxDepth, DepthView);
	}
	
	uint GroupIndex = GroupThreadId.y * TILE_DIMENSION + GroupThreadId.x;
	if(GroupIndex == 0)
	{
        TileLightCount = 0;
        TileMinDepth = 0x7F7FFFFF;	// Max float
        TileMaxDepth = 0;
    }
	
	GroupMemoryBarrierWithGroupSync();
	
	if (MaxDepth >= MinDepth)
	{
        InterlockedMin(TileMinDepth, asuint(MinDepth));
        InterlockedMax(TileMaxDepth, asuint(MaxDepth));
    }
	
	GroupMemoryBarrierWithGroupSync();
	
    float TileMinZ = asfloat(TileMinDepth);
    float TileMaxZ = asfloat(TileMaxDepth);
    
	float2 TileScale = GBufferDimensions * rcp(2.0f * TILE_DIMENSION);
	float2 TileBias = TileScale - float2(GroupId.xy);
	
	float4 ProjectionMatrixColumn0 = float4(ZERNView_ProjectionTransform._11 * TileScale.x, 0.0f, TileBias.x, 0.0f);
	float4 ProjectionMatrixColumn1 = float4(0.0f, -ZERNView_ProjectionTransform._22 * TileScale.y, TileBias.y, 0.0f);
	float4 ProjectionMatrixColumn3 = float4(0.0f, 0.0f, 1.0f, 0.0f);
	
	float4 TileFrustumPlanes[6];
	
	TileFrustumPlanes[0] = ProjectionMatrixColumn3 - ProjectionMatrixColumn0;	//Right
	TileFrustumPlanes[1] = ProjectionMatrixColumn3 + ProjectionMatrixColumn0;	//Left
	TileFrustumPlanes[2] = ProjectionMatrixColumn3 - ProjectionMatrixColumn1;	//Top
	TileFrustumPlanes[3] = ProjectionMatrixColumn3 + ProjectionMatrixColumn1;	//Bottom
	TileFrustumPlanes[4] = float4(0.0f, 0.0f, 1.0f, -TileMinZ);					//Near
	TileFrustumPlanes[5] = float4(0.0f, 0.0f, -1.0f, TileMaxZ);					//Far
	
	for(uint I = 0; I < 4; I++)
		TileFrustumPlanes[I] *= rcp(length(TileFrustumPlanes[I].xyz));
	
	for(uint LightIndex = GroupIndex; LightIndex < ZERNTiledDeferredShadingCompute_LightCount; LightIndex += TILE_SIZE)
	{
		bool InsideFrustum = true;
		//float3 LightPositionView = ZERNTransformations_WorldToView(float4(Lights[LightIndex].PositionView, 1.0f));
		
		for(uint I = 0; I < 6; I++)
		{
			ZERNShading_Light Light = ZERNTiledDeferredShadingCompute_Lights[LightIndex];
			if((dot(TileFrustumPlanes[I], float4(Light.PositionView, 1.0f))) < -Light.Range)
			{
				InsideFrustum = false;
				break;
			}	
		}
		
		if(InsideFrustum)
		{
			uint Index;
			InterlockedAdd(TileLightCount, 1, Index);
			TileLightIndices[Index] = LightIndex;
		}
	}
	
	GroupMemoryBarrierWithGroupSync();
    
	float3 ResultColor = 0.0f;
	
	if(all(PixelCoord < GBufferDimensions))
	{	
		uint LightCount = TileLightCount;
	
		if(LightCount > 0)
		{		
			Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(PixelCoord);
			Surface.NormalView = ZERNGBuffer_GetViewNormal(PixelCoord);
			Surface.Specular = ZERNGBuffer_GetSpecularColor(PixelCoord);
			Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(PixelCoord);
			
			for(uint I = 0; I < LightCount; I++)
			{
				ZERNShading_Light CurrentLight = ZERNTiledDeferredShadingCompute_Lights[TileLightIndices[I]];
			
				if(CurrentLight.Type == 1)
				{
					ResultColor += ZERNTiledDeferredShadingCompute_PointLighting(CurrentLight, Surface);
				}
			}
		}
	}
	
	ZERNTiledDeferredShadingCompute_OutputColorBuffer[PixelCoord] = saturate(ResultColor);
}

#endif
