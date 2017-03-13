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
#include "ZERNIntersections.hlsl"

groupshared uint								TileMinDepth;
groupshared uint								TileMaxDepth;
#if defined ZERN_TILED_TRANSPARENT
	groupshared  uint							TileTransparent;
#endif

groupshared uint								TileLightCount;
groupshared uint								TileLightIndices[MAX_TILED_LIGHT];

StructuredBuffer<ZERNInterSections_Sphere>		PointLightsBoundingSpheres					: register(t6);
StructuredBuffer<ZERNInterSections_Sphere>		SpotLightsBoundingSpheres					: register(t7);

RWStructuredBuffer<uint>						OutputTileLightIndices						: register(u0);

[numthreads(TILE_DIMENSION, TILE_DIMENSION, 1)]
void ZERNTiledDeferredShadingCompute_ComputeShader_Main(uint3 GroupId						: SV_GroupID,
														uint3 DispatchThreadId				: SV_DispatchThreadID,
														uint3 GroupThreadId					: SV_GroupThreadID)
{
	uint GroupIndex = GroupThreadId.y * TILE_DIMENSION + GroupThreadId.x;
	float2 PixelCoord = DispatchThreadId.xy;
	float2 GBufferDimensions = ZERNGBuffer_GetDimensions();
	
	ZERNShading_Surface Surfaces[SAMPLE_COUNT];
	#if defined ZERN_TILED_TRANSPARENT
		uint PixelTransparent = 0;
	#endif
	[unroll]
	for (uint I = 0; I < SAMPLE_COUNT; I++)
	{
		Surfaces[I] = ZERNShading_GetSurfaceFromGBuffers(PixelCoord, I);
		#if defined ZERN_TILED_TRANSPARENT
			uint Stencil = ZERNGBuffer_GetStencil(PixelCoord, I);
			PixelTransparent |= Stencil;
		#endif
	}
	
	float MinDepth = ZERNView_FarZ;
	float MaxDepth = ZERNView_NearZ;
	
	[unroll]
	for (uint S = 0; S < SAMPLE_COUNT; S++)
	{
		MinDepth = min(MinDepth, Surfaces[S].PositionView.z);
		MaxDepth = max(MaxDepth, Surfaces[S].PositionView.z);
	}
	
	if (GroupIndex == 0)
	{
		TileLightCount = 0;
		TileMinDepth = 0x7F7FFFFF;	// Max float
		TileMaxDepth = 0;
		
		#if defined ZERN_TILED_TRANSPARENT
			TileTransparent = 0;
		#endif
	}
	
	GroupMemoryBarrierWithGroupSync();
	
	#if defined ZERN_TILED_TRANSPARENT
		InterlockedOr(TileTransparent, PixelTransparent);
	#endif
	
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
	
	float4 ProjectionMatrixColumn0 = float4(ZERNView_ProjectionTransform._11 * TileScale.x, 0.0f, ZERNView_ProjectionTransform._13 * TileScale.x + TileBias.x, 0.0f);
	float4 ProjectionMatrixColumn1 = float4(0.0f, ZERNView_ProjectionTransform._22 * -TileScale.y, ZERNView_ProjectionTransform._23 * -TileScale.y + TileBias.y, 0.0f);
	float4 ProjectionMatrixColumn3 = float4(0.0f, 0.0f, 1.0f, 0.0f);
	
	ZERNInterSections_Plane TileFrustumPlanes[6] = 
	{
		{ProjectionMatrixColumn3.xyz - ProjectionMatrixColumn0.xyz, 0.0f},
		{ProjectionMatrixColumn3.xyz + ProjectionMatrixColumn0.xyz, 0.0f},
		{ProjectionMatrixColumn3.xyz - ProjectionMatrixColumn1.xyz, 0.0f},
		{ProjectionMatrixColumn3.xyz + ProjectionMatrixColumn1.xyz, 0.0f},
		{float3(0.0f, 0.0f, -1.0f), TileMaxZ},
		{float3(0.0f, 0.0f, 1.0f), -TileMinZ}
	};

	[unroll]
	for (uint I = 0; I < 4; I++)
		TileFrustumPlanes[I].Normal *= rcp(length(TileFrustumPlanes[I].Normal.xyz));
	

	for (uint PointLightIndex = GroupIndex; PointLightIndex < ZERNShading_PointLightCount; PointLightIndex += TILE_SIZE)
	{
		bool InsideFrustum = true;
		ZERNInterSections_Sphere BoundingSphere = PointLightsBoundingSpheres[PointLightIndex];
		
		[unroll]
		for (uint I = 0; I < 4; I++)
			InsideFrustum = InsideFrustum && ZERNInterSections_PlaneSphere(TileFrustumPlanes[I], BoundingSphere);

		bool Visible = InsideFrustum && ZERNInterSections_PlaneSphere(TileFrustumPlanes[4], BoundingSphere);
		InsideFrustum = Visible && ZERNInterSections_PlaneSphere(TileFrustumPlanes[5], BoundingSphere);
		
		if (InsideFrustum
		#if defined ZERN_TILED_TRANSPARENT
			|| (TileTransparent != 0 && Visible)
		#endif
		)
		{
			uint Index;
			InterlockedAdd(TileLightCount, 1, Index);
			TileLightIndices[Index] = PointLightIndex;
		}
	}
	
	GroupMemoryBarrierWithGroupSync();
	
	uint TilePointLightCount = TileLightCount;
	
	#if defined ZERN_TILED_TRANSPARENT
		for (uint SpotLightIndex = GroupIndex; SpotLightIndex < ZERNShading_SpotLightCount; SpotLightIndex += TILE_SIZE)
		{
			bool InsideFrustum = true;
			
			ZERNInterSections_Sphere BoundingSphere = SpotLightsBoundingSpheres[SpotLightIndex];
			
			[unroll]
			for (uint I = 0; I < 5; I++)
				InsideFrustum = InsideFrustum && ZERNInterSections_PlaneSphere(TileFrustumPlanes[I], BoundingSphere);
			
			if (InsideFrustum)
			{
				uint Index;
				InterlockedAdd(TileLightCount, 1, Index);
				TileLightIndices[Index] = SpotLightIndex;
			}
		}
		
		GroupMemoryBarrierWithGroupSync();
		
		uint TileTotalLightCount = TileLightCount;
	#endif

	uint TileIndex = GroupId.y * ZERNShading_TileCountX + GroupId.x;
	uint TileStartOffset = (MAX_TILED_LIGHT + 2) * TileIndex;

	for (uint M = GroupIndex; M < TilePointLightCount; M += TILE_SIZE)
		OutputTileLightIndices[TileStartOffset + 2 + M] = TileLightIndices[M];

	#if defined ZERN_TILED_TRANSPARENT
		for (uint N = (GroupIndex + TilePointLightCount); N < TileTotalLightCount; N += TILE_SIZE)
			OutputTileLightIndices[TileStartOffset + 2 + N] = TileLightIndices[N];
	#endif
	
	if (GroupIndex == 0)
	{
		OutputTileLightIndices[TileStartOffset] = TilePointLightCount;
		#if defined ZERN_TILED_TRANSPARENT
			OutputTileLightIndices[TileStartOffset + 1] = TileTotalLightCount;
		#else
			OutputTileLightIndices[TileStartOffset + 1] = TilePointLightCount;
		#endif
	}
}

#endif
