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

// SHADER RESOURCES
///////////////////////////////////////////////////////////////////////////////

#if SAMPLE_COUNT > 1
	#define MSAA_ENABLED
#endif

#ifdef MSAA_ENABLED
	Texture2DMS<float4, SAMPLE_COUNT>	ZERNGBuffer_Buffer0		: register(t0);
	Texture2DMS<float4, SAMPLE_COUNT>	ZERNGBuffer_Buffer1		: register(t1);
	Texture2DMS<float4, SAMPLE_COUNT>	ZERNGBuffer_Buffer2		: register(t2);
	Texture2DMS<float4, SAMPLE_COUNT>	ZERNGBuffer_Buffer3		: register(t3);
	Texture2DMS<float4, SAMPLE_COUNT>	ZERNGBuffer_DepthBuffer	: register(t4);
#else
	Texture2D<float3>					ZERNGBuffer_Buffer0		: register(t0);
	Texture2D<float4>					ZERNGBuffer_Buffer1		: register(t1);
	Texture2D<float4>					ZERNGBuffer_Buffer2		: register(t2);
	Texture2D<float4>					ZERNGBuffer_Buffer3		: register(t3);
	Texture2D<float>					ZERNGBuffer_DepthBuffer	: register(t4);
#endif

struct ZERNGBuffer
{
	float4								Buffer0					: SV_Target0; // xyz:AccumulationBuffer
	float4								Buffer1					: SV_Target1; // xy:Velocity(optional) z:Unused, w:SpecularPower
	float4								Buffer2					: SV_Target2; // xyz:DiffuseColor, w:Subsurface Scattering 
	float4								Buffer3					: SV_Target3; // xyz:Normal, w:SpecularColor
};


// DIMENSIONS
///////////////////////////////////////////////////////////////////////////////

float2 ZERNGBuffer_GetDimensions()
{
	float2 Dimensions;
	
	#ifdef MSAA_ENABLED
		float SampleCount;
		ZERNGBuffer_DepthBuffer.GetDimensions(Dimensions.x, Dimensions.y, SampleCount);
	#else
		ZERNGBuffer_DepthBuffer.GetDimensions(Dimensions.x, Dimensions.y);
	#endif
	
	return Dimensions;
}

// DEPTH
///////////////////////////////////////////////////////////////////////////////

float ZERNGBuffer_GetDepth(float2 ScreenPos, float SampleIndex = 0)
{
	#ifdef MSAA_ENABLED
		return ZERNGBuffer_DepthBuffer.Load(ScreenPos, SampleIndex).x;
	#else
		return ZERNGBuffer_DepthBuffer.Load(int3(ScreenPos, 0)).x;
	#endif
}

// ACCUMULATION
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetAccumulationColor(inout ZERNGBuffer GBuffer, float3 Color)
{
	GBuffer.Buffer0.xyz = Color;
}

float3 ZERNGBuffer_GetAccumulationColor(float2 ScreenPos, float SampleIndex = 0)
{
	#ifdef MSAA_ENABLED
		return ZERNGBuffer_Buffer0.Load(ScreenPos, SampleIndex).xyz;
	#else
		return ZERNGBuffer_Buffer0.Load(int3(ScreenPos, 0)).xyz;
	#endif
}

// VELOCITY
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetVelocity(inout ZERNGBuffer GBuffer, float2 Velocity)
{
	GBuffer.Buffer1.xy = Velocity;
}

float2 ZERNGBuffer_GetVelocity(float2 ScreenPos, float SampleIndex = 0)
{
	#ifdef MSAA_ENABLED
		return ZERNGBuffer_Buffer1.Load(ScreenPos, SampleIndex).xy;
	#else
		return ZERNGBuffer_Buffer1.Load(int3(ScreenPos, 0)).xy;
	#endif
}

// SPECULAR POWER
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetSpecularPower(inout ZERNGBuffer GBuffer, float SpecularPower)
{
	GBuffer.Buffer1.w = SpecularPower;
}

float ZERNGBuffer_GetSpecularPower(float2 ScreenPos, float SampleIndex = 0)
{
	#ifdef MSAA_ENABLED
		return ZERNGBuffer_Buffer1.Load(ScreenPos, SampleIndex).w * 64.0f;
	#else
		return ZERNGBuffer_Buffer1.Load(int3(ScreenPos, 0)).w * 64.0f;
	#endif
}

// DIFFUSE COLOR
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetDiffuseColor(inout ZERNGBuffer GBuffer, float3 DiffuseColor)
{
	GBuffer.Buffer2.xyz = DiffuseColor;
}

float3 ZERNGBuffer_GetDiffuseColor(float2 ScreenPos, float SampleIndex = 0)
{
	#ifdef MSAA_ENABLED
		return ZERNGBuffer_Buffer2.Load(ScreenPos, SampleIndex).xyz;
	#else
		return ZERNGBuffer_Buffer2.Load(int3(ScreenPos, 0)).xyz;
	#endif
}

// SUB SURFACE SCATTERING
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetSubsurfaceScattering(inout ZERNGBuffer GBuffer, float SubsurfaceScattering /* Range: [0-1]*/)
{
	GBuffer.Buffer2.w = SubsurfaceScattering;
}

float ZERNGBuffer_GetSubsurfaceScattering(float2 ScreenPos, float SampleIndex = 0)
{
	#ifdef MSAA_ENABLED
		return ZERNGBuffer_Buffer2.Load(ScreenPos, SampleIndex).w;
	#else
		return ZERNGBuffer_Buffer2.Load(int3(ScreenPos, 0)).w;
	#endif
}

// NORMAL
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetViewNormal(inout ZERNGBuffer GBuffer, float3 Normal /*Must be Normalized*/)
{
	GBuffer.Buffer3.xyz = Normal * 0.5f + 0.5f;
}

float3 ZERNGBuffer_GetViewNormal(float2 ScreenPos, float SampleIndex = 0)
{
	#ifdef MSAA_ENABLED
		return normalize(ZERNGBuffer_Buffer3.Load(ScreenPos, SampleIndex).xyz * 2.0f - 1.0f);
	#else
		return normalize(ZERNGBuffer_Buffer3.Load(int3(ScreenPos, 0)).xyz * 2.0f - 1.0f);
	#endif
}

// SPECULAR COLOR
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetSpecularColor(inout ZERNGBuffer GBuffer, float3 SpecularColor)
{
	GBuffer.Buffer3.w = SpecularColor.x;
}

float3 ZERNGBuffer_GetSpecularColor(float2 ScreenPos, float SampleIndex = 0)
{
	#ifdef MSAA_ENABLED
		return ZERNGBuffer_Buffer3.Load(ScreenPos, SampleIndex).www;
	#else
		return ZERNGBuffer_Buffer3.Load(int3(ScreenPos, 0)).www;
	#endif
}

#endif
