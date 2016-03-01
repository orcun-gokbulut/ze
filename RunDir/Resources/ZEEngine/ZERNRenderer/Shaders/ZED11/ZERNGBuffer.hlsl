//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNGBuffer.hlsl
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

#ifndef __ZERN_GBUFFER_H__
#define __ZERN_GBUFFER_H__

#include "ZERNView.hlsl"


// SHADER RESOURCES
///////////////////////////////////////////////////////////////////////////////
#define ZERN_LM_BLINN_PONG 0


// SHADER RESOURCES
///////////////////////////////////////////////////////////////////////////////

Texture2D<float>	ZERNGBuffer_DepthBuffer	: register(t0);
Texture2D<float3>	ZERNGBuffer_Buffer0		: register(t1);
Texture2D<float4>	ZERNGBuffer_Buffer1		: register(t2);
Texture2D<float4>	ZERNGBuffer_Buffer2		: register(t3);
Texture2D<float4>	ZERNGBuffer_Buffer3		: register(t4);

struct ZERNGBuffer
{
	float4			Buffer0					: SV_Target0; // xyz:AccumulationBuffer
	float4			Buffer1					: SV_Target1; // xyz:Normal, w: ShadingModel
	float4			Buffer2					: SV_Target2; // xyz:DiffuseColor, w:Subsurface Scattering
	float4			Buffer3					: SV_Target3; // xyz:SpecularColor, w:SpecularPower
};


// DIMENSIONS
///////////////////////////////////////////////////////////////////////////////

float2 ZERNGBuffer_GetDimensions()
{
	float2 Dimensions;
	ZERNGBuffer_DepthBuffer.GetDimensions(Dimensions.x, Dimensions.y);
	
	return Dimensions;
}

// DEPTH
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetDepth(inout ZERNGBuffer GBuffer, float Depth)
{
	// EMPTY BUT MUST BE CALLED BY GBUFFER STAGE SHADERS
	// FOR FUTURE POSSIBLE USAGES
}

float ZERNGBuffer_GetDepth(float2 ScreenPos)
{
	return ZERNGBuffer_DepthBuffer.Load(int3(ScreenPos, 0)).x;
}

float ZERNGBuffer_GetDepth(SamplerState Sampler, float2 TexCoord)
{
	return ZERNGBuffer_DepthBuffer.SampleLevel(Sampler, TexCoord, 0);
}

// POSITION
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetViewPosition(inout ZERNGBuffer GBuffer, float3 ViewPosition)
{
	// EMPTY BUT MUST BE CALLED BY GBUFFER STAGE SHADERS
	// FOR FUTURE POSSIBLE USAGES
}

float3 ZERNGBuffer_GetViewPosition(float2 ClipPos, int2 ScreenPos)
{
    float ClipDepth = ZERNGBuffer_GetDepth(ScreenPos);
 	return float3(ClipPos, -1.0f) / (ClipDepth * ZERNView_InvProjectionTransform[0][0] + ZERNView_InvProjectionTransform[0][0]);
}


// ACCUMULATION
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetAccumulationColor(inout ZERNGBuffer GBuffer, float3 Color)
{
	GBuffer.Buffer0.xyz = Color;
}

float3 ZERNGBuffer_GetAccumulationColor(float2 ScreenPos)
{
	return ZERNGBuffer_Buffer0.Load(int3(ScreenPos.xy, 0));
}

float3 ZERNGBuffer_GetAccumulationColor(SamplerState Sampler, float2 TexCoord)
{
	return ZERNGBuffer_Buffer0.SampleLevel(Sampler, TexCoord, 0);
}

// SHADING MODEL
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetShadingModel(inout ZERNGBuffer GBuffer, uint Model)
{
	GBuffer.Buffer1.w = Model;
}

uint ZERNGBuffer_GetShadingModel(float2 ScreenPos)
{
	return ZERNGBuffer_Buffer1.Load(int3(ScreenPos.xy, 0)).w * 255.0f + 0.5f;
}

