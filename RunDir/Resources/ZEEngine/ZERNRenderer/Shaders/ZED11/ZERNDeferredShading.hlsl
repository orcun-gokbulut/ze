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
	float4x4					ProjectionTransform;
	float						NearZView;
	float						FarZView;
	float2						Band;
};

cbuffer ZERNDeferredShading_LightConstants											: register(b8)
{	
	ZERNShading_Light			ZERNDeferredShading_LightInstance;					//64
	float4x4					ZERNDeferredShading_LightProjectionMatrix;			//64
	float4x4					ZERNDeferredShading_LightWorldMatrix;				//64
	float4x4					ZERNDeferredShading_LightRotation;					//64
	bool						ZERNDeferredShading_CastShadow;
	uint						ZERNDeferredShading_SampleCount;
	float						ZERNDeferredShading_SampleLengthOffset;
	uint						ZERNDeferredShading_Reserved;
	bool						ZERNDeferredShading_ShowCascades;
	uint3						ZERNDeferredShading_Reserved2;
	float4						ZERNDeferredShading_OffsetVectors[8];				//max 16 sample
};

cbuffer ZERNDeferredShading_CascadeConstants										: register(b9)
{
	ZERNDeferredShading_Cascade ZERNDeferredShading_Cascades[4];
	uint						ZERNDeferredShading_CascadeCount;
	float3						ZERNDeferredShading_Reserved3;
};

SamplerState					ZERNDeferredShading_SamplerLinearBorder				: register(s0);
SamplerComparisonState			ZERNDeferredShading_SamplerComparisonLinearBorder	: register(s1);
SamplerState					ZERNDeferredShading_SamplerPointWrap				: register(s2);
SamplerState					ZERNDeferredShading_SamplerPointBorder				: register(s3);

Texture2DArray<float>			ZERNDeferredShading_ShadowMaps						: register(t5);
Texture2D						ZERNDeferredShading_ProjectionMap					: register(t6);
TextureCube						ZERNDeferredShading_OmniProjectionMap				: register(t7);
Texture2D<float2>				ZERNDeferredShading_RandomVectors					: register(t8);

static const float3 ZERNDeferredShading_CascadeColors[] = 
{
	float3(1.0f, 0.0f, 0.0f),
	float3(0.0f, 1.0f, 0.0f),
	float3(0.0f, 0.0f, 1.0f),
	float3(0.0f, 0.0f, 0.0f),
};

static const float ZERNDeferredShading_DepthBias = 0.0015f;

