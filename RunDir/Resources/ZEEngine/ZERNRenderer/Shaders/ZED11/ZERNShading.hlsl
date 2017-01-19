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

#include "ZERNView.hlsl"
#include "ZERNSamplers.hlsl"

#define MAX_LIGHT			512

#define TILE_DIMENSION		16
#define TILE_SIZE			(TILE_DIMENSION * TILE_DIMENSION)

struct ZERNShading_PointLight
{
	float3										PositionView;
	float										Range;
	
	float3										Color;
	int											Type;
	
	float3  									Attenuation;
	bool										CastShadow;
	
	float4x4									WorldMatrix;
};

struct ZERNShading_ProjectiveLight
{
	float3										PositionView;
	float										Range;
	
	float3										CullPositionView;
	float										CullRange;
	
	float3										Color;
	uint										ShadowMapIndex;
	
	float3  									Attenuation;
	bool										CastShadow;
	
	float4x4									WorldMatrix;
	float4x4									ProjectionTransform;
	
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
	
	ZERNShading_Cascade							Cascades[4];
};

struct ZERNShading_Surface				
{
	float3										PositionView;
	float										Reserved0;
	float3										NormalView;
	float										Reserved1;
	float3										Diffuse;
	float										SubsurfaceScattering;
	float3										Specular;
	float										SpecularPower;
	float3										Ambient;
	float										Opacity;
	float3										Emissive;
	float										Reserved2;
};

cbuffer	ZERNShading_Constants																	: register(b8)
{
	ZERNShading_DirectionalLight				ZERNShading_DirectionalLights[2];
	uint										ZERNShading_DirectionalLightCount;
	uint										ZERNShading_PointLightCount;
	uint										ZERNShading_ProjectiveLightCount;
	uint										ZERNShading_TileCountX;
	
	bool										ZERNShading_AddTiledDeferredOutput;
	float3										ZERNShading_Reserved;
};

cbuffer ZERNShading_ProjectiveLight_Constants													: register(b9)
{
	ZERNShading_ProjectiveLight					ZERNShading_ProjectiveLightConstant;
};

cbuffer ZERNFog_Constants																		: register(b10)
{
	float										ZERNShading_FogDensity;
	float										ZERNShading_FogStartDistance;
	float2										ZERNShading_FogReserved0;
	
	float3										ZERNShading_FogColor;
	float										ZERNShading_FogReserved1;
};

Texture2D<float4>								ZERNShading_ProjectionTexture					: register(t10);
Texture2DArray<float>							ZERNShading_CascadeShadowMaps					: register(t11);
Texture2DArray<float>							ZERNShading_ProjectiveShadowMaps				: register(t12);
Texture2D<float2>								ZERNShading_RandomVectors						: register(t13);

StructuredBuffer<ZERNShading_PointLight>		ZERNShading_PointLights							: register(t14);
StructuredBuffer<ZERNShading_ProjectiveLight>	ZERNShading_ProjectiveLights					: register(t15);
StructuredBuffer<uint>							ZERNShading_TileLightIndices					: register(t16);

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

float4 ZERNShading_CalculateFogColor(float3 PositionView)
{
	float Distance = length(PositionView);
	float Value = max(0.0f, Distance - ZERNShading_FogStartDistance) * ZERNShading_FogDensity;
	float FogFactor = exp(-Value * Value);

	return float4(ZERNShading_FogColor, max(1.0f - FogFactor, 0.0f));
}

float ZERNShading_CalculateVisibility(uint SampleCount, float SampleLength, Texture2DArray<float> ShadowMap, uint ShadowMapIndex, float2 ShadowMapDimensions, float3 TexCoordDepth)
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

float ZERNShading_DistanceAttenuation(float3 Attenuation, float Range, float DistanceToLight)
{
	return saturate((Range - DistanceToLight) / Range) / (((DistanceToLight * DistanceToLight) / Range) + exp(3.0f * DistanceToLight / Range));
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
					Visibility = ZERNShading_CalculateVisibility(Cascade.SampleCount, Cascade.SampleLength, ZERNShading_CascadeShadowMaps, CascadeIndex, ShadowMapDimensions, TexCoordDepth);
					
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
			Visibility = ZERNShading_CalculateVisibility(ProjectiveLight.ShadowSampleCount, ProjectiveLight.ShadowSampleLength, ZERNShading_ProjectiveShadowMaps, ProjectiveLight.ShadowMapIndex, ShadowMapDimensions, TexCoordDepth);
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
					
				ResultColor = ZERNShading_TotalBRDF(LightDirectionView, Surface) * ProjectiveLight.Color * NdotL * ZERNShading_DistanceAttenuation(ProjectiveLight.Attenuation, ProjectiveLight.Range, DistanceToLight);
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
			ResultColor = ZERNShading_TotalBRDF(LightDirectionView, Surface) * PointLight.Color * NdotL * ZERNShading_DistanceAttenuation(PointLight.Attenuation, PointLight.Range, DistanceToLight);
		}
	}
	
	return ResultColor;
}

#endif
