//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNShading.hlsl
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

#ifndef __ZERN_SHADING_H__
#define __ZERN_SHADING_H__

#include "ZERNGBuffer.hlsl"
#include "ZERNView.hlsl"
#include "ZERNSamplers.hlsl"
#include "ZERNTransformations.hlsl"

#define SAMPLE_COUNT					4

#define MAX_EMITTER						1000
#define MAX_DEFERRED_LIGHT				1024
#define MAX_TILED_LIGHT					1021
#define MAX_POINT_LIGHT_SHADOW			2
#define MAX_SPOT_LIGHT_SHADOW			4
#define MAX_PROJECTIVE_LIGHT			8
#define MAX_PROJECTIVE_LIGHT_SHADOW		4
#define MAX_DIRECTIONAL_LIGHT			2
#define MAX_DIRECTIONAL_LIGHT_SHADOW	1
#define MAX_CASCADE						4

#define TILE_LIGHT_HEADER_COUNT			3
#define TILE_LIGHT_TOTAL_COUNT			(TILE_LIGHT_HEADER_COUNT + MAX_TILED_LIGHT)

#define TILE_PARTICLE_HEADER_COUNT		1
#define TILE_PARTICLE_COUNT				1023
#define TILE_PARTICLE_TOTAL_COUNT		(TILE_PARTICLE_HEADER_COUNT + TILE_PARTICLE_COUNT)

#define TILE_PARTICLE_DIM				16
#define TILE_LIGHT_DIM					32

#define TILE_PARTICLE_SIZE				(TILE_PARTICLE_DIM * TILE_PARTICLE_DIM)
#define TILE_LIGHT_SIZE					(TILE_LIGHT_DIM * TILE_LIGHT_DIM)

#define WORK_COUNT						4

struct ZERNShading_PointLight
{
	float3										PositionView;
	float										Range;
	
	float3										Color;
	float										FalloffExponent;
};

struct ZERNShading_SpotLight
{
	float3										PositionView;
	float										Range;
	
	float3										DirectionView;
	float										CosOuterConeAngle;
	
	float3										Color;
	float										FalloffExponent;
	
	float										CosInnerConeAngle;
	bool										CastShadow;
	uint										ShadowIndex;
	uint										Reserved;
};

struct ZERNShading_SpotLightShadow
{
	float4x4									ProjectionTransform;
	uint										ShadowSampleCount;
	float										ShadowSampleLength;
	float										ShadowDepthBias;
	float										ShadowNormalBias;
};

struct ZERNShading_ProjectiveLight
{
	float3										PositionView;
	float										Range;
	
	float3										Color;
	float										FalloffExponent;
	
	float4x4									ProjectionTransform;
	
	bool										CastShadow;
	uint										ShadowMapIndex;
	float2										Reserved;
	uint										ShadowSampleCount;
	float										ShadowSampleLength;
	float										ShadowDepthBias;
	float										ShadowNormalBias;
};

struct ZERNShading_Cascade
{
	float4x4									ViewTransform;
	float4x4									ProjectionTransform;
	
	uint										SampleCount;
	float										SampleLength;
	float										DepthBias;
	float										NormalBias;
};

struct ZERNShading_DirectionalLight
{
	float3										DirectionView;
	bool										CastShadow;
	
	float3										Color;
	uint										CascadeCount;
	
	uint										ShadowMapIndex;
	float3										Reserved;
	
	ZERNShading_Cascade							Cascades[MAX_CASCADE];
};

struct ZERNShading_Surface
{
	float3										PositionView;
	float3										NormalView;
	float3										Diffuse;
	float										SubsurfaceScattering;
	float3										Specular;
	float										SpecularPower;
	float3										Ambient;
	float										Opacity;
	float3										Emissive;
};

struct ZERNShading_Particle
{
	float3										Position;
	float										Radius;
	float4										Color;
	float2										Size;
	float										Rotation;
	bool										LightReceiver;
};

struct ZERNShading_Emitter
{
	uint										Axis;
	uint										Flags;
	
	uint										StartOffset;
	uint										ParticleCount;
	
	uint										ColorStartOffset;
	float2										Size;
	float										Rotation;
	float4										TexCoords;
	float4										Color;
};

