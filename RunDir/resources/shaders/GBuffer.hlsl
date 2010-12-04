//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GBuffer.hlsl
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

sampler2D GBuffer1 : register(s0);
sampler2D GBuffer2 : register(s1);

float3 ZEGBuffer_ViewVector : register(vs, c0);
float4 ZEGBuffer_PipelineParameters0 : register(ps, c6);
#define ZEGBuffer_PixelSize_2				ZEGBuffer_PipelineParameters0.xy
#define ZEGBuffer_FarZ						ZEGBuffer_PipelineParameters0.z

struct ZEGBuffer
{
	float4 Position : COLOR0;
	float4 NormalGloss : COLOR1;
};

// View Vector
float3 ZEGBuffer_GetViewVector(float4 ClipPosition)
{
	return float3((ClipPosition.xy / ClipPosition.w) * ZEGBuffer_ViewVector.xy, ZEGBuffer_ViewVector.z);
}

// View Position
void ZEGBuffer_SetViewPosition(inout ZEGBuffer Output, float3 ViewPosition)
{
	Output.Position.x = length(ViewPosition.z);
	Output.Position.yzw = ViewPosition;
}

float3 ZEGBuffer_GetViewPosition(float2 Texcoord, float3 ViewVector)
{
	//return ;
	float3 ViewPosition = tex2D(GBuffer1, Texcoord).yzw;
	float3 Position = ViewVector * tex2D(GBuffer1, Texcoord).x;
	return Position;
}

// View Normal
void ZEGBuffer_SetViewNormal(inout ZEGBuffer Output, float3 ViewNormal)
{
	Output.NormalGloss.xyz = ViewNormal * 0.5f + 0.5f;
}

float3 ZEGBuffer_GetViewNormal(float2 Texcoord)
{
	return 2.0f * tex2D(GBuffer2, Texcoord).xyz - 1.0f;
}

// Specular Glossiness
void ZEGBuffer_SetSpecularGlossiness(inout ZEGBuffer Output, float SpecularGlossiness)
{
	Output.NormalGloss.w = SpecularGlossiness;
}

float ZEGBuffer_GetSpecularGlossiness(float2 Texcoord)
{
	return tex2D(GBuffer2, Texcoord).w;
}

float ZEGBuffer_GetSpecularPower(float2 Texcoord)
{
	return 128.0f - ZEGBuffer_GetSpecularGlossiness(Texcoord) * 120.0f;
}

// Velocity
float2 ZEGBuffer_GetScreenVelocity(float2 Texcoord)
{
	return float2(0.0f, 0.0f);
}