float ZERNDeferredShading_CalculateVisibility(uint CascadeIndex, float3 PositionView, float2 ShadowMapDimensions)
{	
	float4 PositionProjectionLight = mul(ZERNDeferredShading_Cascades[CascadeIndex].ProjectionTransform, float4(PositionView, 1.0f));
	
	if(PositionProjectionLight.x < -PositionProjectionLight.w || PositionProjectionLight.x > PositionProjectionLight.w || 
		PositionProjectionLight.y < -PositionProjectionLight.w || PositionProjectionLight.y > PositionProjectionLight.w)
		return 0.0f;
	
	float3 PositionHomogeneous = ZERNTransformations_ProjectionToHomogeneous(PositionProjectionLight);
	float2 TexCoord = ZERNTransformations_HomogeneousToTexelCorner(PositionHomogeneous.xy);
			
	float2 RandomVector = ZERNDeferredShading_RandomVectors.SampleLevel(ZERNDeferredShading_SamplerPointWrap, TexCoord, 0) * 2.0f - 1.0f;
	RandomVector = normalize(RandomVector);
	
	float Visibility = 0.0f;
	
	uint SampleCount = ZERNDeferredShading_SampleCount / 2;
	for(uint I = 0; I < SampleCount; I++)
	{	
		float2 OffsetVector = reflect(ZERNDeferredShading_OffsetVectors[I].xy, RandomVector);
		OffsetVector += ZERNDeferredShading_SampleLengthOffset;
		OffsetVector /= ShadowMapDimensions;
		
		Visibility += ZERNDeferredShading_ShadowMaps.SampleCmpLevelZero(ZERNDeferredShading_SamplerComparisonLinearBorder, float3(TexCoord + OffsetVector, CascadeIndex), PositionHomogeneous.z - ZERNDeferredShading_DepthBias);
		
		OffsetVector = reflect(ZERNDeferredShading_OffsetVectors[I].zw, RandomVector);
		OffsetVector += ZERNDeferredShading_SampleLengthOffset;
		OffsetVector /= ShadowMapDimensions;
		
		Visibility += ZERNDeferredShading_ShadowMaps.SampleCmpLevelZero(ZERNDeferredShading_SamplerComparisonLinearBorder, float3(TexCoord + OffsetVector, CascadeIndex), PositionHomogeneous.z - ZERNDeferredShading_DepthBias);
	}

	Visibility /= ZERNDeferredShading_SampleCount;
	
	return Visibility;
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
				float DepthView = Surface.PositionView.z;
				
				if(DepthView > Cascade.Band.x && DepthView < Cascade.Band.y)
				{
					float VisibilityCurrentCascade = ZERNDeferredShading_CalculateVisibility(CascadeIndex, Surface.PositionView, ShadowMapDimensions);
					if((CascadeIndex + 1) != ZERNDeferredShading_CascadeCount)
					{
						float VisibilityNextCascade = ZERNDeferredShading_CalculateVisibility(CascadeIndex + 1, Surface.PositionView, ShadowMapDimensions);
						
						if(VisibilityCurrentCascade != 0.0f)
						{
							float TotalWeight = Cascade.Band.y - Cascade.Band.x;
							float WeightCurrentCascade = Cascade.Band.y - DepthView;
							float WeightNextCascade = DepthView - Cascade.Band.x;
							Visibility = (WeightCurrentCascade * VisibilityCurrentCascade + WeightNextCascade * VisibilityNextCascade) / TotalWeight;
						}
						else
						{
							Visibility = VisibilityNextCascade;
						}
						
						CascadeColor = ZERNDeferredShading_CascadeColors[CascadeIndex] + ZERNDeferredShading_CascadeColors[CascadeIndex + 1];
					}
					else
					{
						Visibility = VisibilityCurrentCascade * (Cascade.Band.y - DepthView) / (Cascade.Band.y - Cascade.Band.x);
						CascadeColor = float3(0.5f, 0.5f, 0.5f);
					}
					
					break;
				}
				else if(DepthView > Cascade.NearZView && DepthView < Cascade.FarZView)
				{
					Visibility = ZERNDeferredShading_CalculateVisibility(CascadeIndex, Surface.PositionView, ShadowMapDimensions);
					CascadeColor = ZERNDeferredShading_CascadeColors[CascadeIndex];
					
					break;
				}
			}
		}
	}
	
	if(ZERNDeferredShading_ShowCascades)
	{
		return CascadeColor;
	}
	else
	{
		float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(DirectionalLight, Surface);
		float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(DirectionalLight, Surface);
		
		return (ResultDiffuse + ResultSpecular) * Visibility * DirectionalLight.Intensity;
	}
}

float3 ZERNDeferredShading_PointLighting(ZERNShading_Light PointLight, ZERNShading_Surface Surface)
{
	float3 LightVectorView = PointLight.PositionView - Surface.PositionView;
	float LightDistanceView = length(LightVectorView);
	
	PointLight.DirectionView = LightVectorView / LightDistanceView;
	
	float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(PointLight, Surface);
	float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(PointLight, Surface);
	
	float DistanceAttenuation = 1.0f / dot(PointLight.Attenuation, float3(1.0f, LightDistanceView, LightDistanceView * LightDistanceView));
	
	return (ResultDiffuse + ResultSpecular) * PointLight.Intensity * DistanceAttenuation;
}

