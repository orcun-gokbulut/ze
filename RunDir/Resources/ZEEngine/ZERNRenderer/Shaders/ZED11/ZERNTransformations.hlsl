//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNTransformations.hlsl
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

#ifndef __ZERN_TRANSFORMATIONS_HLSL__
#define __ZERN_TRANSFORMATIONS_HLSL__

#include "ZERNView.hlsl"

//FROM HOMOGENEOUS TRANSFORMATIONS
///////////////////////////////////////////////////////////////////////////

float2 ZERNTransformations_HomogeneousToTexelCorner(float2 VectorHomogeneous)
{
	return VectorHomogeneous * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
}

float2 ZERNTransformations_HomogeneousToTexelCenter(float2 VectorHomogeneous, float2 Dimensions)
{
	float2 TexelCorner = ZERNTransformations_HomogeneousToTexelCorner(VectorHomogeneous);
	
	return (TexelCorner * (Dimensions - 1.0f) + 0.5f) / Dimensions;
}

float ZERNTransformations_HomogeneousToViewDepth(float DepthHomogeneous)
{
	return ZERNView_ProjectionTransform._34 / (DepthHomogeneous - ZERNView_ProjectionTransform._33);
}

float2 ZERNTransformations_HomogeneousToView(float2 VectorHomogeneous)
{
	return VectorHomogeneous / float2(ZERNView_ProjectionTransform._11, ZERNView_ProjectionTransform._22);
}

float3 ZERNTransformations_HomogeneousToView(float2 VectorHomogeneous, float DepthHomogeneous)
{
	float DepthView = ZERNTransformations_HomogeneousToViewDepth(DepthHomogeneous);
	VectorHomogeneous *= DepthView;
	VectorHomogeneous -= DepthView * float2(ZERNView_ProjectionTransform._13, ZERNView_ProjectionTransform._23);
	float2 VectorView = ZERNTransformations_HomogeneousToView(VectorHomogeneous);
	
	return float3(VectorView, DepthView);
}

//FROM PROJECTION TRANSFORMATIONS
///////////////////////////////////////////////////////////////////////////

float3 ZERNTransformations_ProjectionToHomogeneous(float4 VectorProjection)
{
	return VectorProjection.xyz / VectorProjection.w;
}

float2 ZERNTransformations_ProjectionToTexelCorner(float4 VectorProjection)
{
	float3 PositionHomogeneous = ZERNTransformations_ProjectionToHomogeneous(VectorProjection);
	
	return ZERNTransformations_HomogeneousToTexelCorner(PositionHomogeneous.xy);
}

float2 ZERNTransformations_ProjectionToTexelCenter(float4 VectorProjection, float2 Dimensions)
{
	float3 PositionHomogeneous = ZERNTransformations_ProjectionToHomogeneous(VectorProjection);
	
	return ZERNTransformations_HomogeneousToTexelCenter(PositionHomogeneous.xy, Dimensions);
}

//FROM TEXEL TRANSFORMATIONS
///////////////////////////////////////////////////////////////////////////

float2 ZERNTransformations_TexelToHomogeneous(float2 TexCoord)
{
	return TexCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
}

float3 ZERNTransformations_TexelToView(float2 TexCoord, float DepthHomogeneous)
{
	float2 VectorHomogeneous = ZERNTransformations_TexelToHomogeneous(TexCoord);
	
	return ZERNTransformations_HomogeneousToView(VectorHomogeneous, DepthHomogeneous);
}

float3 ZERNTransformations_TexelToView2(float2 TexCoord, float DepthView)
{
	float2 VectorHomogeneous = ZERNTransformations_TexelToHomogeneous(TexCoord);
	VectorHomogeneous *= DepthView;
	VectorHomogeneous -= DepthView * float2(ZERNView_ProjectionTransform._13, ZERNView_ProjectionTransform._23);
	float2 VectorView = ZERNTransformations_HomogeneousToView(VectorHomogeneous);
	
	return float3(VectorView, DepthView);
}

float2 ZERNTransformations_TexelToViewport(float2 TexCoord, float2 ViewportDimensions)
{
	return TexCoord * ViewportDimensions;
}

//FROM VIEWPORT TRANSFORMATIONS
///////////////////////////////////////////////////////////////////////////

float2 ZERNTransformations_ViewportToTexelCorner(float2 VectorViewport, float2 ViewportDimensions)
{
	return (VectorViewport - 0.5f) / ViewportDimensions;
}

float2 ZERNTransformations_ViewportToTexelCenter(float2 VectorViewport, float2 ViewportDimensions)
{
	return VectorViewport / ViewportDimensions;
}

