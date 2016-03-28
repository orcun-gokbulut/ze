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
#include "ZERNLightScatteringCommon.hlsl"

#define ZE_LT_POINT				1
#define ZE_LT_DIRECTIONAL		2
#define ZE_LT_PROJECTIVE		3
#define ZE_LT_OMNIPROJECTIVE	4

// INPUT OUTPUTS
///////////////////////////////////////////////////////////////////////////////

struct ZERNDeferredShading_Cascade
{
	float4x4						ProjectionTransform;
	float4							Borders;
	float4							Band;
};

cbuffer ZERNDeferredShading_LightConstants													: register(b8)
{	
	ZERNShading_Light				ZERNDeferredShading_LightInstance;						//64
	float4x4						ZERNDeferredShading_LightProjectionMatrix;				//64
	float4x4						ZERNDeferredShading_LightWorldMatrix;					//64
	float4x4						ZERNDeferredShading_LightRotation;						//64
	bool							ZERNDeferredShading_CastShadow;
	uint							ZERNDeferredShading_SampleCount;
	float							ZERNDeferredShading_SampleLength;
	bool							ZERNDeferredShading_ShowCascades;
	float							ZERNDeferredShading_DepthBias;
	float3							ZERNDeferredShading_Reserved0;
};

cbuffer ZERNDeferredShading_CascadeConstants												: register(b9)
{
	ZERNDeferredShading_Cascade 	ZERNDeferredShading_Cascades[4];
	uint							ZERNDeferredShading_CascadeCount;
	float3							ZERNDeferredShading_Reserved1;
};	
	
SamplerState						ZERNDeferredShading_SamplerLinearBorder					: register(s1);
SamplerComparisonState				ZERNDeferredShading_SamplerComparisonLinearPointClamp	: register(s2);
SamplerState						ZERNDeferredShading_SamplerPointWrap					: register(s3);
SamplerState						ZERNDeferredShading_SamplerPointBorder					: register(s4);

Texture2DArray<float>				ZERNDeferredShading_ShadowMaps							: register(t5);
Texture2D							ZERNDeferredShading_ProjectionMap						: register(t6);
TextureCube							ZERNDeferredShading_OmniProjectionMap					: register(t7);
Texture2D<float2>					ZERNDeferredShading_RandomVectors						: register(t8);
Texture2D<float3>					ZERNDeferredShading_TiledComputeColorBuffer				: register(t9);

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

float ZERNDeferredShading_CalculateVisibility(uint CascadeIndex, float3 TexCoordDepth, float2 ShadowMapDimensions)
{		
	float Visibility = 0.0f;
	
	float2 RandomVector = ZERNDeferredShading_RandomVectors.SampleLevel(ZERNDeferredShading_SamplerPointWrap, 4.0f * TexCoordDepth.xy, 0) * 2.0f - 1.0f;
	RandomVector = normalize(RandomVector);
	
	for(uint I = 0; I < 16; I++)
	{
		float2 RandomOrientedSample = reflect(ZERNDeferredShading_PoissonDiskSamples[I], RandomVector);
		float2 Offset = RandomOrientedSample * ZERNDeferredShading_SampleLength / ShadowMapDimensions;
		Visibility += ZERNDeferredShading_ShadowMaps.SampleCmpLevelZero(ZERNDeferredShading_SamplerComparisonLinearPointClamp, float3(TexCoordDepth.xy + Offset, CascadeIndex), TexCoordDepth.z + ZERNDeferredShading_DepthBias);
	}
		
	Visibility /= 16.0f;
	
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

float4 ZERNDeferredShading_VertexShader_LightingStage(float3 Position : POSITION0)	: SV_Position
{	
	if(ZERNDeferredShading_LightInstance.Type != ZE_LT_DIRECTIONAL)
	{
		float4 PositionWorld = mul(ZERNDeferredShading_LightWorldMatrix, float4(Position, 1.0f));
		return ZERNTransformations_WorldToProjection(PositionWorld);
	}
	
	return float4(Position, 1.0f);
};

float3 ZERNDeferredShading_DirectionalLighting(ZERNShading_Light DirectionalLight, ZERNShading_Surface Surface)
{
	float Visibility = 1.0f;
	float3 CascadeColor = (float3)0.0f;

	if(ZERNDeferredShading_CastShadow)
	{
		if(Surface.PositionView.z < ZERNView_ShadowDistance)
		{
			float2 ShadowMapDimensions;
			float Index = 0.0f;
			ZERNDeferredShading_ShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
		
			for(uint CascadeIndex = 0; CascadeIndex < ZERNDeferredShading_CascadeCount; CascadeIndex++)
			{	
				ZERNDeferredShading_Cascade Cascade = ZERNDeferredShading_Cascades[CascadeIndex];
				
				float3 TexCoordDepth;
				if(ZERNDeferredShading_InsideCascade(Cascade.ProjectionTransform, Surface.PositionView, TexCoordDepth))
				{
					TexCoordDepth.z += CascadeIndex * 0.0001f;
					Visibility = ZERNDeferredShading_CalculateVisibility(CascadeIndex, TexCoordDepth, ShadowMapDimensions);
					CascadeColor = ZERNDeferredShading_CascadeColors[CascadeIndex];
					
					break;
				}
			}
		}
	}

	float3 LightDirectionWorld = ZERNTransformations_ViewToWorld(float4(DirectionalLight.DirectionView, 0.0f));
	LightDirectionWorld = normalize(LightDirectionWorld);
	
	float3 SurfacePositionWorld = ZERNTransformations_ViewToWorld(float4(Surface.PositionView, 1.0f));
	
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);	
	float3 EarthToPosition = SurfacePositionWorld - EarthCenter;
	float EarthToPositionLength = length(EarthToPosition);
	float3 EarthNormal = EarthToPosition / EarthToPositionLength;
	
	float CosSunZenith = dot(LightDirectionWorld, EarthNormal) * 0.5f + 0.5f;
	float HeightAboveEarth = EarthToPositionLength - EARTH_RADIUS;
	
	float3 AmbientColor = ZERNLightScatteringCommon_GetAmbientColor(CosSunZenith);
	float3 Extinction = ZERNLightScatteringCommon_GetExtinctionToAtmosphere(CosSunZenith, HeightAboveEarth);
	
	float3 LightColor = DirectionalLight.Color * Extinction;
	AmbientColor *= DirectionalLight.Color;
	
	float NdotL = max(0.0f, dot(Surface.NormalView, DirectionalLight.DirectionView));
	float ENdotL = saturate(CosSunZenith);
	
	float3 ResultColor = Surface.Diffuse * (AmbientColor * ENdotL + LightColor * NdotL * Visibility);
	
	if(ZERNDeferredShading_ShowCascades)
		return ResultColor * CascadeColor;
	else
		return ResultColor;
}

