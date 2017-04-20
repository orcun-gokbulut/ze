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

#if defined ZERN_TILED_PARTICLE
	#define ZERN_TILED_THREAD_WORK_DIM			uint2(2, 1)
	#define ZERN_TILED_THREAD_DIM				uint2(128, 1)
	#define ZERN_TILED_TILE_DIM					TILE_PARTICLE_DIM
#else
	#define ZERN_TILED_THREAD_WORK_DIM			uint2(4, 1)
	#define ZERN_TILED_THREAD_DIM				uint2(256, 1)
	#define ZERN_TILED_TILE_DIM					TILE_LIGHT_DIM
#endif

#define ZERN_TILED_THREAD_WORK_SIZE				(ZERN_TILED_THREAD_WORK_DIM.x * ZERN_TILED_THREAD_WORK_DIM.y)
#define ZERN_TILED_THREAD_SIZE					(ZERN_TILED_THREAD_DIM.x * ZERN_TILED_THREAD_DIM.y)
#define ZERN_TILED_WARP_DIM						uint2(8, 4)
#define ZERN_TILED_WARP_SIZE					(ZERN_TILED_WARP_DIM.x * ZERN_TILED_WARP_DIM.y)
#define ZERN_TILED_WARP_WORK_DIM				(ZERN_TILED_WARP_DIM * ZERN_TILED_THREAD_WORK_DIM)
#define ZERN_TILED_WARP_WORK_SIZE				(ZERN_TILED_WARP_WORK_DIM.x * ZERN_TILED_WARP_WORK_DIM.y)
#define ZERN_TILED_WARP_COUNT_X					(ZERN_TILED_TILE_DIM / ZERN_TILED_WARP_WORK_DIM.x)

#define SURFACE_COUNT							(SAMPLE_COUNT * ZERN_TILED_THREAD_WORK_SIZE)

groupshared uint								TileMinDepth;
groupshared uint								TileMaxDepth;

groupshared uint								TileLightCount;

#if defined ZERN_TILED_TRANSPARENT
	groupshared uint							TileLightCountTransparent;
#endif
groupshared uint								TileLightIndices[MAX_TILED_LIGHT];

StructuredBuffer<ZERNInterSections_Sphere>		PointLightsBoundingSpheres					: register(t6);
StructuredBuffer<ZERNInterSections_Sphere>		SpotLightsBoundingSpheres					: register(t7);

RWStructuredBuffer<uint>						OutputTileLightIndices						: register(u0);

#if defined ZERN_TILED_PARTICLE
	groupshared uint							TileParticleCount;
	RWStructuredBuffer<uint>					OutputTileParticleIndices					: register(u1);
#endif

