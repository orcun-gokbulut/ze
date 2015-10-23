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

#define ZE_LT_NONE				0
#define ZE_LT_POINT				1
#define ZE_LT_DIRECTIONAL		2
#define ZE_LT_PROJECTIVE		3
#define ZE_LT_OMNIPROJECTIVE	4
	
// INPUT OUTPUTS
///////////////////////////////////////////////////////////////////////////////

struct ZERNDeferredShading_Light
{
	float3	PositionWorld;
	float	Range;
	float3	Color;
	float	Intensity;
	float3  Attenuation;
	float	Fov;
	float3	Direction;
	int		Type;
};

struct ZERNDeferredShading_Surface
{
	float3	PositionView;
	float	Reserved;
	float3	NormalView;
	float	Reserved1;
	float3	Diffuse;
	float	Reserved2;
	float3	Specular;
	float	SpecularPower;
};

Texture2D					ProjectionMap			:	register(t5);
TextureCube					OmniProjectionMap		:	register(t6);

cbuffer ZERNDeferredShading_LightConstants			:	register(b8)
{
	ZERNDeferredShading_Light	ZERNDeferredShading_LightInstance;				//64
	float4x4					ZERNDeferredShading_LightProjectionMatrix;		//64
	float4x4					ZERNDeferredShading_LightWorldMatrix;			//64
	float3x3					ZERNDeferredShading_LightRotation;				//36
	float						ZERNDeferredShading_Reserved[3];				//12
};

SamplerState ZERNDeferredShading_ProjectionSampler
{
    Filter		= MIN_MAG_MIP_LINEAR;
    AddressU	= Wrap;
    AddressV	= Wrap;
};

struct ZERNDeferredShading_VSInput
{
	float3 Position	: POSITION0;
	float3 Normal	: NORMAL0;
	float2 Texcoord : TEXCOORD0;
	float3 Color	: COLOR0;
};

struct ZERNDeferredShading_PSInput 
{
	float4 PositionViewport	: SV_Position;
};

ZERNDeferredShading_PSInput ZERNDeferredShading_VertexShader_LightingStage(ZERNDeferredShading_VSInput Input)
{
	ZERNDeferredShading_PSInput Output;
	
	if(ZERNDeferredShading_LightInstance.Type != ZE_LT_DIRECTIONAL)
	{
		float4x4 WorldViewProjectionTransform = mul(ZERNView_ProjectionTransform, ZERNView_ViewTransform);
		WorldViewProjectionTransform = mul(WorldViewProjectionTransform, ZERNDeferredShading_LightWorldMatrix);
		Output.PositionViewport = mul(WorldViewProjectionTransform, float4(Input.Position, 1.0f));
		
		return Output;
	}
	
	Output.PositionViewport = float4(Input.Position, 1.0f);
	
	return Output;
};

float3 ZERNDeferredShading_PointLighting(ZERNDeferredShading_Light PointLight, ZERNDeferredShading_Surface Surface)
{
	float3 LightPositionView = mul(ZERNView_ViewTransform, float4(PointLight.PositionWorld, 1.0f)).xyz;
	float3 LightDir = LightPositionView - Surface.PositionView;
	float LightDist = length(LightDir);
	LightDir = LightDir / LightDist;
	float NdotL = max(0, dot(Surface.NormalView, LightDir));
	
	float3 ViewDir = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDir + LightDir);
	float NdotH = max(0, dot(Surface.NormalView, HalfVector));
	float DistanceAttenuation = 1.0f / dot(PointLight.Attenuation, float3(1.0f, LightDist, LightDist * LightDist));
	
	float3 ResultSpecular = pow(NdotH, Surface.SpecularPower) * Surface.Specular;
	float3 ResultDiffuse = NdotL * PointLight.Color * Surface.Diffuse;
	
	return (ResultDiffuse + ResultSpecular) * PointLight.Intensity * DistanceAttenuation;
}

float3 ZERNDeferredShading_DirectionalLighting(ZERNDeferredShading_Light DirectionalLight, ZERNDeferredShading_Surface Surface)
{
	float3 DirectionView = mul(ZERNView_ViewTransform, float4(DirectionalLight.Direction, 0.0f)).xyz;
	DirectionView = normalize(DirectionView);
	float NdotL = max(0, dot(Surface.NormalView, DirectionView));
	
	float3 ViewDir = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDir + DirectionView);
	float NdotH = max(0, dot(Surface.NormalView, HalfVector));
	
	float3 ResultSpecular = pow(NdotH, Surface.SpecularPower) * Surface.Specular;
	float3 ResultDiffuse = Surface.Diffuse * DirectionalLight.Color * NdotL;
	
	return (ResultDiffuse + ResultSpecular) * DirectionalLight.Intensity;
}

float3 ZERNDeferredShading_ProjectiveLighting(ZERNDeferredShading_Light ProjectiveLight, ZERNDeferredShading_Surface Surface)
{
	float4 TextureLookup = mul(ZERNDeferredShading_LightProjectionMatrix, float4(Surface.PositionView, 1.0f));
	float2 SampleCoord = TextureLookup.xy / TextureLookup.w;
	if (SampleCoord.x < 0.0f || SampleCoord.x > 1.0f) return float3(0.0f, 0.0f, 0.0f);
	if (SampleCoord.y < 0.0f || SampleCoord.y > 1.0f) return float3(0.0f, 0.0f, 0.0f);
	float3 ProjLightColor = ProjectionMap.SampleLevel(ZERNDeferredShading_ProjectionSampler, SampleCoord, 0.0f).rgb;
	
	float3 LightPositionView = mul(ZERNView_ViewTransform, float4(ProjectiveLight.PositionWorld, 1.0f)).xyz;
	float3 LightDir = LightPositionView - Surface.PositionView;
	float LightDist = length(LightDir);
	LightDir = LightDir / LightDist;
	float NdotL = max(0, dot(Surface.NormalView, LightDir));

	float DistanceAttenuation = 1.0f / dot(ProjectiveLight.Attenuation, float3(1.0f, LightDist, LightDist * LightDist));
	
	float3 ViewDir = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDir + LightDir);
	float NdotH = max(0, dot(Surface.NormalView, HalfVector));
	
	float3 ResultSpecular = pow(NdotH, Surface.SpecularPower) * Surface.Specular;
	float3 ResultDiffuse = Surface.Diffuse * NdotL * ProjLightColor;
	
	return (ResultDiffuse + ResultSpecular) * ProjectiveLight.Intensity * DistanceAttenuation;
}

