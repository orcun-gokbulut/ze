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

#pragma once


// SHADER RESOURCES
///////////////////////////////////////////////////////////////////////////////

Texture2D<float> ZERNGBuffer_DepthBuffer	: register(t0);
Texture2D<float4> ZERNGBuffer_Buffer0		: register(t1);
Texture2D<float4> ZERNGBuffer_Buffer1		: register(t2);
Texture2D<float4> ZERNGBuffer_Buffer2		: register(t3);
Texture2D<float4> ZERNGBuffer_Buffer3		: register(t4);

struct ZERNGBuffer
{
	float4 Buffer0 : COLOR0; // xy:Normal, z:Subsurface Scattering, w: ShadingModel
	float4 Buffer1 : COLOR1; // xyz:DiffuseColor, w: Unused
	float4 Buffer2 : COLOR2; // xyz:SpecularColor, w:SpacularPower
	float4 Buffer3 : COLOR3; // xyz:AccumulationBuffer, w:Normal Z Sign
};


// DEPTH
///////////////////////////////////////////////////////////////////////////////

float ZERNGBuffer_GetDepth(float4 ScreenPos)
{
	return ZERNGBuffer_DepthStencil.Load(ScreenPos);
}


// POSITION
///////////////////////////////////////////////////////////////////////////////

float3 ZERNGBuffer_GetViewPosition(float2 ClipPos, float4 ScreenPos)
{
	int3 ScreenCoord = int3(ScreenPos.xy, 0 );
    float ClipDepth = g_Depth.Load(ScreenCoord);
 	return float3(ClipPos, -1.0f) / (ClipDepth * ZERNView_Constants.InvProjectionTransform.z + ZERNView_Constants.InvProjectionTransform.w);
}


// SHADING MODEL
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetShadingModel(inout ZERNGBuffer GBuffer, uint Model)
{
	GBuffer.Buffer0.w = Model;
}

uint ZERNGBuffer_GetShadingModel(float4 ScreenPos)
{
	return ZERNGBuffer.Buffer0.Load(ScreenPos).w;
}


// NORMAL
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetViewNormal(inout ZERNGBuffer GBuffer, float3 Normal /*Must be Normalized*/)
{
	GBuffer.Buffer0.xy	= Normal.xy * 0.5f + 0.5f;
	GBuffer.Buffer3.a = sign(Normal.z) + 1.0f;
}

float3 ZERNGBuffer_GetViewNormal(float4 ScreenPos)
{
	float3 Normal;
	Normal.xy =	ZERNGBuffer_Buffer0.Sample(ZERNGBuffer_PointSampler, ScreenPos).xy * 2.0f - 1.0f;
	Normal.z = (ZERNGBuffer_Buffer3.Sample(ZERNGBuffer_PointSampler, ScreenPos).a - 1.0f) * sqrt(1.0f - dot(Normal.xy, Normal.xy);
	return Normal;
}


// SUB SURFACE SCATTERING
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetSubsurfaceScattering(inout ZERNGBuffer GBuffer, float SubsurfaceScattering /* Range: [0-1]*/)
{
	GBuffer.Buffer0.z = SubsurfaceScattering;
}

float ZERNGBuffer_GetSubsurfaceScattering(float4 ScreenPos)
{
	return ZERNGBuffer_Buffer0.Sample(ZERNGBuffer_PointSampler, ScreenPos).z;
}


// DIFFUSE COLOR
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetDiffuseColor(inout ZERNGBuffer GBuffer, float3 DiffuseColor)
{
	GBuffer.Buffer1.xyz = DiffuseColor;
}

float3 ZERNGBuffer_GetDiffuseColor(float4 ScreenPos)
{
	return ZERNGBuffer_Buffer1.Sample(ZERNGBuffer_PointSampler, ScreenPos).xyz;
}


/// SPECULAR COLOR
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetSpecularColor(inout ZERNGBuffer GBuffer, float3 SpecularColor)
{
	GBuffer.Buffer2.xyz = SpecularColor;
}

float3 ZERNGBuffer_GetSpecularColor(float4 ScreenPos)
{
	return ZERNGBuffer_Buffer2.Sample(ZERNGBuffer_PointSampler, ScreenPos).xyz;
}


// SPECULAR POWER
///////////////////////////////////////////////////////////////////////////////

void ZERNGBuffer_SetSpecularPower(inout ZERNGBuffer GBuffer, float SpecularPower)
{
	GBuffer.Buffer2.w = SpecularPower;
}

float ZERNGBuffer_SetSpecularPower(float4 ScreenPos)
{
	return ZERNGBuffer_Buffer2.Sample(ZERNGBuffer_PointSampler, ScreenPos).w;
}