cbuffer ZERNShading_EdgeDetection_Constants														: register(b9)
{
	float										ZERNShading_EdgeDetection_DepthThreshold;
	float										ZERNShading_EdgeDetection_NormalThreshold;
	float2										ZERNShading_EdgeDetection_Reserved;
};

cbuffer	ZERNShading_Constants																	: register(b8)
{
	ZERNShading_DirectionalLight				ZERNShading_DirectionalLights[MAX_DIRECTIONAL_LIGHT];
	uint										ZERNShading_DirectionalLightCount;
	uint										ZERNShading_PointLightCount;
	uint										ZERNShading_SpotLightCount;
	uint										ZERNShading_ProjectiveLightCount;
	
	uint										ZERNShading_TileCountX;
	float3										ZERNShading_Reserved;
};

cbuffer ZERNShading_PointLights_Constants														: register(b9)
{
	ZERNShading_PointLight						ZERNShading_PointLights[MAX_TILED_LIGHT];
};

cbuffer ZERNShading_SpotLights_Constants														: register(b10)
{
	ZERNShading_SpotLight						ZERNShading_SpotLights[MAX_TILED_LIGHT];
};

cbuffer ZERNShading_SpotLightShadow_Constants													: register(b11)
{
	ZERNShading_SpotLightShadow					ZERNShading_SpotLightsShadow[MAX_SPOT_LIGHT_SHADOW];
};

cbuffer ZERNShading_ProjectiveLight_Constants													: register(b9)
{
	ZERNShading_ProjectiveLight					ZERNShading_ProjectiveLightConstant;
};

cbuffer ZERNFog_Constants																		: register(b12)
{
	float										ZERNShading_FogDensity;
	float										ZERNShading_FogStartDistance;
	float										ZERNShading_FogHeight;
	float										ZERNShading_FogReserved0;
	
	float3										ZERNShading_FogColor;
	float										ZERNShading_FogReserved1;
};

cbuffer ZERNShading_Emitters_Constants															: register(b13)
{
	ZERNShading_Emitter							ZERNShading_Emitters[MAX_EMITTER];
};

Texture2D<float4>								ZERNShading_ProjectionTexture					: register(t10);
Texture2DArray<float>							ZERNShading_CascadeShadowMaps					: register(t11);
Texture2DArray<float>							ZERNShading_SpotShadowMaps						: register(t12);
Texture2DArray<float>							ZERNShading_ProjectiveShadowMaps				: register(t13);
Texture2D<float2>								ZERNShading_RandomVectors						: register(t14);

StructuredBuffer<uint>							ZERNShading_TileLightIndices					: register(t15);
StructuredBuffer<uint>							ZERNShading_TileParticleIndices					: register(t16);
Buffer<float4>									ZERNShading_ParticlePool						: register(t17);

static const float2 ZERNShading_PoissonDiskSamples[] = 
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

ZERNShading_Surface ZERNShading_GetSurfaceFromGBuffers(float2 PositionViewport
//#ifdef MSAA_ENABLED
, float SampleIndex = 0
//#endif
)
{
	#ifdef MSAA_ENABLED
		float2 GBufferDimensions;
		float Dummy;
		ZERNGBuffer_DepthBuffer.GetDimensions(GBufferDimensions.x, GBufferDimensions.y, Dummy);
		float Depth = ZERNGBuffer_DepthBuffer.Load(PositionViewport, SampleIndex).x;
		float4 VelocitySpecularPower = ZERNGBuffer_Buffer1.Load(PositionViewport, SampleIndex);
		float4 DiffuseSubsurface = ZERNGBuffer_Buffer2.Load(PositionViewport, SampleIndex);
		float4 NormalSpecularColor = ZERNGBuffer_Buffer3.Load(PositionViewport, SampleIndex);
	#else
		float2 GBufferDimensions;
		ZERNGBuffer_DepthBuffer.GetDimensions(GBufferDimensions.x, GBufferDimensions.y);
		float Depth = ZERNGBuffer_DepthBuffer.Load(int3(PositionViewport, 0)).x;
		float4 VelocitySpecularPower = ZERNGBuffer_Buffer1.Load(int3(PositionViewport, 0));
		float4 DiffuseSubsurface = ZERNGBuffer_Buffer2.Load(int3(PositionViewport, 0));
		float4 NormalSpecularColor = ZERNGBuffer_Buffer3.Load(int3(PositionViewport, 0));
	#endif
	
	ZERNShading_Surface Surface;
	Surface.PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, GBufferDimensions, Depth);
	Surface.SpecularPower = VelocitySpecularPower.w;
	Surface.Diffuse = DiffuseSubsurface.xyz;
	Surface.SubsurfaceScattering = DiffuseSubsurface.w;
	Surface.NormalView = normalize(NormalSpecularColor.xyz * 2.0f - 1.0f);
	Surface.Specular = NormalSpecularColor.www;
	
	return Surface;
}