float3 ZERNDeferredShading_ProjectiveLighting(ZERNShading_Light ProjectiveLight, ZERNShading_Surface Surface)
{
	float4 PositionProjectionLight = mul(ZERNDeferredShading_LightProjectionMatrix, float4(Surface.PositionView, 1.0f));
	float3 PositionHomogeneous = ZERNTransformations_ProjectionToHomogeneous(PositionProjectionLight);
	
	if(PositionHomogeneous.x < -1.0f || PositionHomogeneous.x > 1.0f || PositionHomogeneous.y < -1.0f || PositionHomogeneous.y > 1.0f)
		return (float3)0.0f;
	
	float2 TexCoord = ZERNTransformations_HomogeneousToTexelCorner(PositionHomogeneous.xy);
	
	float Visibility = 1.0f;
		
	if(ZERNDeferredShading_CastShadow)
	{
		if(Surface.PositionView.z < ZERNView_ShadowDistance)
		{
			float2 ShadowMapDimensions;
			float Index = 0.0f;
			ZERNDeferredShading_ShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
			
			float2 RandomVector = ZERNDeferredShading_RandomVectors.SampleLevel(ZERNDeferredShading_SamplerPointWrap, TexCoord, 0) * 2.0f - 1.0f;
			RandomVector = normalize(RandomVector);
			
			uint SampleCount = ZERNDeferredShading_SampleCount / 2;
			for(uint I = 0; I < SampleCount; I++)
			{	
				float2 OffsetVector = reflect(ZERNDeferredShading_OffsetVectors[I].xy, RandomVector);
				OffsetVector += ZERNDeferredShading_SampleLengthOffset;
				OffsetVector /= ShadowMapDimensions;
				
				Visibility += ZERNDeferredShading_ShadowMaps.SampleCmpLevelZero(ZERNDeferredShading_SamplerComparisonLinearBorder, float3(TexCoord + OffsetVector, 0), PositionHomogeneous.z - ZERNDeferredShading_DepthBias);
				
				OffsetVector = reflect(ZERNDeferredShading_OffsetVectors[I].zw, RandomVector);
				OffsetVector += ZERNDeferredShading_SampleLengthOffset;
				OffsetVector /= ShadowMapDimensions;
				
				Visibility += ZERNDeferredShading_ShadowMaps.SampleCmpLevelZero(ZERNDeferredShading_SamplerComparisonLinearBorder, float3(TexCoord + OffsetVector, 0), PositionHomogeneous.z - ZERNDeferredShading_DepthBias);
			}

			Visibility /= ZERNDeferredShading_SampleCount;
		}
	}
	
	ProjectiveLight.Color = ZERNDeferredShading_ProjectionMap.SampleLevel(ZERNDeferredShading_SamplerLinearBorder, TexCoord, 0).rgb;
	
	float3 LightVectorView = ProjectiveLight.PositionView - Surface.PositionView;
	float LightDistanceView = length(LightVectorView);
	
	ProjectiveLight.DirectionView = LightVectorView / LightDistanceView;
	
	float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(ProjectiveLight, Surface);
	float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(ProjectiveLight, Surface);
	
	float DistanceAttenuation = 1.0f / dot(ProjectiveLight.Attenuation, float3(1.0f, LightDistanceView, LightDistanceView * LightDistanceView));
	
	return (ResultDiffuse + ResultSpecular) * Visibility * ProjectiveLight.Intensity * DistanceAttenuation;
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
	
	return (ResultDiffuse + ResultSpecular) * OmniProjectiveLight.Intensity * DistanceAttenuation;
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
	
	float2 Dimensions = ZERNGBuffer_GetDimensions();
	float DepthClip = ZERNGBuffer_GetDepth(PositionViewport.xy);
	
	Surface.PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, Dimensions, DepthClip);
	Surface.NormalView = ZERNGBuffer_GetViewNormal(PositionViewport.xy);
	Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(PositionViewport.xy);
	Surface.Specular = ZERNGBuffer_GetSpecularColor(PositionViewport.xy);
	Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(PositionViewport.xy);
	
	return ZERNDeferredShading_Lighting(Surface);
}

#endif
