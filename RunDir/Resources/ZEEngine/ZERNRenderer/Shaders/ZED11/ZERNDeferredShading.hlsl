//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNDeferredShading.hlsl
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
#include "ZERNTransformations.hlsl"
#include "ZERNShading.hlsl"

#define ZE_LT_POINT				1
#define ZE_LT_DIRECTIONAL		2
#define ZE_LT_PROJECTIVE		3
#define ZE_LT_OMNIPROJECTIVE	4

// INPUT OUTPUTS
///////////////////////////////////////////////////////////////////////////////

struct ZERNDeferredShading_Cascade
{
	float4x4						ViewTransform;
	float4x4						ProjectionTransform;
	float							DepthBias;
	float							NormalBias;
	float2							Reserved;
};

cbuffer ZERNDeferredShading_LightConstants													: register(b8)
{
	ZERNShading_Light				ZERNDeferredShading_LightInstance[2];
	float4x4						ZERNDeferredShading_LightProjectionMatrix;
	float4x4						ZERNDeferredShading_LightWorldMatrix;
};

cbuffer ZERNDeferredShading_CascadeConstants												: register(b9)
{
	ZERNDeferredShading_Cascade 	ZERNDeferredShading_Cascades[4];
	uint							ZERNDeferredShading_CascadeCount;
	float3							ZERNDeferredShading_Reserved;
};

SamplerState						ZERNDeferredShading_SamplerLinearBorder					: register(s0);
SamplerComparisonState				ZERNDeferredShading_SamplerComparisonLinearPointClamp	: register(s1);
SamplerState						ZERNDeferredShading_SamplerPointWrap					: register(s2);

Texture2D<float4>					ZERNDeferredShading_ProjectionMap						: register(t5);
Texture2DArray<float>				ZERNDeferredShading_ShadowMaps							: register(t6);
Texture2D<float2>					ZERNDeferredShading_RandomVectors						: register(t7);

static const float3 ZERNDeferredShading_CascadeColors[] = 
{
	float3(1.0f, 0.0f, 0.0f),
	float3(0.0f, 1.0f, 0.0f),
	float3(0.0f, 0.0f, 1.0f),
	float3(1.0f, 1.0f, 0.0f),
};

static const float2 ZERNDeferredShading_PoissonDiskSamples[] = 
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

float ZERNDeferredShading_CalculateVisibility(ZERNShading_Light Light, uint CascadeIndex, float3 TexCoordDepth, float2 ShadowMapDimensions)
{		
	float Visibility = 0.0f;
	
	float2 RandomVector = ZERNDeferredShading_RandomVectors.SampleLevel(ZERNDeferredShading_SamplerPointWrap, 16.0f * TexCoordDepth.xy, 0.0f) * 2.0f - 1.0f;
	RandomVector = normalize(RandomVector);
	
	for (uint I = 0; I < Light.ShadowSampleCount; I++)
	{
		float2 RandomOrientedSample = reflect(ZERNDeferredShading_PoissonDiskSamples[I], RandomVector);
		float2 Offset = RandomOrientedSample * Light.ShadowSampleLength / ShadowMapDimensions;
		Visibility += ZERNDeferredShading_ShadowMaps.SampleCmpLevelZero(ZERNDeferredShading_SamplerComparisonLinearPointClamp, float3(TexCoordDepth.xy + Offset, CascadeIndex), TexCoordDepth.z);
	}
		
	Visibility /= Light.ShadowSampleCount;
	
	return Visibility;
}

bool ZERNDeferredShading_InsideCascade(float4x4 CascadeProjectionTransform, float3 PositionView, inout float3 OutTexCoordDepth)
{
	OutTexCoordDepth = mul(CascadeProjectionTransform, float4(PositionView, 1.0f)).xyz;
	
	if (all(OutTexCoordDepth >= 0.0f && OutTexCoordDepth <= 1.0f))
		return true;
	else
		return false;
}

float4 ZERNDeferredShading_VertexShader_LightingStage(float3 Position : POSITION0) : SV_Position
{	
	if (ZERNDeferredShading_LightInstance[0].Type != ZE_LT_DIRECTIONAL)
	{
		float4 PositionWorld = mul(ZERNDeferredShading_LightWorldMatrix, float4(Position, 1.0f));
		return ZERNTransformations_WorldToProjection(PositionWorld);
	}
	
	return float4(Position, 1.0f);
};