uint ZERNGBuffer_GetShadingModel(SamplerState Sampler, float2 TexCoord)
{
	return ZERNGBuffer_Buffer1.SampleLevel(Sampler, TexCoord, 0).w * 255.0f + 0.5f;
}

// NORMAL
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetViewNormal(inout ZERNGBuffer GBuffer, float3 Normal /*Must be Normalized*/)
{
	GBuffer.Buffer1.xyz = Normal * 0.5f + 0.5f;
	/*GBuffer.Buffer0.xy = Normal.xy * 0.5f + 0.5f;
	GBuffer.Buffer3.a = sign(Normal.z) + 1.0f;*/
}

float3 ZERNGBuffer_GetViewNormal(float2 ScreenPos)
{
	/*float3 Normal;
	Normal.xy =	ZERNGBuffer_Buffer0.Load(int3(ScreenPos.xy, 0)).xy * 2.0f - 1.0f;
	Normal.z = (ZERNGBuffer_Buffer3.Load(int3(ScreenPos.xy, 0)).w - 1.0f) * sqrt(1.0f - dot(Normal.xy, Normal.xy));
	return Normal;*/
	return normalize(ZERNGBuffer_Buffer1.Load(int3(ScreenPos, 0)).xyz * 2.0f - 1.0f);
}

float3 ZERNGBuffer_GetViewNormal(SamplerState Sampler, float2 TexCoord)
{
	return ZERNGBuffer_Buffer1.SampleLevel(Sampler, TexCoord, 0).xyz * 2.0f - 1.0f;
}


// SUB SURFACE SCATTERING
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetSubsurfaceScattering(inout ZERNGBuffer GBuffer, float SubsurfaceScattering /* Range: [0-1]*/)
{
	GBuffer.Buffer2.w = SubsurfaceScattering;
}

float ZERNGBuffer_GetSubsurfaceScattering(float2 ScreenPos)
{
	return ZERNGBuffer_Buffer2.Load(int3(ScreenPos.xy, 0)).w;
}

float ZERNGBuffer_GetSubsurfaceScattering(SamplerState Sampler, float2 TexCoord)
{
	return ZERNGBuffer_Buffer2.SampleLevel(Sampler, TexCoord, 0).w;
}

// DIFFUSE COLOR
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetDiffuseColor(inout ZERNGBuffer GBuffer, float3 DiffuseColor)
{
	GBuffer.Buffer2.xyz = DiffuseColor;
}

float3 ZERNGBuffer_GetDiffuseColor(float2 ScreenPos)
{
	return ZERNGBuffer_Buffer2.Load(int3(ScreenPos.xy, 0)).xyz;
}

float3 ZERNGBuffer_GetDiffuseColor(SamplerState Sampler, float2 TexCoord)
{
	return ZERNGBuffer_Buffer2.SampleLevel(Sampler, TexCoord, 0).xyz;
}

/// SPECULAR COLOR
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetSpecularColor(inout ZERNGBuffer GBuffer, float3 SpecularColor)
{
	GBuffer.Buffer3.xyz = SpecularColor;
}

float3 ZERNGBuffer_GetSpecularColor(float2 ScreenPos)
{
	return ZERNGBuffer_Buffer3.Load(int3(ScreenPos.xy, 0)).xyz;
}

float3 ZERNGBuffer_GetSpecularColor(SamplerState Sampler, float2 TexCoord)
{
	return ZERNGBuffer_Buffer3.SampleLevel(Sampler, TexCoord, 0).xyz;
}

// SPECULAR POWER
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetSpecularPower(inout ZERNGBuffer GBuffer, float SpecularPower)
{
	GBuffer.Buffer3.w = SpecularPower;
}

float ZERNGBuffer_GetSpecularPower(float2 ScreenPos)
{
	return ZERNGBuffer_Buffer3.Load(int3(ScreenPos.xy, 0)).w * 64.0f;
}

float ZERNGBuffer_GetSpecularPower(SamplerState Sampler, float2 TexCoord)
{
	return ZERNGBuffer_Buffer3.SampleLevel(Sampler, TexCoord, 0).w * 64.0f;
}

#endif
