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

// Samplers
SamplerState	GBufferPointSampler		: register(s0);

//Textures
Texture2D<float>	GBuffer1		: register(t0);
Texture2D<float4>	GBuffer2		: register(t1);
Texture2D<float4>	GBuffer3		: register(t2);

struct ZEGBuffer
{
	float Position								: SV_TARGET0;
	float4 NormalGloss							: SV_TARGET1;
	float4 VelocitySubSurfaceScatteringFactor	: SV_TARGET2;
};

// Depth
float ZEGBuffer_GetDepth(in float2 TexCoord)
{
	return GBuffer1.Sample(GBufferPointSampler, TexCoord);
}

// View Vector
float3 ZEGBuffer_GetViewVector(in float4 ClipPosition)
{
	return ClipPosition.xyz;
}

// View Position
void ZEGBuffer_SetViewPosition(inout ZEGBuffer Output, in float3 ViewPosition)
{
	Output.Position = length(ViewPosition);
}

float3 ZEGBuffer_GetViewPosition(in float2 Texcoord, in float3 ViewVector)
{
	float Depth = GBuffer1.Sample(GBufferPointSampler, Texcoord);
	return normalize(ViewVector) * Depth;
}

// View Normal
void ZEGBuffer_SetViewNormal(inout ZEGBuffer Output, in float3 ViewNormal)
{
	Output.NormalGloss.xyz = normalize(ViewNormal.xyz) * 0.5f + 0.5f;
}

float3 ZEGBuffer_GetViewNormal(in float2 Texcoord)
{
	return normalize(GBuffer2.Sample(GBufferPointSampler, Texcoord).xyz * 2.0f - 1.0f);
}

// Specular Glossiness
void ZEGBuffer_SetSpecularGlossiness(inout ZEGBuffer Output, in float SpecularGlossiness)
{
	Output.NormalGloss.w = SpecularGlossiness;
}

float ZEGBuffer_GetSpecularGlossiness(in float2 Texcoord)
{
	return GBuffer2.Sample(GBufferPointSampler, Texcoord).w;
}

float ZEGBuffer_GetSpecularPower(in float2 Texcoord)
{
	return 128.0f - ZEGBuffer_GetSpecularGlossiness(Texcoord) * 120.0f;
}

// Velocity
void ZEGBuffer_SetScreenVelocity(inout ZEGBuffer Output, in float2 Velocity)
{
	Output.VelocitySubSurfaceScatteringFactor.xy = Velocity;
}

float2 ZEGBuffer_GetScreenVelocity(in float2 Texcoord)
{
	return GBuffer3.Sample(GBufferPointSampler, Texcoord).xy;
}

// Sub Surface Scattering
void ZEGBuffer_SetSubSurfaceScatteringFactor(inout ZEGBuffer Output, in float Factor)
{
	Output.VelocitySubSurfaceScatteringFactor.z = Factor;
}

float ZEGBuffer_GetSubSurfaceScatteringFactor(in float2 Texcoord)
{
	return GBuffer3.Sample(GBufferPointSampler, Texcoord).z;
}
