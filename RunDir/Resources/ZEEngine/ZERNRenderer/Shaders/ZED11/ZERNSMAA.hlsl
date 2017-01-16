//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSMAA.hlsl
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

#ifndef __ZERN_SMAA_H__
#define __ZERN_SMAA_H__

#include "ZERNGBuffer.hlsl"
#include "ZERNScreenCover.hlsl"
#include "ZERNShaderSlots.hlsl"
#include "ZERNTransformations.hlsl"

cbuffer ZERNSMAA_Constant_Buffer	: register(ZERN_SHADER_CONSTANT_STAGE)
{
	float2	ZERNSMAA_OutputSize;
	float2	ZERNSMAA_InvOutputSize;
	
	float4	ZERNSMAA_SubsampleIndices;
};

#define		SMAA_RT_METRICS float4(ZERNSMAA_InvOutputSize, ZERNSMAA_OutputSize)
#define		SMAA_HLSL_4
#define		SMAA_PRESET_ULTRA

#include "SMAA.hlsl"

Texture2D			ZERNSMAA_InputTexture		: register(t5);
Texture2D			ZERNSMAA_AreaTexture		: register(t6);
Texture2D			ZERNSMAA_SearchTexture		: register(t7);
Texture2D			ZERNSMAA_EdgeTexture		: register(t8);
Texture2D			ZERNSMAA_BlendTexture		: register(t9);
Texture2D			ZERNSMAA_CurrColorTexture	: register(t10);
Texture2D			ZERNSMAA_PrevColorTexture	: register(t11);
Texture2D			ZERNSMAA_VelocityTexture	: register(t12);
Texture2D<float>	ZERNSMAA_DepthTexture		: register(t13);

void ZERNSMAA_EdgeDetection_VertexShader(
	in uint VertexIndex : SV_VertexId, 
	out float4 Position : SV_POSITION, 
	out float2 Texcoord : TEXCOORD0, 
	out float4 Offset[3] : TEXCOORD1) 
{
	Position = ZERNScreenCover_GetPosition(VertexIndex);
	Texcoord = ZERNScreenCover_GetTexcoords(Position);
	
    SMAAEdgeDetectionVS(Texcoord, Offset);
}

void ZERNSMAA_BlendingWeightCalculation_VertexShader(
	in uint VertexIndex : SV_VertexId, 
	out float4 Position : SV_POSITION, 
	out float2 Texcoord : TEXCOORD0,
	out float2 Pixcoord : TEXCOORD1,
	out float4 Offset[3] : TEXCOORD2) 
{
	Position = ZERNScreenCover_GetPosition(VertexIndex);
	Texcoord = ZERNScreenCover_GetTexcoords(Position);
	
    SMAABlendingWeightCalculationVS(Texcoord, Pixcoord, Offset);
}

void ZERNSMAA_NeighborhoodBlending_VertexShader(
	in uint VertexIndex : SV_VertexId, 
	out float4 Position : SV_POSITION, 
	out float2 Texcoord : TEXCOORD0,
	out float4 Offset : TEXCOORD1) 
{
	Position = ZERNScreenCover_GetPosition(VertexIndex);
	Texcoord = ZERNScreenCover_GetTexcoords(Position);
	
    SMAANeighborhoodBlendingVS(Texcoord, Offset);
}

float2 ZERNSMAA_EdgeDetection_PixelShader(
	float4 Position : SV_POSITION,
    float2 Texcoord : TEXCOORD0,
    float4 Offset[3] : TEXCOORD1) : SV_TARGET 
{
	#if (ZERNSMAA_EDGE_DETECTION == 1)
			return SMAAColorEdgeDetectionPS(Texcoord, Offset, ZERNSMAA_InputTexture);
	#elif (ZERNSMAA_EDGE_DETECTION == 2)
		return SMAADepthEdgeDetectionPS(Texcoord, Offset, ZERNGBuffer_DepthBuffer);
	#else
		return SMAALumaEdgeDetectionPS(Texcoord, Offset, ZERNSMAA_InputTexture);
	#endif
}

float4 ZERNSMAA_BlendingWeightCalculation_PixelShader(
	float4 Position : SV_POSITION,
	float2 Texcoord : TEXCOORD0,
	float2 Pixcoord : TEXCOORD1,
	float4 Offset[3] : TEXCOORD2) : SV_TARGET
{
    return SMAABlendingWeightCalculationPS(Texcoord, Pixcoord, Offset, ZERNSMAA_EdgeTexture, ZERNSMAA_AreaTexture, ZERNSMAA_SearchTexture, ZERNSMAA_SubsampleIndices);
}

float4 ZERNSMAA_NeighborhoodBlending_PixelShader(
	float4 Position : SV_POSITION,
	float2 Texcoord : TEXCOORD0,
	float4 Offset : TEXCOORD1) : SV_TARGET
{
	return SMAANeighborhoodBlendingPS(Texcoord, Offset, ZERNSMAA_InputTexture, ZERNSMAA_BlendTexture 
	#if SMAA_REPROJECTION
	, ZERNSMAA_VelocityTexture
	#endif
	);
}

float2 ZERNSMAA_GenerateVelocityBuffer_PixelShader(float4 PositionViewport : SV_POSITION) : SV_TARGET0
{
	float3 PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, ZERNSMAA_OutputSize, ZERNSMAA_DepthTexture[floor(PositionViewport.xy)]);
	float3 PositionWorld = ZERNTransformations_ViewToWorld(float4(PositionView, 1.0f));
	
	float4 PrevPositionHomogeneous = ZERNTransformations_WorldToPrevProjection(float4(PositionWorld, 1.0f));
	PrevPositionHomogeneous.xy /= PrevPositionHomogeneous.w;
	float2 CurrPositionHomogeneous = ZERNTransformations_ViewportToHomogeneous(PositionViewport.xy, ZERNSMAA_OutputSize);
	
	float2 Velocity = (CurrPositionHomogeneous - PrevPositionHomogeneous.xy) * float2(0.5f, -0.5f);
	
	return (length(Velocity) > length(ZERNSMAA_InvOutputSize)) ? ZERNSMAA_InvOutputSize : Velocity;
}

float4 ZERNSMAA_Reprojection_PixelShader(float4 Position : SV_POSITION) : SV_TARGET0
{
	return SMAAResolvePS(Position.xy * ZERNSMAA_InvOutputSize, ZERNSMAA_CurrColorTexture, ZERNSMAA_PrevColorTexture
	#if SMAA_REPROJECTION
	, ZERNSMAA_VelocityTexture
	#endif
	);
}

#endif