float2 ZERNTransformations_ViewportToHomogeneous(float2 VectorViewport, float2 ViewportDimensions)
{
	float2 TexelCenter = ZERNTransformations_ViewportToTexelCenter(VectorViewport, ViewportDimensions);
	return ZERNTransformations_TexelToHomogeneous(TexelCenter);
}

float3 ZERNTransformations_ViewportToView(float2 VectorViewport, float2 ViewportDimensions, float DepthHomogeneous)
{
	float2 VectorHomogeneous = ZERNTransformations_ViewportToHomogeneous(VectorViewport, ViewportDimensions);
	
	return ZERNTransformations_HomogeneousToView(VectorHomogeneous, DepthHomogeneous);
}

float3 ZERNTransformations_ViewportToView2(float2 VectorViewport, float2 ViewportDimensions, float DepthView)
{
	float2 VectorHomogeneous = ZERNTransformations_ViewportToHomogeneous(VectorViewport, ViewportDimensions);
	VectorHomogeneous *= DepthView;
	VectorHomogeneous -= DepthView * float2(ZERNView_ProjectionTransform._13, ZERNView_ProjectionTransform._23);
	float2 VectorView = ZERNTransformations_HomogeneousToView(VectorHomogeneous);
	
	return float3(VectorView, DepthView);
}

float2 ZERNTransformations_ViewportToView(float2 VectorViewport, float2 ViewportDimensions)
{
	float2 VectorHomogeneous = ZERNTransformations_ViewportToHomogeneous(VectorViewport, ViewportDimensions);
	
	return ZERNTransformations_HomogeneousToView(VectorHomogeneous);
}

//FROM VIEW TRANSFORMATIONS
///////////////////////////////////////////////////////////////////////////

float3 ZERNTransformations_ViewToHomogeneous(float3 VectorView)
{
	float4 SampleVectorHomogeneous = mul(ZERNView_ProjectionTransform, float4(VectorView, 1.0f));
	
	return SampleVectorHomogeneous.xyz / SampleVectorHomogeneous.w;
}

float2 ZERNTransformations_ViewToTexelCorner(float3 VectorView)
{
	float3 VectorHomogeneous = ZERNTransformations_ViewToHomogeneous(VectorView);
	
	return ZERNTransformations_HomogeneousToTexelCorner(VectorHomogeneous.xy);
}

float2 ZERNTransformations_ViewToTexelCenter(float3 VectorView, float2 Dimensions)
{
	float3 VectorHomogeneous = ZERNTransformations_ViewToHomogeneous(VectorView);
	
	return ZERNTransformations_HomogeneousToTexelCenter(VectorHomogeneous.xy, Dimensions);
}

float2 ZERNTransformations_ViewToViewport(float3 VectorView, float2 ViewportDimensions)
{
	float2 TexelCorner = ZERNTransformations_ViewToTexelCorner(VectorView);
	
	return ZERNTransformations_TexelToViewport(TexelCorner, ViewportDimensions);
}

float4 ZERNTransformations_ViewToProjection(float3 VectorView)
{
	return mul(ZERNView_ProjectionTransform, float4(VectorView, 1.0f));
}

float3 ZERNTransformations_ViewToWorld(float4 VectorView)
{
	return mul(ZERNView_InvViewTransform, VectorView).xyz;
}

//FROM WORLD TRANSFORMATIONS
///////////////////////////////////////////////////////////////////////////

float3 ZERNTransformations_WorldToView(float4 VectorWorld)
{
	return mul(ZERNView_ViewTransform, VectorWorld).xyz;
}

float4 ZERNTransformations_WorldToProjection(float4 VectorWorld)
{
	return mul(ZERNView_ViewProjectionTransform, VectorWorld);
}

float3 ZERNTransformations_WorldToHomogeneous(float4 VectorWorld)
{
	float4 VectorProjection = ZERNTransformations_WorldToProjection(VectorWorld);
	return VectorProjection.xyz / VectorProjection.w;
}

float2 ZERNTransformations_WorldToTexelCorner(float4 VectorWorld)
{
	float3 VectorHomogeneous = ZERNTransformations_WorldToHomogeneous(VectorWorld);
	return ZERNTransformations_HomogeneousToTexelCorner(VectorHomogeneous.xy);
}

float2 ZERNTransformations_WorldToTexelCenter(float4 VectorWorld, float2 TextureDimensions)
{
	float3 VectorHomogeneous = ZERNTransformations_WorldToHomogeneous(VectorWorld);
	return ZERNTransformations_HomogeneousToTexelCenter(VectorHomogeneous.xy, TextureDimensions);
}

float2 ZERNTransformations_WorldToViewport(float4 VectorWorld, float2 ViewportDimensions)
{
	float2 TexCoord = ZERNTransformations_WorldToTexelCorner(VectorWorld);
	return ZERNTransformations_TexelToViewport(TexCoord, ViewportDimensions);
}

#endif