float3 ZERNDeferredShading_PointLighting(ZERNShading_Light PointLight, ZERNShading_Surface Surface)
{
	float3 LightVectorView = PointLight.PositionView - Surface.PositionView;
	float LightDistanceView = length(LightVectorView);
	
	float3 ResultColor = 0.0f;
	
	if(LightDistanceView < PointLight.Range)
	{
		PointLight.DirectionView = LightVectorView / LightDistanceView;
		
		float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(PointLight, Surface);
		float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(PointLight, Surface);
		
		float DistanceAttenuation = 1.0f / dot(PointLight.Attenuation, float3(1.0f, LightDistanceView, LightDistanceView * LightDistanceView));
		
		ResultColor = (ResultDiffuse + ResultSpecular) * PointLight.Color * DistanceAttenuation;
	}
	
	return saturate(ResultColor);
}

float3 ZERNDeferredShading_ProjectiveLighting(ZERNShading_Light ProjectiveLight, ZERNShading_Surface Surface)
{
	float4 PositionProjectionLight = mul(ZERNDeferredShading_LightProjectionMatrix, float4(Surface.PositionView, 1.0f));
	
	float3 ResultColor = (float3)0.0f;
	
	if((PositionProjectionLight.x >= -PositionProjectionLight.w && PositionProjectionLight.x <= PositionProjectionLight.w) && 
		(PositionProjectionLight.y >= -PositionProjectionLight.w && PositionProjectionLight.y <= PositionProjectionLight.w))
	{
		float3 PositionHomogeneous = ZERNTransformations_ProjectionToHomogeneous(PositionProjectionLight);
		float2 TexCoord = ZERNTransformations_HomogeneousToTexelCorner(PositionHomogeneous.xy);
		
		float Visibility = 1.0f;
			
		if(ZERNDeferredShading_CastShadow)
		{
			if(Surface.PositionView.z < ZERNView_ShadowDistance)
			{
				float2 ShadowMapDimensions;
				float Index = 0.0f;
				ZERNDeferredShading_ShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
				
				float2 RandomVector = ZERNDeferredShading_RandomVectors.SampleLevel(ZERNDeferredShading_SamplerPointWrap, 4.0f * TexCoord, 0) * 2.0f - 1.0f;
				RandomVector = normalize(RandomVector);
				
				Visibility = 0.0f;
				
				for(uint I = 0; I < 16; I++)
				{	
					float2 RandomOrientedSample = reflect(ZERNDeferredShading_PoissonDiskSamples[I], RandomVector);
					float2 Offset = RandomOrientedSample * ZERNDeferredShading_SampleLength / ShadowMapDimensions;
					Visibility += ZERNDeferredShading_ShadowMaps.SampleCmpLevelZero(ZERNDeferredShading_SamplerComparisonLinearPointClamp, float3(TexCoord.xy + Offset, 0), PositionHomogeneous.z + ZERNDeferredShading_DepthBias);
					
				}
	
				Visibility /= 16.0f;
			}
		}
		
		ProjectiveLight.Color = ZERNDeferredShading_ProjectionMap.SampleLevel(ZERNDeferredShading_SamplerLinearBorder, TexCoord, 0).rgb;
		
		float3 LightVectorView = ProjectiveLight.PositionView - Surface.PositionView;
		float LightDistanceView = length(LightVectorView);
		
		ProjectiveLight.DirectionView = LightVectorView / LightDistanceView;
		
		float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(ProjectiveLight, Surface);
		float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(ProjectiveLight, Surface);
		
		float DistanceAttenuation = 1.0f / dot(ProjectiveLight.Attenuation, float3(1.0f, LightDistanceView, LightDistanceView * LightDistanceView));
		
		ResultColor = (ResultDiffuse + ResultSpecular) * Visibility * ProjectiveLight.Color * DistanceAttenuation;
	}
	
	return ResultColor;
}