bool ZERNShading_EdgeDetection_MSAA(ZERNShading_Surface Surfaces[SAMPLE_COUNT])
{
	bool EdgePixel = false;
	[unroll]
	for (uint S = 1; S < SAMPLE_COUNT; S++)
	{
		EdgePixel = EdgePixel || 
		(abs(Surfaces[S].PositionView.z - Surfaces[0].PositionView.z) > ZERNShading_EdgeDetection_DepthThreshold) || 
		(dot(Surfaces[S].NormalView, Surfaces[0].NormalView) < ZERNShading_EdgeDetection_NormalThreshold);
	}
	
	return EdgePixel;
}

float4 ZERNShading_CalculateFogColor(float3 PositionView)
{
	float Distance = length(PositionView);
	float Value = max(0.0f, Distance - ZERNShading_FogStartDistance) * ZERNShading_FogDensity;
	float FogFactor = exp(-Value * Value);

	return float4(ZERNShading_FogColor, max(1.0f - FogFactor, 0.0f));
}

float ZERNShading_CalculateVisibility(uint SampleCount, float SampleLength, Texture2D<float> ShadowMap, float2 ShadowMapDimensions, float3 TexCoordDepth)
{
	float Visibility = 0.0f;
	
	float2 RandomVector = ZERNShading_RandomVectors.SampleLevel(ZERNSampler_PointWrap, 4.0f * TexCoordDepth.xy, 0.0f) * 2.0f - 1.0f;
	RandomVector = normalize(RandomVector);
	
	for (uint I = 0; I < SampleCount; I++)
	{
		float2 RandomOrientedSample = reflect(ZERNShading_PoissonDiskSamples[I], RandomVector);
		float2 Offset = RandomOrientedSample * SampleLength / ShadowMapDimensions;
		Visibility += ShadowMap.SampleCmpLevelZero(ZERNSampler_ComparisonLinearPointClamp, TexCoordDepth.xy + Offset, TexCoordDepth.z);
	}
	
	Visibility /= SampleCount;
	
	return Visibility;
}

float ZERNShading_CalculateVisibilityArray(uint SampleCount, float SampleLength, Texture2DArray<float> ShadowMap, uint ShadowMapIndex, float2 ShadowMapDimensions, float3 TexCoordDepth)
{		
	float Visibility = 0.0f;
	
	float2 RandomVector = ZERNShading_RandomVectors.SampleLevel(ZERNSampler_PointWrap, 4.0f * TexCoordDepth.xy, 0.0f) * 2.0f - 1.0f;
	RandomVector = normalize(RandomVector);
	
	for (uint I = 0; I < SampleCount; I++)
	{
		float2 RandomOrientedSample = reflect(ZERNShading_PoissonDiskSamples[I], RandomVector);
		float2 Offset = RandomOrientedSample * SampleLength / ShadowMapDimensions;
		Visibility += ShadowMap.SampleCmpLevelZero(ZERNSampler_ComparisonLinearPointClamp, float3(TexCoordDepth.xy + Offset, ShadowMapIndex), TexCoordDepth.z);
	}
	
	Visibility /= SampleCount;
	
	return Visibility;
}

bool ZERNShading_InsideLightVolume(float4x4 VolumeProjectionTransform, float3 PositionVector, inout float3 OutTexCoordDepth)
{
	float4 TexCoordDepth = mul(VolumeProjectionTransform, float4(PositionVector, 1.0f));
	OutTexCoordDepth = TexCoordDepth.xyz / TexCoordDepth.w;
	
	if (all(OutTexCoordDepth >= 0.0f && OutTexCoordDepth <= 1.0f))
		return true;
	else
		return false;
}

