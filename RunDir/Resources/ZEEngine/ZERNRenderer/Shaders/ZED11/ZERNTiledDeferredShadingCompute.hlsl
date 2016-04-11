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

#define TILE_DIMENSION		16
#define TILE_SIZE			(TILE_DIMENSION * TILE_DIMENSION)
#define MAX_LIGHT			255

#define ZE_LT_NONE				0
#define ZE_LT_POINT				1
#define ZE_LT_DIRECTIONAL		2
#define ZE_LT_PROJECTIVE		3
#define ZE_LT_OMNIPROJECTIVE	4

struct ZERNTiledDeferredShadingCompute_Cascade
{
	float4x4									ProjectionTransform;
	float4										Borders;
	float4										Band;
};

cbuffer ZERNTiledDeferredShadingCompute_Constants																	: register(b8)
{
	uint										ZERNTiledDeferredShadingCompute_LightCount;
	float3										ZERNTiledDeferredShadingCompute_Reserved;
};

cbuffer ZERNTiledDeferredShadingCompute_CascadeConstants															: register(b9)
{
	ZERNTiledDeferredShadingCompute_Cascade 	ZERNTiledDeferredShadingCompute_Cascades[4];
	uint										ZERNTiledDeferredShadingCompute_CascadeCount;
	float3										ZERNTiledDeferredShadingCompute_Reserved1;
};

SamplerComparisonState							ZERNTiledDeferredShadingCompute_SamplerComparisonLinearPointClamp	: register(s2);
SamplerState									ZERNTiledDeferredShadingCompute_SamplerPointWrap					: register(s3);

StructuredBuffer<ZERNShading_Light>				ZERNTiledDeferredShadingCompute_Lights								: register(t12);

Texture2DArray<float>							ZERNTiledDeferredShadingCompute_ShadowMaps							: register(t5);
TextureCube										ZERNTiledDeferredShadingCompute_OmniProjectionMap					: register(t7);
Texture2D<float2>								ZERNTiledDeferredShadingCompute_RandomVectors						: register(t8);
RWTexture2D<float3>								ZERNTiledDeferredShadingCompute_OutputColorBuffer					: register(u0);

groupshared uint								TileMinDepth;
groupshared uint								TileMaxDepth;

groupshared uint								TileLightCount;
groupshared uint								TileLightIndices[MAX_LIGHT];

static const float2 ZERNTiledDeferredShadingCompute_PoissonDiskSamples[] = 
{
	float2(0.7336021f, -0.6101369f),
	float2(0.9601513f, -0.0785616f),
	float2(0.3773608f, -0.8469894f),
	float2(0.5185271f, 0.1443706f),
	float2(0.03093804f, -0.4597275f),
	float2(-0.4946301f, 0.1093941f),
	float2(-0.08905835f, -0.9754086f),
	float2(-0.3824577f, -0.6261504f),
	float2(-0.05106043f, 0.2872932f),
	float2(0.4576186f, -0.2664742f),
	float2(-0.05316915f, 0.723882f),
	float2(0.5617343f, 0.7523296f),
	float2(-0.5127173f, 0.826395f),
	float2(-0.8760068f, -0.2534642f),
	float2(-0.8184632f, 0.431774f),
	float2(0.8985078f, 0.4366908f)
};

float ZERNTiledDeferredShadingCompute_CalculateVisibility(uint CascadeIndex, float3 TexCoordDepth, float2 ShadowMapDimensions)
{		
	float Visibility = 0.0f;
	
	float2 RandomVector = ZERNTiledDeferredShadingCompute_RandomVectors.SampleLevel(ZERNTiledDeferredShadingCompute_SamplerPointWrap, 4.0f * TexCoordDepth.xy, 0) * 2.0f - 1.0f;
	RandomVector = normalize(RandomVector);
	
	for(uint I = 0; I < 16; I++)
	{
		float2 RandomOrientedSample = reflect(ZERNTiledDeferredShadingCompute_PoissonDiskSamples[I], RandomVector);
		float2 Offset = RandomOrientedSample / ShadowMapDimensions;
		Visibility += ZERNTiledDeferredShadingCompute_ShadowMaps.SampleCmpLevelZero(ZERNTiledDeferredShadingCompute_SamplerComparisonLinearPointClamp, float3(TexCoordDepth.xy + Offset, CascadeIndex), TexCoordDepth.z + 0.0001f);
	}
		
	Visibility /= 16.0f;
	
	return Visibility;
}

bool ZERNTiledDeferredShadingCompute_InsideCascade(float4x4 CascadeProjectionTransform, float3 PositionView, inout float3 OutTexCoordDepth)
{
	OutTexCoordDepth = mul(CascadeProjectionTransform, float4(PositionView, 1.0f)).xyz;
	
	if (all(OutTexCoordDepth >= 0.0f && OutTexCoordDepth <= 1.0f))
		return true;
	else
		return false;
}

float3 ZERNTiledDeferredShadingCompute_PointLighting(ZERNShading_Light PointLight, ZERNShading_Surface Surface)
{
	float3 LightVectorView = PointLight.PositionView - Surface.PositionView;
	float LightDistance = length(LightVectorView);
	
	float3 ResultColor = 0.0f;
	
	if(LightDistance < PointLight.Range)
	{
		PointLight.DirectionView = LightVectorView / LightDistance;
		
		float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(PointLight, Surface);
		float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(PointLight, Surface);
		
		float DistanceAttenuation = 1.0f / dot(PointLight.Attenuation, float3(1.0f, LightDistance, LightDistance * LightDistance));
		float NdotL = dot(Surface.NormalView, PointLight.DirectionView);
		NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
		float3 LightColor = PointLight.Color * DistanceAttenuation * NdotL;
		
		ResultColor = (ResultDiffuse + ResultSpecular) * LightColor;
	}
	
	return ResultColor;
}

