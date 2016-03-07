//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFastLightScattering.hlsl
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

#ifndef __ZERN_FASTLIGHTSCATTERING_HLSL__
#define __ZERN_FASTLIGHTSCATTERING_HLSL__

#include "ZERNLightScatteringCommon.hlsl"
#include "ZERNScreenCover.hlsl"
#include "ZERNGBuffer.hlsl"
#include "ZERNTransformations.hlsl"
#include "ZERNIntersections.hlsl"

cbuffer ZERNFastLightScattering_Constants								: register(b8)
{
	float3			ZERNFastLightScattering_LightDirection;
	float			ZERNFastLightScattering_Intensity;
	float3			ZERNFastLightScattering_LightColor;
	float			ZERNFastLightScattering_Reserved;
};

Texture3D<float3>	ZERNFastLightScattering_ScatteringBuffer			: register(t5);

float3 ZERNFastLightScattering_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float3 EarthCenter = float3(0.0f, -EARTH_RADIUS, 0.0f);
	
	float DepthHomogeneous = ZERNGBuffer_GetDepth(PositionViewport.xy);
	float DepthView = ZERNTransformations_HomogeneousToViewDepth(DepthHomogeneous);
	
	float2 TextureDimensions = ZERNGBuffer_GetDimensions();
	
	//float2 PixelPositionViewNormalized = ZERNTransformations_ViewportToView(PositionViewport.xy, TextureDimensions);
	//float3 ViewDirection = ZERNTransformations_ViewToWorld(float4(PixelPositionViewNormalized, DepthView, 0.0f));
	//ViewDirection = normalize(ViewDirection);
	
	float3 PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, TextureDimensions, DepthHomogeneous);
	float3 PositionWorld = ZERNTransformations_ViewToWorld(float4(PositionView, 1.0f));
	float3 ViewDirection = normalize(PositionWorld - ZERNView_Position);
	
	float3 LightDirectionWorld = -normalize(ZERNFastLightScattering_LightDirection);
	
	float4 StartEndDistance = (float4)0.0f;
	ZERNIntersections_RaySphere2(ZERNView_Position, ViewDirection, EarthCenter, float2(TOTAL_RADIUS, EARTH_RADIUS), StartEndDistance);
	
	float3 RayStart = ZERNView_Position + ViewDirection * max(0.0f, StartEndDistance.x);
		
	float RayLength = StartEndDistance.y;
	
	if(StartEndDistance.x < 0.0f && DepthHomogeneous != 0.0f)	//in earth
		RayLength = DepthView;
	else if(StartEndDistance.x > 0.0f && StartEndDistance.z > 0.0f)	//in space
		RayLength = StartEndDistance.z;
	
	float3 RayEnd = ZERNView_Position + ViewDirection * RayLength;
	
	float3 Extinction = exp(-ZERNLightScatteringCommon_CalculateExtinction(RayStart, RayEnd));
	
	float PrevTexCoordY = -1.0f;
	float3 Inscattering = ZERNLightScatteringCommon_LookupPrecomputedScattering(ZERNFastLightScattering_ScatteringBuffer, RayStart, ViewDirection, LightDirectionWorld, EarthCenter, PrevTexCoordY);
	Inscattering -= Extinction * ZERNLightScatteringCommon_LookupPrecomputedScattering(ZERNFastLightScattering_ScatteringBuffer, RayEnd, ViewDirection, LightDirectionWorld, EarthCenter, PrevTexCoordY);
	Inscattering *= ZERNFastLightScattering_Intensity;
	
	float3 PixelColor = ZERNGBuffer_GetAccumulationColor(PositionViewport.xy);
	
	return PixelColor * Extinction + Inscattering;
}

#endif