float3 ZERNShading_Diffuse_Lambert(float3 LightDirectionView, ZERNShading_Surface Surface)
{
	return (1.0f - Surface.SubsurfaceScattering) * Surface.Diffuse;
}

float3 ZERNShading_Specular_BlinnPhong(float3 LightDirectionView, ZERNShading_Surface Surface)
{	
	float3 ViewDirection = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDirection + LightDirectionView);
	float NdotH = max(0.0f, dot(Surface.NormalView, HalfVector));
	
	return pow(NdotH, Surface.SpecularPower) * Surface.Specular;
}

float3 ZERNShading_TotalBRDF(float3 LightDirectionView, ZERNShading_Surface Surface)
{
	float3 ResultSurfaceDiffuse = ZERNShading_Diffuse_Lambert(LightDirectionView, Surface);
	float3 ResultSurfaceSpecular = ZERNShading_Specular_BlinnPhong(LightDirectionView, Surface);
	
	return (ResultSurfaceDiffuse + ResultSurfaceSpecular);
}

float ZERNShading_DistanceAttenuation(float FalloffExponent, float Range, float DistanceToLight)
{
	return saturate((Range - DistanceToLight) / Range) / (((DistanceToLight * DistanceToLight) / Range) + exp(FalloffExponent * DistanceToLight / Range));
}

float3 ZERNShading_DirectionalShadowing(ZERNShading_DirectionalLight DirectionalLight, ZERNShading_Surface Surface)
{
	float Visibility = 1.0f;
	
	if (DirectionalLight.CastShadow)
	{	
		if (Surface.PositionView.z < ZERNView_ShadowDistance)
		{
			float2 ShadowMapDimensions;
			float Index = 0.0f;
			ZERNShading_CascadeShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
		
			for (uint CascadeIndex = 0; CascadeIndex < DirectionalLight.CascadeCount; CascadeIndex++)
			{
				ZERNShading_Cascade Cascade = DirectionalLight.Cascades[CascadeIndex];
				
				float3 PositionLightView = mul(Cascade.ViewTransform, float4(Surface.PositionView, 1.0f)).xyz;
				float3 NormalLightView = mul(Cascade.ViewTransform, float4(Surface.NormalView, 0.0f)).xyz;
				PositionLightView += normalize(NormalLightView) * Cascade.NormalBias;
				
				float3 TexCoordDepth;
				if (ZERNShading_InsideLightVolume(Cascade.ProjectionTransform, PositionLightView, TexCoordDepth))
				{
					TexCoordDepth.z += Cascade.DepthBias;
					Visibility = ZERNShading_CalculateVisibilityArray(Cascade.SampleCount, Cascade.SampleLength, ZERNShading_CascadeShadowMaps, DirectionalLight.ShadowMapIndex + CascadeIndex, ShadowMapDimensions, TexCoordDepth);
					
					break;
				}
			}
		}
	}
	
	return Visibility;
}

float3 ZERNShading_DirectionalShading(ZERNShading_DirectionalLight DirectionalLight, ZERNShading_Surface Surface)
{
	float3 ResultColor = 0.0f;
	
	float NdotL = dot(DirectionalLight.DirectionView, Surface.NormalView);
	NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
	if (NdotL > 0.0f)
	{	
		ResultColor = ZERNShading_TotalBRDF(DirectionalLight.DirectionView, Surface) * DirectionalLight.Color * NdotL;
		ResultColor *= ZERNShading_DirectionalShadowing(DirectionalLight, Surface);
	}
	
	return ResultColor;
}

float3 ZERNShading_ProjectiveShadowing(ZERNShading_ProjectiveLight ProjectiveLight, ZERNShading_Surface Surface, float3 TexCoordDepth)
{
	float Visibility = 1.0f;
	
	if (ProjectiveLight.CastShadow)
	{
		if (Surface.PositionView.z < ZERNView_ShadowDistance)
		{
			float2 ShadowMapDimensions;
			float Index = 0.0f;
			ZERNShading_ProjectiveShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
			
			TexCoordDepth.z += ProjectiveLight.ShadowDepthBias;
			Visibility = ZERNShading_CalculateVisibilityArray(ProjectiveLight.ShadowSampleCount, ProjectiveLight.ShadowSampleLength, ZERNShading_ProjectiveShadowMaps, ProjectiveLight.ShadowMapIndex, ShadowMapDimensions, TexCoordDepth);
		}
	}
	
	return Visibility;
}