[numthreads(ZERN_TILED_THREAD_DIM.x, ZERN_TILED_THREAD_DIM.y, 1)]
void ZERNTiledDeferredShadingCompute_ComputeShader_Main(uint3 GroupId						: SV_GroupID,
														uint3 DispatchThreadId				: SV_DispatchThreadID,
														uint3 GroupThreadId					: SV_GroupThreadID)
{
	uint GroupThreadIndex = GroupThreadId.y * ZERN_TILED_THREAD_DIM.x + GroupThreadId.x;
	uint WarpThreadIndex = GroupThreadIndex % ZERN_TILED_WARP_SIZE;
	uint WarpIndex = GroupThreadIndex / ZERN_TILED_WARP_SIZE;
	
	uint2 WarpPixelCoord = GroupId.xy * ZERN_TILED_TILE_DIM + uint2(WarpIndex % ZERN_TILED_WARP_COUNT_X, WarpIndex / ZERN_TILED_WARP_COUNT_X) * ZERN_TILED_WARP_WORK_DIM;

	float2 GBufferDimensions = ZERNGBuffer_GetDimensions();
	
	ZERNShading_Surface Surfaces[SURFACE_COUNT];
	
	[unroll]
	for (uint W = 0; W < ZERN_TILED_THREAD_WORK_SIZE; W++)
	{
		uint2 WorkId = uint2(W % ZERN_TILED_THREAD_WORK_DIM.x, W / ZERN_TILED_THREAD_WORK_DIM.x);
		float2 PixelCoord = WarpPixelCoord + WorkId * ZERN_TILED_WARP_DIM;
		
		[unroll]
		for (uint S = 0; S < SAMPLE_COUNT; S++)
			Surfaces[W * SAMPLE_COUNT + S] = ZERNShading_GetSurfaceFromGBuffers(PixelCoord + float2((WarpThreadIndex / SAMPLE_COUNT) + ((S % 2) * 4), S / 2), (WarpThreadIndex % SAMPLE_COUNT));
	}

	float MinDepth = ZERNView_FarZ;
	float MaxDepth = ZERNView_NearZ;
	
	[unroll]
	for (uint S = 0; S < SURFACE_COUNT; S++)
	{
		MinDepth = min(MinDepth, Surfaces[S].PositionView.z);
		MaxDepth = max(MaxDepth, Surfaces[S].PositionView.z);
	}
	
	if (GroupThreadIndex == 0)
	{
		TileMinDepth = 0x7F7FFFFF;	// Max float
		TileMaxDepth = 0;
		TileLightCount = 0;
		#if defined ZERN_TILED_TRANSPARENT
			TileLightCountTransparent = 0;
		#endif
		
		#if defined ZERN_TILED_PARTICLE
			TileParticleCount = 0;
		#endif
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
	
	float2 TileScale = GBufferDimensions * rcp(2.0f * ZERN_TILED_TILE_DIM);
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
		TileFrustumPlanes[I].Normal *= rcp(length(TileFrustumPlanes[I].Normal));

	#if defined ZERN_TILED_PARTICLE
		uint TileIndex = GroupId.y * (GBufferDimensions.x / ZERN_TILED_TILE_DIM) + GroupId.x;
		uint TileStartOffset = TileIndex * TILE_PARTICLE_TOTAL_COUNT;
		
		for (uint EmitterIndex = 0; EmitterIndex < 1; EmitterIndex++)
		{
			uint StartOffset = ZERNShading_Emitters[EmitterIndex].StartOffset;
			uint ParticleCount = ZERNShading_Emitters[EmitterIndex].ParticleCount;
			for (uint ParticleIndex = StartOffset + GroupThreadIndex; ParticleIndex < (StartOffset + ParticleCount)/*ZERNShading_ParticleCount*/; ParticleIndex += ZERN_TILED_THREAD_SIZE)
			{
				bool InsideFrustum = true;
				ZERNInterSections_Sphere BoundingSphere = {ZERNShading_ParticlePool[ParticleIndex].xyz, ZERNShading_ParticlePool[ParticleIndex].w};
				
				[unroll]
				for (uint I = 0; I < 5; I++)
					InsideFrustum = InsideFrustum && ZERNInterSections_PlaneSphere(TileFrustumPlanes[I], BoundingSphere);
				
				if (InsideFrustum)
				{
					uint Index;
					InterlockedAdd(TileParticleCount, 1, Index);
					OutputTileParticleIndices[TileStartOffset + TILE_PARTICLE_HEADER_COUNT + Index] = (EmitterIndex << 20) | ParticleIndex;
				}
			}
		}
		GroupMemoryBarrierWithGroupSync();
		
		if (GroupThreadIndex == 0)
			OutputTileParticleIndices[TileStartOffset] = TileParticleCount;
	#else
	
	for (uint PointLightIndex = GroupThreadIndex; PointLightIndex < ZERNShading_PointLightCount; PointLightIndex += ZERN_TILED_THREAD_SIZE)
	{
		bool InsideFrustum = true;
		ZERNInterSections_Sphere BoundingSphere = PointLightsBoundingSpheres[PointLightIndex];
		
		[unroll]
		for (uint I = 0; I < 5; I++)
			InsideFrustum = InsideFrustum && ZERNInterSections_PlaneSphere(TileFrustumPlanes[I], BoundingSphere);
		
		bool Visible = InsideFrustum;
		
		InsideFrustum = InsideFrustum && ZERNInterSections_PlaneSphere(TileFrustumPlanes[5], BoundingSphere);
		
		if (InsideFrustum)
		{
			uint Index;
			InterlockedAdd(TileLightCount, 1, Index);
			TileLightIndices[Index] = PointLightIndex;
		}
	#if defined ZERN_TILED_TRANSPARENT
		else if (Visible)
		{
			uint Index;
			InterlockedAdd(TileLightCountTransparent, 1, Index);
			TileLightIndices[(MAX_TILED_LIGHT - 1) - Index] = PointLightIndex;
		}
	#endif
	}
	GroupMemoryBarrierWithGroupSync();
	
	uint TilePointLightCount = TileLightCount;
	
	#if defined ZERN_TILED_TRANSPARENT
		uint TilePointLightCountTransparent = TileLightCountTransparent;
		
		for (uint SpotLightIndex = GroupThreadIndex; SpotLightIndex < ZERNShading_SpotLightCount; SpotLightIndex += ZERN_TILED_THREAD_SIZE)
		{
			bool InsideFrustum = true;
			ZERNInterSections_Sphere BoundingSphere = SpotLightsBoundingSpheres[SpotLightIndex];
			
			[unroll]
			for (uint I = 0; I < 5; I++)
				InsideFrustum = InsideFrustum && ZERNInterSections_PlaneSphere(TileFrustumPlanes[I], BoundingSphere);
			
			if (InsideFrustum)
			{
				uint Index;
				InterlockedAdd(TileLightCountTransparent, 1, Index);
				TileLightIndices[(MAX_TILED_LIGHT - 1) - Index] = SpotLightIndex;
			}
		}
		
		GroupMemoryBarrierWithGroupSync();
		
		uint TileTotalLightCountTransparent = TileLightCountTransparent;
	#endif

	uint TileIndex = GroupId.y * ZERNShading_TileCountX + GroupId.x;
	uint TileStartOffset = TileIndex * TILE_LIGHT_TOTAL_COUNT;

	for (uint M = GroupThreadIndex; M < TilePointLightCount; M += ZERN_TILED_THREAD_SIZE)
		OutputTileLightIndices[TileStartOffset + TILE_LIGHT_HEADER_COUNT + M] = TileLightIndices[M];

	#if defined ZERN_TILED_TRANSPARENT
		for (int N = ((MAX_TILED_LIGHT - 1) - GroupThreadIndex); N >= int(MAX_TILED_LIGHT - TileTotalLightCountTransparent); N -= ZERN_TILED_THREAD_SIZE)
			OutputTileLightIndices[TileStartOffset + TILE_LIGHT_HEADER_COUNT + TilePointLightCount + ((MAX_TILED_LIGHT - 1) - N)] = TileLightIndices[N];
	#endif
	
	if (GroupThreadIndex == 0)
	{
		OutputTileLightIndices[TileStartOffset] = TilePointLightCount;
		#if defined ZERN_TILED_TRANSPARENT
			OutputTileLightIndices[TileStartOffset + 1] = TilePointLightCountTransparent;
			OutputTileLightIndices[TileStartOffset + 2] = TileTotalLightCountTransparent - TilePointLightCountTransparent;
		#endif
	}
	#endif
}

#endif