float3 ZERNDeferredShading_OmniProjectiveLighting(ZERNDeferredShading_Light OmniProjectiveLight, ZERNDeferredShading_Surface Surface)
{	
	float3 LightPositionView = mul(ZERNView_ViewTransform, float4(OmniProjectiveLight.PositionWorld, 1.0f)).xyz;
	float3 LightDir = LightPositionView - Surface.PositionView;
	float LightDist = length(LightDir);
	LightDir = LightDir / LightDist;
	float NdotL = max(0, dot(Surface.NormalView, LightDir));
	
	float3 TextureLookup = mul(ZERNDeferredShading_LightRotation, LightDir);
	float3 ProjLightColor = OmniProjectionMap.SampleLevel(ZERNDeferredShading_ProjectionSampler, TextureLookup, 0.0f).rgb;
	
	float3 ViewDir = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDir + LightDir);
	float NdotH = max(0, dot(Surface.NormalView, HalfVector));
	
	float DistanceAttenuation = 1.0f / dot(OmniProjectiveLight.Attenuation, float3(1.0f, LightDist, LightDist * LightDist));
	
	float3 ResultSpecular = pow(NdotH, Surface.SpecularPower) * Surface.Specular;
	float3 ResultDiffuse = Surface.Diffuse * OmniProjectiveLight.Color * ProjLightColor * NdotL;
	
	return (ResultDiffuse + ResultSpecular) * OmniProjectiveLight.Intensity * DistanceAttenuation;
}

float4 ZERNDeferredShading_Lighting(ZERNDeferredShading_Surface Surface)
{
	float3 ResultColor = {0.0f, 0.0f, 0.0f};
	
	if(ZERNDeferredShading_LightInstance.Type == ZE_LT_POINT)
	{
		ResultColor += ZERNDeferredShading_PointLighting(ZERNDeferredShading_LightInstance, Surface);
	}
	else if(ZERNDeferredShading_LightInstance.Type == ZE_LT_DIRECTIONAL)
	{
		ResultColor += ZERNDeferredShading_DirectionalLighting(ZERNDeferredShading_LightInstance, Surface);
	}
	else if(ZERNDeferredShading_LightInstance.Type == ZE_LT_PROJECTIVE)
	{
		ResultColor += ZERNDeferredShading_ProjectiveLighting(ZERNDeferredShading_LightInstance, Surface);
	}
	else if(ZERNDeferredShading_LightInstance.Type == ZE_LT_OMNIPROJECTIVE)
	{
		ResultColor += ZERNDeferredShading_OmniProjectiveLighting(ZERNDeferredShading_LightInstance, Surface);
	}
	
	return float4(saturate(ResultColor), 1.0f);
}

float3 ZERNDeferredShading_GetPositionViewFromDepth(float2 PositionScreen, float DepthView)
{
	float2 ScreenCoord = PositionScreen / float2(ZERNView_ProjectionTransform._11, ZERNView_ProjectionTransform._22);
	ScreenCoord = ScreenCoord * DepthView;
	
	return float3(ScreenCoord, DepthView);
}

// LIGHTING STAGE - PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float4 ZERNDeferredShading_PixelShader_LightingStage(ZERNDeferredShading_PSInput Input) : SV_Target0
{	
	float Depth = ZERNGBuffer_GetDepth(uint2(Input.PositionViewport.xy));
	float DepthView = ZERNView_ProjectionTransform._34 / (Depth - ZERNView_ProjectionTransform._33);
	
	float2 GBufferDimension;
	ZERNGBuffer_DepthBuffer.GetDimensions(GBufferDimension.x, GBufferDimension.y);
	
	float2 PositionScreen = (Input.PositionViewport.xy - 0.5f) * (float2(2.0f, -2.0f) / GBufferDimension) + float2(-1.0f, 1.0f);
	
	ZERNDeferredShading_Surface Surface;
	Surface.PositionView = ZERNDeferredShading_GetPositionViewFromDepth(PositionScreen, DepthView);
	Surface.Diffuse = ZERNGBuffer_GetDiffuseColor(int2(Input.PositionViewport.xy));
	Surface.Specular = ZERNGBuffer_GetSpecularColor(int2(Input.PositionViewport.xy));
	Surface.SpecularPower = ZERNGBuffer_GetSpecularPower(int2(Input.PositionViewport.xy));
	Surface.SpecularPower *= 64.0f;
	
	if(Surface.Diffuse.x != 0.0f)
	{	
		Surface.NormalView = mul(ZERNView_ViewTransform, float4(0.0f, 1.0f, 0.0f, 0.0f)).xyz;
		Surface.NormalView = normalize(Surface.NormalView);
		
		return ZERNDeferredShading_Lighting(Surface);
	}
	
	return float4(Surface.Diffuse, 1.0f);
}

#endif