float3 ZERNShading_ProjectiveShading(ZERNShading_ProjectiveLight ProjectiveLight, ZERNShading_Surface Surface)
{	
	float3 ResultColor = 0.0f;
	
	float3 VectorToLight = ProjectiveLight.PositionView - Surface.PositionView;
	float DistanceToLight = length(VectorToLight);
	if (DistanceToLight < ProjectiveLight.Range)
	{	
		float3 LightDirectionView = VectorToLight / DistanceToLight;
		
		float NdotL = dot(LightDirectionView, Surface.NormalView);
		NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
		if (NdotL > 0.0f)
		{		
			float3 TexCoordDepth;
			if (ZERNShading_InsideLightVolume(ProjectiveLight.ProjectionTransform, Surface.PositionView, TexCoordDepth))
			{			
				ProjectiveLight.Color *= ZERNShading_ProjectionTexture.SampleLevel(ZERNSampler_LinearBorderZero, TexCoordDepth.xy, 0.0f).rgb;
					
				ResultColor = ZERNShading_TotalBRDF(LightDirectionView, Surface) * ProjectiveLight.Color * NdotL * ZERNShading_DistanceAttenuation(ProjectiveLight.FalloffExponent, ProjectiveLight.Range, DistanceToLight);
				ResultColor *= ZERNShading_ProjectiveShadowing(ProjectiveLight, Surface, TexCoordDepth);
			}
		}
	}
	
	return ResultColor;
}

float3 ZERNShading_PointShading(ZERNShading_PointLight PointLight, ZERNShading_Surface Surface)
{
	float3 ResultColor = 0.0f;
	
	float3 VectorToLight = PointLight.PositionView - Surface.PositionView;
	float DistanceToLight = length(VectorToLight);
	if (DistanceToLight < PointLight.Range)
	{		
		float3 LightDirectionView = VectorToLight / DistanceToLight;
		
		float NdotL = dot(LightDirectionView, Surface.NormalView);
		NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
		if (NdotL > 0.0f)
		{		
			ResultColor = ZERNShading_TotalBRDF(LightDirectionView, Surface) * PointLight.Color * NdotL * ZERNShading_DistanceAttenuation(PointLight.FalloffExponent, PointLight.Range, DistanceToLight);
		}
	}
	
	return ResultColor;
}

float3 ZERNShading_SpotShadowing(ZERNShading_SpotLight SpotLight, ZERNShading_Surface Surface)
{
	float Visibility = 1.0f;
	
	[branch]
	if (SpotLight.CastShadow)
	{
		ZERNShading_SpotLightShadow SpotLightShadow = ZERNShading_SpotLightsShadow[SpotLight.ShadowIndex];

		float3 TexCoordDepth;
		if (ZERNShading_InsideLightVolume(SpotLightShadow.ProjectionTransform, Surface.PositionView, TexCoordDepth))
		{
			float2 ShadowMapDimensions;
			float Index = 0.0f;
			ZERNShading_SpotShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
		
			TexCoordDepth.z += SpotLightShadow.ShadowDepthBias;
			Visibility = ZERNShading_CalculateVisibilityArray(SpotLightShadow.ShadowSampleCount, SpotLightShadow.ShadowSampleLength, ZERNShading_SpotShadowMaps, SpotLight.ShadowIndex, ShadowMapDimensions, TexCoordDepth);
		}
	}

	return Visibility;
}