float3 ZERNDeferredShading_PointLighting(ZERNShading_Light PointLight, ZERNShading_Surface Surface)
{
	float3 LightVectorView = PointLight.PositionView - Surface.PositionView;
	float LightDistance = length(LightVectorView);
	
	float3 ResultColor = 0.0f;
	
	if (LightDistance < PointLight.Range)
	{
		PointLight.DirectionView = LightVectorView / LightDistance;
		
		float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(PointLight, Surface);
		float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(PointLight, Surface);
		
		float DistanceAttenuation = 1.0f / dot(PointLight.Attenuation.zyx, float3(1.0f, LightDistance, LightDistance * LightDistance));
		float NdotL = dot(Surface.NormalView, PointLight.DirectionView);
		NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
		float3 LightColor = PointLight.Color * DistanceAttenuation * NdotL;
		
		ResultColor = (ResultDiffuse + ResultSpecular) * LightColor;
	}
	
	return ResultColor;
}

float3 ZERNDeferredShading_DirectionalLighting(ZERNShading_Light DirectionalLight, ZERNShading_Surface Surface)
{
	float Visibility = 1.0f;
	float3 CascadeColor = (float3)0.0f;

	if (DirectionalLight.CastShadow)
	{
		if (Surface.PositionView.z < ZERNView_ShadowDistance)
		{
			float2 ShadowMapDimensions;
			float Index = 0.0f;
			ZERNDeferredShading_ShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
		
			for (uint CascadeIndex = 0; CascadeIndex < ZERNDeferredShading_CascadeCount; CascadeIndex++)
			{	
				ZERNDeferredShading_Cascade Cascade = ZERNDeferredShading_Cascades[CascadeIndex];
				
				float3 PositionLightView = mul(Cascade.ViewTransform, float4(Surface.PositionView, 1.0f)).xyz;
				float3 NormalLightView = mul(Cascade.ViewTransform, float4(Surface.NormalView, 0.0f)).xyz;
				PositionLightView += normalize(NormalLightView) * Cascade.NormalBias;
				
				float3 TexCoordDepth;
				if (ZERNDeferredShading_InsideCascade(Cascade.ProjectionTransform, PositionLightView, TexCoordDepth))
				{
					TexCoordDepth.z += Cascade.DepthBias;
					Visibility = ZERNDeferredShading_CalculateVisibility(DirectionalLight, CascadeIndex, TexCoordDepth, ShadowMapDimensions);
					CascadeColor = ZERNDeferredShading_CascadeColors[CascadeIndex];
					
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

float3 ZERNDeferredShading_ProjectiveLighting(ZERNShading_Light ProjectiveLight, ZERNShading_Surface Surface)
{
	float4 PositionProjectionLight = mul(ZERNDeferredShading_LightProjectionMatrix, float4(Surface.PositionView, 1.0f));
	
	float3 ResultColor = (float3)0.0f;
	
	if ((PositionProjectionLight.x >= -PositionProjectionLight.w && PositionProjectionLight.x <= PositionProjectionLight.w) && 
		(PositionProjectionLight.y >= -PositionProjectionLight.w && PositionProjectionLight.y <= PositionProjectionLight.w))
	{
		float3 LightVectorView = ProjectiveLight.PositionView - Surface.PositionView;
		float LightDistanceView = length(LightVectorView);
		
		if (LightDistanceView < ProjectiveLight.Range)
		{
			float3 PositionHomogeneous = ZERNTransformations_ProjectionToHomogeneous(PositionProjectionLight);
			float2 TexCoord = ZERNTransformations_HomogeneousToTexelCorner(PositionHomogeneous.xy);
			
			float Visibility = 1.0f;
			
			if (ProjectiveLight.CastShadow)
			{
				if (Surface.PositionView.z < ZERNView_ShadowDistance)
				{
					float2 ShadowMapDimensions;
					float Index = 0.0f;
					ZERNDeferredShading_ShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
					
					float2 RandomVector = ZERNDeferredShading_RandomVectors.SampleLevel(ZERNDeferredShading_SamplerPointWrap, 4.0f * TexCoord, 0) * 2.0f - 1.0f;
					RandomVector = normalize(RandomVector);
					
					Visibility = 0.0f;
					
					for (uint I = 0; I < ProjectiveLight.ShadowSampleCount; I++)
					{	
						float2 RandomOrientedSample = reflect(ZERNDeferredShading_PoissonDiskSamples[I], RandomVector);
						float2 Offset = RandomOrientedSample * ProjectiveLight.ShadowSampleLength / ShadowMapDimensions;
						Visibility += ZERNDeferredShading_ShadowMaps.SampleCmpLevelZero(ZERNDeferredShading_SamplerComparisonLinearPointClamp, float3(TexCoord.xy + Offset, 0), PositionHomogeneous.z + ProjectiveLight.ShadowDepthBias);
					}
		
					Visibility /= ProjectiveLight.ShadowSampleCount;
				}
			}
			
			ProjectiveLight.Color *= ZERNDeferredShading_ProjectionMap.SampleLevel(ZERNDeferredShading_SamplerLinearBorder, TexCoord, 0).rgb;
			
			ProjectiveLight.DirectionView = LightVectorView / LightDistanceView;
				
			float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(ProjectiveLight, Surface);
			float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(ProjectiveLight, Surface);
				
			float DistanceAttenuation = 1.0f / dot(ProjectiveLight.Attenuation.zyx, float3(1.0f, LightDistanceView, LightDistanceView * LightDistanceView));
			float NdotL = dot(Surface.NormalView, ProjectiveLight.DirectionView);
			NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
			float3 LightColor = ProjectiveLight.Color * DistanceAttenuation * NdotL;
			
			ResultColor = (ResultDiffuse + ResultSpecular) * Visibility * LightColor;
		}
	}
	
	return ResultColor;
}

float3 ZERNDeferredShading_Lighting(ZERNShading_Surface Surface)
{
	float3 ResultColor = 0.0f;
	
	if (ZERNDeferredShading_LightInstance[0].Type == ZE_LT_DIRECTIONAL)
	{
		ResultColor = ZERNDeferredShading_DirectionalLighting(ZERNDeferredShading_LightInstance[0], Surface);
		if (ZERNDeferredShading_LightInstance[1].Type == ZE_LT_DIRECTIONAL)
			ResultColor += ZERNDeferredShading_DirectionalLighting(ZERNDeferredShading_LightInstance[1], Surface);
	}
	else if (ZERNDeferredShading_LightInstance[0].Type == ZE_LT_PROJECTIVE)
	{
		ResultColor = ZERNDeferredShading_ProjectiveLighting(ZERNDeferredShading_LightInstance[0], Surface);
	}
	else if (ZERNDeferredShading_LightInstance[0].Type == ZE_LT_POINT)
	{		
		ResultColor = ZERNDeferredShading_PointLighting(ZERNDeferredShading_LightInstance[0], Surface);
	}
	return ResultColor;
}

// LIGHTING STAGE - PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float3 ZERNDeferredShading_PixelShader_LightingStage(float4 PositionViewport : SV_Position) : SV_Target0
{
	ZERNShading_Surface Surface;
	Surface.PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, ZERNGBuffer_GetDimensions(), ZERNGBuffer_GetDepth(PositionViewport.xy));
	Surface.NormalView = ZERNGBuffer_GetViewNormal(PositionViewport.xy);
	Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(PositionViewport.xy);
	Surface.SubsurfaceScattering = ZERNGBuffer_GetSubsurfaceScattering(PositionViewport.xy);
	Surface.Specular = ZERNGBuffer_GetSpecularColor(PositionViewport.xy);
	Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(PositionViewport.xy);
	
	return ZERNDeferredShading_Lighting(Surface);
}

float3 ZERNDeferredShading_PixelShader_PerSample_LightingStage(float4 PositionViewport : SV_Position, uint SampleIndex : SV_SampleIndex) : SV_Target0
{
	ZERNShading_Surface Surface;
	Surface.PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, ZERNGBuffer_GetDimensions(), ZERNGBuffer_GetDepth(PositionViewport.xy, SampleIndex));
	Surface.NormalView = ZERNGBuffer_GetViewNormal(PositionViewport.xy, SampleIndex);
	Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(PositionViewport.xy, SampleIndex);
	Surface.SubsurfaceScattering = ZERNGBuffer_GetSubsurfaceScattering(PositionViewport.xy, SampleIndex);
	Surface.Specular = ZERNGBuffer_GetSpecularColor(PositionViewport.xy, SampleIndex);
	Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(PositionViewport.xy, SampleIndex);
	
	return ZERNDeferredShading_Lighting(Surface);
}

float3 ZERNDeferredShading_Accumulate_Emissive_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	return ZERNGBuffer_GetEmissiveColor(PositionViewport.xy);
}

void ZERNDeferredShading_EdgeDetection_PixelShader_Main(float4 PositionViewport : SV_Position)
{
	ZERNShading_Surface Surfaces[SAMPLE_COUNT];
	[unroll]
    for (uint I = 0; I < SAMPLE_COUNT; I++)
	{
		Surfaces[I].PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, ZERNGBuffer_GetDimensions(), ZERNGBuffer_GetDepth(PositionViewport.xy, I));
		Surfaces[I].NormalView = ZERNGBuffer_GetViewNormal(PositionViewport.xy, I);
	}
	
	bool EdgePixel = false;
	[unroll]
	for (uint J = 1; J < SAMPLE_COUNT; J++)
	{
		if (abs(Surfaces[J].PositionView.z - Surfaces[0].PositionView.z) > 0.1f ||
			dot(Surfaces[J].NormalView, Surfaces[0].NormalView) < 0.99f)
			EdgePixel = true;	
	}
	
	if (!EdgePixel)
		discard;
}

#endif