float3 ZERNTiledDeferredShadingCompute_DirectionalLighting(ZERNShading_Light DirectionalLight, ZERNShading_Surface Surface)
{
	float Visibility = 1.0f;

	if(DirectionalLight.CastShadow)
	{
		if(Surface.PositionView.z < ZERNView_ShadowDistance)
		{
			float2 ShadowMapDimensions;
			float Index = 0.0f;
			ZERNTiledDeferredShadingCompute_ShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
		
			for(uint CascadeIndex = 0; CascadeIndex < ZERNTiledDeferredShadingCompute_CascadeCount; CascadeIndex++)
			{	
				ZERNTiledDeferredShadingCompute_Cascade Cascade = ZERNTiledDeferredShadingCompute_Cascades[CascadeIndex];
				
				float3 TexCoordDepth;
				if(ZERNTiledDeferredShadingCompute_InsideCascade(Cascade.ProjectionTransform, Surface.PositionView, TexCoordDepth))
				{
					TexCoordDepth.z += CascadeIndex * 0.0001f;
					Visibility = ZERNTiledDeferredShadingCompute_CalculateVisibility(CascadeIndex, TexCoordDepth, ShadowMapDimensions);
					
					break;
				}
			}
		}
	}
	
	float NdotL = dot(Surface.NormalView, DirectionalLight.DirectionView);
	NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
	float3 LightColor = NdotL * DirectionalLight.Color;
	
	float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(DirectionalLight, Surface);
	float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(DirectionalLight, Surface);
	
	float3 ResultColor = (ResultDiffuse + ResultSpecular) * Visibility * LightColor;
	
	return ResultColor;
}

float3 ZERNTiledDeferredShadingCompute_OmniProjectiveLighting(ZERNShading_Light OmniProjectiveLight, ZERNShading_Surface Surface)
{	
	float3 LightVectorView = OmniProjectiveLight.PositionView - Surface.PositionView;
	float LightDistance = length(LightVectorView);
	
	float3 ResultColor = 0.0f;
	
	if(LightDistance < OmniProjectiveLight.Range)
	{
		OmniProjectiveLight.DirectionView = LightVectorView / LightDistance;
		
		float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(OmniProjectiveLight, Surface);
		float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(OmniProjectiveLight, Surface);
		
		float DistanceAttenuation = 1.0f / dot(OmniProjectiveLight.Attenuation, float3(1.0f, LightDistance, LightDistance * LightDistance));
		float NdotL = dot(Surface.NormalView, OmniProjectiveLight.DirectionView);
		NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
		float3 LightColor = OmniProjectiveLight.Color * DistanceAttenuation * NdotL;
		
		ResultColor = (ResultDiffuse + ResultSpecular) * LightColor;
	}
	
	return ResultColor;
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
	
	if(DepthHomogeneous != 0.0f)
	{
		MinDepth = min(MinDepth, Surface.PositionView.z);
		MaxDepth = max(MaxDepth, Surface.PositionView.z);
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
	
	for(uint I = 0; I < 4; I++)
		TileFrustumPlanes[I] *= rcp(length(TileFrustumPlanes[I].xyz));
	
	for(uint LightIndex = GroupIndex; LightIndex < ZERNTiledDeferredShadingCompute_LightCount; LightIndex += TILE_SIZE)
	{
		bool InsideFrustum = true;
		ZERNShading_Light Light = ZERNTiledDeferredShadingCompute_Lights[LightIndex];
		
		if (Light.Type == ZE_LT_POINT || Light.Type == ZE_LT_OMNIPROJECTIVE)
		{
			for(uint I = 0; I < 6; I++)
			{
				if(dot(TileFrustumPlanes[I], float4(Light.PositionView, 1.0f)) < -Light.Range)
				{
					InsideFrustum = false;
					break;
				}	
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
	
	if(all(PixelCoord < GBufferDimensions) && DepthHomogeneous != 0.0f)
	{
		uint LightCount = TileLightCount;
	
		if(LightCount > 0)
		{	
			//ZERNShading_Surface Surface;
			//Surface.PositionView = ZERNTransformations_ViewportToView(PixelCoord + 0.5f, GBufferDimensions, DepthHomogeneous);
			Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(PixelCoord);
			Surface.SubsurfaceScattering = ZERNGBuffer_GetSubsurfaceScattering(PixelCoord);
			Surface.NormalView = ZERNGBuffer_GetViewNormal(PixelCoord);
			Surface.Specular = ZERNGBuffer_GetSpecularColor(PixelCoord);
			Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(PixelCoord);
			
			for(uint I = 0; I < LightCount; I++)
			{
				ZERNShading_Light CurrentLight = ZERNTiledDeferredShadingCompute_Lights[TileLightIndices[I]];
				
				if (CurrentLight.Type == ZE_LT_POINT)
					ResultColor += ZERNTiledDeferredShadingCompute_PointLighting(CurrentLight, Surface);
				else if (CurrentLight.Type == ZE_LT_DIRECTIONAL)
					ResultColor += ZERNTiledDeferredShadingCompute_DirectionalLighting(CurrentLight, Surface);
				else if (CurrentLight.Type == ZE_LT_OMNIPROJECTIVE)
					ResultColor += ZERNTiledDeferredShadingCompute_OmniProjectiveLighting(CurrentLight, Surface);
			}
			
			ZERNTiledDeferredShadingCompute_OutputColorBuffer[PixelCoord] = ResultColor;
		}
	}
	
	//ZERNTiledDeferredShadingCompute_OutputColorBuffer[PixelCoord] = ResultColor;
}

#endif