float3 ZERNShading_SpotShading(ZERNShading_SpotLight SpotLight, ZERNShading_Surface Surface)
{
	float3 ResultColor = 0.0f;

	float3 VectorToLight = SpotLight.PositionView - Surface.PositionView;
	float DistanceToLight = length(VectorToLight);
	if (DistanceToLight < SpotLight.Range)
	{
		float3 LightDirectionView = VectorToLight / DistanceToLight;
		float CosAlpha = dot(SpotLight.DirectionView, LightDirectionView);
		
		if (CosAlpha > SpotLight.CosOuterConeAngle)
		{
			float NdotL = dot(LightDirectionView, Surface.NormalView);
			NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
			if (NdotL > 0.0f)
			{
				ResultColor = ZERNShading_TotalBRDF(LightDirectionView, Surface) * SpotLight.Color * NdotL * 
				pow(saturate((CosAlpha - SpotLight.CosOuterConeAngle) / (SpotLight.CosInnerConeAngle - SpotLight.CosOuterConeAngle)), SpotLight.FalloffExponent) * 
				ZERNShading_DistanceAttenuation(SpotLight.FalloffExponent, SpotLight.Range, DistanceToLight);

				ResultColor *= ZERNShading_SpotShadowing(SpotLight, Surface);
			}
		}
	}
	
	return ResultColor;
}

float3 ZERNShading_Shade(float2 PositionViewport, ZERNShading_Surface Surface)
{
	float3 ResultColor = 0.0f;
	
	uint2 TileId = floor(PositionViewport) / TILE_LIGHT_DIM;
	uint TileIndex = TileId.y * ZERNShading_TileCountX + TileId.x;
	uint TileStartOffset = TileIndex * TILE_LIGHT_TOTAL_COUNT;
	
	uint TilePointLightCount = ZERNShading_TileLightIndices[TileStartOffset];
	#ifdef ZERN_SHADING_TRANSPARENT
		uint TilePointLightCountTransparent = ZERNShading_TileLightIndices[TileStartOffset + 1];
		uint TileSpotLightCountTransparent = ZERNShading_TileLightIndices[TileStartOffset + 2];
		
		TilePointLightCount += TilePointLightCountTransparent;
	#endif
	
	[unroll(2)]
	for (uint I = 0; I < ZERNShading_DirectionalLightCount; I++)
		ResultColor += ZERNShading_DirectionalShading(ZERNShading_DirectionalLights[I], Surface);
	
	uint TailCount = TilePointLightCount & (WORK_COUNT - 1);
	uint BufferIndex = TileStartOffset + TILE_LIGHT_HEADER_COUNT;
	uint LightIndices[WORK_COUNT];
	
	[unroll(WORK_COUNT - 1)]
	for (uint JJ = 0; JJ < TailCount; JJ++)
	{
		LightIndices[JJ] = ZERNShading_TileLightIndices[BufferIndex++];
		
		ResultColor += ZERNShading_PointShading(ZERNShading_PointLights[LightIndices[JJ]], Surface);
	}
	
	for (uint J = TailCount; J < TilePointLightCount; J += WORK_COUNT)
	{
		[unroll]
		for (int W = 0; W < WORK_COUNT; W++)
			LightIndices[W] = ZERNShading_TileLightIndices[BufferIndex++];
		
		[unroll]
		for (int WW = 0; WW < WORK_COUNT; WW++)
			ResultColor += ZERNShading_PointShading(ZERNShading_PointLights[LightIndices[WW]], Surface);
	}
	
	#ifdef ZERN_SHADING_TRANSPARENT
		TailCount = (TileSpotLightCountTransparent & (WORK_COUNT - 1));
		
		[unroll(WORK_COUNT - 1)]
		for (uint KK = 0; KK < TailCount; KK++)
		{
			LightIndices[KK] = ZERNShading_TileLightIndices[BufferIndex++];
			
			ResultColor += ZERNShading_SpotShading(ZERNShading_SpotLights[LightIndices[KK]], Surface);
		}
		
		for (uint K = TailCount; K < TileSpotLightCountTransparent; K += WORK_COUNT)
		{
			[unroll]
			for (int W = 0; W < WORK_COUNT; W++)
				LightIndices[W] = ZERNShading_TileLightIndices[BufferIndex++];
			
			[unroll]
			for (int WW = 0; WW < WORK_COUNT; WW++)
				ResultColor += ZERNShading_SpotShading(ZERNShading_SpotLights[LightIndices[WW]], Surface);
		}
		
		ResultColor += Surface.Ambient + Surface.Emissive;
		
		float4 FogColor = ZERNShading_CalculateFogColor(Surface.PositionView);
		ResultColor = lerp(ResultColor, FogColor.rgb, FogColor.a);
	#endif
	
	return ResultColor;	
}

#endif
