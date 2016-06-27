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

groupshared uint					TileMinDepth;
groupshared uint					TileMaxDepth;

groupshared uint					TileLightCount;
groupshared uint					TileLightIndices[MAX_LIGHT];

#ifdef MSAA_ENABLED
	groupshared	uint				PerSamplePixels[TILE_SIZE];
	groupshared	uint				PerSamplePixelCount;
#endif

#ifdef ZERN_TILED_DEFERRED
	RWTexture2D<float3>				ZERNTiledDeferredShadingCompute_OutputColorBuffer		: register(u0);
#elif defined ZERN_TILED_FORWARD
	RWStructuredBuffer<uint>		ZERNTiledDeferredShadingCompute_OutputTileLightIndices	: register(u0);
#endif

[numthreads(TILE_DIMENSION, TILE_DIMENSION, 1)]
void ZERNTiledDeferredShadingCompute_ComputeShader_Main(uint3 GroupId						: SV_GroupID,
														uint3 DispatchThreadId				: SV_DispatchThreadID,
														uint3 GroupThreadId					: SV_GroupThreadID)
{
	float2 PixelCoord = DispatchThreadId.xy;
	float2 GBufferDimensions = ZERNGBuffer_GetDimensions();
	
	ZERNShading_Surface Surfaces[SAMPLE_COUNT];
	
	float MinDepth = ZERNView_FarZ;
	float MaxDepth = ZERNView_NearZ;
	
	[unroll]
	for (uint S = 0; S < SAMPLE_COUNT; S++)
	{
		Surfaces[S].PositionView.z = ZERNTransformations_HomogeneousToViewDepth(ZERNGBuffer_GetDepth(PixelCoord, S));
		
		MinDepth = min(MinDepth, Surfaces[S].PositionView.z);
		MaxDepth = max(MaxDepth, Surfaces[S].PositionView.z);
	}
	
	uint GroupIndex = GroupThreadId.y * TILE_DIMENSION + GroupThreadId.x;
	if (GroupIndex == 0)
	{
		TileLightCount = 0;
		TileMinDepth = 0x7F7FFFFF;	// Max float
		TileMaxDepth = 0;
		
		#ifdef MSAA_ENABLED
			PerSamplePixelCount = 0;
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
	
	float2 TileScale = GBufferDimensions * rcp(2.0f * TILE_DIMENSION);
	float2 TileBias = TileScale - float2(GroupId.xy);
	
	float4 ProjectionMatrixColumn0 = float4(ZERNView_ProjectionTransform._11 * TileScale.x, 0.0f, ZERNView_ProjectionTransform._13 * TileScale.x + TileBias.x, 0.0f);
	float4 ProjectionMatrixColumn1 = float4(0.0f, ZERNView_ProjectionTransform._22 * -TileScale.y, ZERNView_ProjectionTransform._23 * -TileScale.y + TileBias.y, 0.0f);
	float4 ProjectionMatrixColumn3 = float4(0.0f, 0.0f, 1.0f, 0.0f);
	
	float4 TileFrustumPlanes[6];
	
	TileFrustumPlanes[0] = ProjectionMatrixColumn3 - ProjectionMatrixColumn0;	//Right
	TileFrustumPlanes[1] = ProjectionMatrixColumn3 + ProjectionMatrixColumn0;	//Left
	TileFrustumPlanes[2] = ProjectionMatrixColumn3 - ProjectionMatrixColumn1;	//Top
	TileFrustumPlanes[3] = ProjectionMatrixColumn3 + ProjectionMatrixColumn1;	//Bottom
	TileFrustumPlanes[4] = float4(0.0f, 0.0f, 1.0f, -TileMinZ);					//Near
	TileFrustumPlanes[5] = float4(0.0f, 0.0f, -1.0f, TileMaxZ);					//Far
	
	for (uint I = 0; I < 4; I++)
		TileFrustumPlanes[I] *= rcp(length(TileFrustumPlanes[I].xyz));
	
	for (uint PointLightIndex = GroupIndex; PointLightIndex < ZERNShading_PointLightCount; PointLightIndex += TILE_SIZE)
	{
		bool InsideFrustum = true;
		ZERNShading_PointLight PointLight = ZERNShading_PointLights[PointLightIndex];
		
		for (uint I = 0; I < 6; I++)
			InsideFrustum = InsideFrustum && (dot(TileFrustumPlanes[I], float4(PointLight.PositionView, 1.0f)) >= -PointLight.Range);
		
		if (InsideFrustum)
		{
			uint Index;
			InterlockedAdd(TileLightCount, 1, Index);
			TileLightIndices[Index] = PointLightIndex;
		}
	}
	
	GroupMemoryBarrierWithGroupSync();
	
	#ifdef ZERN_TILED_FORWARD
		uint TilePointLightCount = TileLightCount;
	
		for (uint ProjectiveLightIndex = GroupIndex; ProjectiveLightIndex < ZERNShading_ProjectiveLightCount; ProjectiveLightIndex += TILE_SIZE)
		{
			bool InsideFrustum = true;
			ZERNShading_ProjectiveLight ProjectiveLight = ZERNShading_ProjectiveLights[ProjectiveLightIndex];
			
			for (uint I = 0; I < 6; I++)
				InsideFrustum = InsideFrustum && (dot(TileFrustumPlanes[I], float4(ProjectiveLight.CullPositionView, 1.0f)) >= -ProjectiveLight.CullRange);
			
			if (InsideFrustum)
			{
				uint Index;
				InterlockedAdd(TileLightCount, 1, Index);
				TileLightIndices[Index] = ProjectiveLightIndex;
			}
		}
		
		GroupMemoryBarrierWithGroupSync();
		
		uint TileIndex = GroupId.y * ZERNShading_TileCountX + GroupId.x;
		uint TileStartOffset = (MAX_LIGHT + 2) * TileIndex;
		uint TileTotalLightCount = TileLightCount;
			
		for (uint M = GroupIndex; M < TilePointLightCount; M += TILE_SIZE)
			ZERNTiledDeferredShadingCompute_OutputTileLightIndices[TileStartOffset + 1 + M] = TileLightIndices[M];
			
		for (uint N = (GroupIndex + TilePointLightCount); N < TileTotalLightCount; N += TILE_SIZE)
			ZERNTiledDeferredShadingCompute_OutputTileLightIndices[TileStartOffset + 2 + N] = TileLightIndices[N];
			
		if (GroupIndex == 0)
		{
			ZERNTiledDeferredShadingCompute_OutputTileLightIndices[TileStartOffset] = TilePointLightCount;
			ZERNTiledDeferredShadingCompute_OutputTileLightIndices[TileStartOffset + 1 + TilePointLightCount] = TileTotalLightCount;
		}
	#elif defined ZERN_TILED_DEFERRED
		uint TilePointLightCount = TileLightCount;
		if (all(PixelCoord < GBufferDimensions) && TilePointLightCount > 0)
		{
			Surfaces[0].PositionView = ZERNTransformations_ViewportToView2(PixelCoord + 0.5f, GBufferDimensions, Surfaces[0].PositionView.z);
			Surfaces[0].Diffuse = ZERNGBuffer_GetDiffuseColor(PixelCoord);
			Surfaces[0].SubsurfaceScattering = ZERNGBuffer_GetSubsurfaceScattering(PixelCoord);
			Surfaces[0].NormalView = ZERNGBuffer_GetViewNormal(PixelCoord);
			Surfaces[0].Specular = ZERNGBuffer_GetSpecularColor(PixelCoord);
			Surfaces[0].SpecularPower = ZERNGBuffer_GetSpecularPower(PixelCoord);
			
			float3 ResultColor = 0.0f;
			
			for (uint I = 0; I < TilePointLightCount; I++)
				ResultColor += ZERNShading_PointShading(ZERNShading_PointLights[TileLightIndices[I]], Surfaces[0]);
			
			#ifdef MSAA_ENABLED
				uint2 MSAAPixelCoord = PixelCoord * uint2(2, 2);
				ZERNTiledDeferredShadingCompute_OutputColorBuffer[MSAAPixelCoord] = ResultColor;
				
				bool EdgePixel = false;
				[unroll]
				for (uint S = 1; S < SAMPLE_COUNT; S++)
				{
					Surfaces[S].NormalView = ZERNGBuffer_GetViewNormal(PixelCoord, S);
					
					EdgePixel = EdgePixel || (abs(Surfaces[S].PositionView.z - Surfaces[0].PositionView.z) > 0.1f) || (dot(Surfaces[S].NormalView, Surfaces[0].NormalView) < 0.99f);
				}
				
				if (EdgePixel)
				{
					uint Index;
					InterlockedAdd(PerSamplePixelCount, 1, Index);
					PerSamplePixels[Index] = (DispatchThreadId.x << 16) | DispatchThreadId.y;
				}
				else
				{
					[unroll]
					for (uint S = 1; S < SAMPLE_COUNT; S++)
					{
						uint2 SamplePixelCoord = MSAAPixelCoord + uint2(S & 1, S > 1);
						ZERNTiledDeferredShadingCompute_OutputColorBuffer[SamplePixelCoord] = ResultColor;
					}
				}
			#else
				ZERNTiledDeferredShadingCompute_OutputColorBuffer[PixelCoord] = ResultColor;
			#endif
		}
		
		#ifdef MSAA_ENABLED
			GroupMemoryBarrierWithGroupSync();
			
			const uint SampleCount = SAMPLE_COUNT - 1;
			uint TotalSampleCount = PerSamplePixelCount * SampleCount;
			
			for (uint GlobalSampleIndex = GroupIndex; GlobalSampleIndex < TotalSampleCount; GlobalSampleIndex += TILE_SIZE)
			{
				uint PixelIndex = GlobalSampleIndex / SampleCount;
				uint PixelXYPacked = PerSamplePixels[PixelIndex];
				uint2 PixelXY = uint2(PixelXYPacked >> 16, PixelXYPacked & 0x0000FFFF);
				uint SampleIndex = GlobalSampleIndex % SampleCount + 1;
				
				ZERNShading_Surface Surface;
				Surface.PositionView = ZERNTransformations_ViewportToView(PixelXY + 0.5f, GBufferDimensions, ZERNGBuffer_GetDepth(PixelXY, SampleIndex));
				Surface.NormalView = ZERNGBuffer_GetViewNormal(PixelXY, SampleIndex);
				Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(PixelXY, SampleIndex);
				Surface.SubsurfaceScattering = ZERNGBuffer_GetSubsurfaceScattering(PixelXY, SampleIndex);
				Surface.Specular = ZERNGBuffer_GetSpecularColor(PixelXY, SampleIndex);
				Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(PixelXY, SampleIndex);
				
				float3 ResultColor = 0.0f;
				
				for (uint I = 0; I < TilePointLightCount; I++)
					ResultColor += ZERNShading_PointShading(ZERNShading_PointLights[TileLightIndices[I]], Surface);
				
				uint2 SamplePixelCoord = PixelXY * uint2(2, 2) + uint2(SampleIndex & 1, SampleIndex > 1);
				ZERNTiledDeferredShadingCompute_OutputColorBuffer[SamplePixelCoord] = ResultColor;
			}
		#endif
	#endif
}

#endif