float3 ZERNDeferredShading_OmniProjectiveLighting(ZERNShading_Light OmniProjectiveLight, ZERNShading_Surface Surface)
{	
	float3 LightVectorView = OmniProjectiveLight.PositionView - Surface.PositionView;
	float LightDistanceView = length(LightVectorView);
	
	OmniProjectiveLight.DirectionView = LightVectorView / LightDistanceView;
	
	float3 TexCoord = mul(ZERNDeferredShading_LightRotation, float4(OmniProjectiveLight.DirectionView, 1.0f)).xyz;
	OmniProjectiveLight.Color = ZERNDeferredShading_OmniProjectionMap.SampleLevel(ZERNDeferredShading_SamplerLinearBorder, TexCoord, 0).rgb;
	
	float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(OmniProjectiveLight, Surface);
	float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(OmniProjectiveLight, Surface);
	
	float DistanceAttenuation = 1.0f / dot(OmniProjectiveLight.Attenuation, float3(1.0f, LightDistanceView, LightDistanceView * LightDistanceView));
	
	return (ResultDiffuse + ResultSpecular) * OmniProjectiveLight.Color * DistanceAttenuation;
}

float3 ZERNDeferredShading_Lighting(ZERNShading_Surface Surface)
{
	float3 ResultColor = {0.0f, 0.0f, 0.0f};
	
	if(ZERNDeferredShading_LightInstance.Type == ZE_LT_POINT)
	{
		ResultColor = ZERNDeferredShading_PointLighting(ZERNDeferredShading_LightInstance, Surface);
	}
	else if(ZERNDeferredShading_LightInstance.Type == ZE_LT_DIRECTIONAL)
	{
		ResultColor = ZERNDeferredShading_DirectionalLighting(ZERNDeferredShading_LightInstance, Surface);
	}
	else if(ZERNDeferredShading_LightInstance.Type == ZE_LT_PROJECTIVE)
	{
		ResultColor = ZERNDeferredShading_ProjectiveLighting(ZERNDeferredShading_LightInstance, Surface);
	}
	else if(ZERNDeferredShading_LightInstance.Type == ZE_LT_OMNIPROJECTIVE)
	{
		ResultColor = ZERNDeferredShading_OmniProjectiveLighting(ZERNDeferredShading_LightInstance, Surface);
	}
	
	return saturate(ResultColor);
}

// LIGHTING STAGE - PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float3 ZERNDeferredShading_PixelShader_LightingStage(float4 PositionViewport : SV_Position) : SV_Target0
{
	ZERNShading_Surface Surface;
	Surface.PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, ZERNGBuffer_GetDimensions(), ZERNGBuffer_GetDepth(PositionViewport.xy));
	Surface.NormalView = ZERNGBuffer_GetViewNormal(PositionViewport.xy);
	Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(PositionViewport.xy);
	Surface.Specular = ZERNGBuffer_GetSpecularColor(PositionViewport.xy);
	Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(PositionViewport.xy);
	
	return ZERNDeferredShading_Lighting(Surface);
}

float3 ZERNDeferredShading_Accumulate_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	return ZERNDeferredShading_TiledComputeColorBuffer.Load(int3(PositionViewport.xy, 0));
}

#endif
