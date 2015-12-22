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

cbuffer ZERNDeferredShading_LightConstants										: register(b8)
{
	ZERNShading_Light			ZERNDeferredShading_LightInstance;				//64
	float4x4					ZERNDeferredShading_LightProjectionMatrix;		//64
	float4x4					ZERNDeferredShading_LightWorldMatrix;			//64
	float3x3					ZERNDeferredShading_LightRotation;				//36
	bool						ZERNDeferredShading_CastShadow;					//4
	float2						ZERNDeferredShading_Reserved;					//8
};

SamplerState					ZERNDeferredShading_SamplerLinearBorder			: register(s0);
SamplerState					ZERNDeferredShading_SamplerPointBorder			: register(s1);

Texture2D						ZERNDeferredShading_ShadowMap					: register(t5);
Texture2D						ZERNDeferredShading_ProjectionMap				: register(t6);
TextureCube						ZERNDeferredShading_OmniProjectionMap			: register(t7);

float4 ZERNDeferredShading_VertexShader_LightingStage(float3 Position : POSITION0) : SV_Position
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
	float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(DirectionalLight, Surface);
	float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(DirectionalLight, Surface);
	
	return (ResultDiffuse + ResultSpecular) * DirectionalLight.Intensity;
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
	float4 PositionProjection = mul(ZERNDeferredShading_LightProjectionMatrix, float4(Surface.PositionView, 1.0f));
	float2 TexCoord = ZERNTransformations_ProjectionToTexelCorner(PositionProjection);

	float DepthInShadowMap = ZERNDeferredShading_ShadowMap.SampleLevel(ZERNDeferredShading_SamplerPointBorder, TexCoord, 0).r;
	if(ZERNDeferredShading_CastShadow)
	{
		if(PositionProjection.w > (DepthInShadowMap * ProjectiveLight.Range))
		return (float3)0.0f;
	}
	
	ProjectiveLight.Color = ZERNDeferredShading_ProjectionMap.SampleLevel(ZERNDeferredShading_SamplerLinearBorder, TexCoord, 0).rgb;
	
	float3 LightVectorView = ProjectiveLight.PositionView - Surface.PositionView;
	float LightDistanceView = length(LightVectorView);
	
	ProjectiveLight.DirectionView = LightVectorView / LightDistanceView;
	
	float3 ResultDiffuse = ZERNShading_Diffuse_Lambert(ProjectiveLight, Surface);
	float3 ResultSpecular = ZERNShading_Specular_BlinnPhong(ProjectiveLight, Surface);
	
	float DistanceAttenuation = 1.0f / dot(ProjectiveLight.Attenuation, float3(1.0f, LightDistanceView, LightDistanceView * LightDistanceView));
	
	return (ResultDiffuse + ResultSpecular) * ProjectiveLight.Intensity * DistanceAttenuation;
}

float3 ZERNDeferredShading_OmniProjectiveLighting(ZERNShading_Light OmniProjectiveLight, ZERNShading_Surface Surface)
{	
	float3 LightVectorView = OmniProjectiveLight.PositionView - Surface.PositionView;
	float LightDistanceView = length(LightVectorView);
	
	OmniProjectiveLight.DirectionView = LightVectorView / LightDistanceView;
	
	float3 TexCoord = mul(ZERNDeferredShading_LightRotation, OmniProjectiveLight.DirectionView);
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
