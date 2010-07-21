//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SSOAProcessor.hlsl
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

sampler2D DepthBuffer : register(s0);
sampler2D NormalBuffer : register(s1);
sampler2D RandomNormalBuffer : register(s2);

float4 Parameters0 : register(c0);
float2 PixelSize : register(c1);
float3 ViewVector_ : register(c2);

const float2 SampleKernel[4] : register(c5);

#define Intensity		Parameters0[0]
#define SampleRadius	Parameters0[1]
#define SampleScale		Parameters0[2]
#define SampleBias		Parameters0[3]

#include "GBuffer.hlsl"

struct VSInput
{
	float4 Position			: POSITION;
	float2 Texcoord			: TEXCOORD0;
};

struct VSOutput
{
   float4 Position			: POSITION0;
   float2 ScreenTexcoord	: TEXCOORD0;
   float2 RandomTexcoord	: TEXCOORD1;
   float3 ViewVector		: TEXCOORD2;
};

struct PSInput
{
   float2 Texcoord			: TEXCOORD0;
   float2 RandomTexcoord	: TEXCOORD1; 
   float3 ViewVector		: TEXCOORD2;
};

struct PSOutput
{
   float4 Color : COLOR0;
};

float2 GetRandomVector(in float2 Texcoord)
{
	return normalize(tex2D(RandomNormalBuffer, Texcoord).xy * 2.0f - 1.0f);
}

float AmbientOcclusion(in float2 Texcoord, in float3 PixelPosition, in float3 PixelNormal, in float3 ViewVector)
{
	//float3 Diff = GetViewPosition(DepthBuffer, Texcoord, ViewVector) - PixelPosition;
	float3 Diff = tex2D(DepthBuffer, Texcoord).yzw - PixelPosition;
	const float3 v = normalize(Diff);
	const float Dist = length(Diff) * SampleScale;
	return max(0.0f, dot(PixelNormal, v) - SampleBias) * (1.0f / (1.0f + Dist)) * Intensity;
}

VSOutput VSMain(VSInput Input)
{
	VSOutput Output;

	Output.Position = float4(Input.Position.xy, 0.0f, 1.0f);
	Output.ScreenTexcoord = Input.Texcoord + PixelSize * 0.5f;
	Output.RandomTexcoord = Output.ScreenTexcoord * (1.0f / PixelSize) / 64.0f;
	
	Output.ViewVector.xy = Input.Position.xy * ViewVector_.xy;
	Output.ViewVector.z = ViewVector_.z;

	return Output;
}

PSOutput PSMain(PSInput Input)
{   
	PSOutput Output;
	
	float3 PixelPosition = tex2D(DepthBuffer, Input.Texcoord).yzw;//GetViewPosition(DepthBuffer, Input.Texcoord, Input.ViewVector);
	float3 PixelNormal = GetViewNormal(NormalBuffer, Input.Texcoord);
	

	float2 RandomNormal = GetRandomVector(Input.RandomTexcoord);

	float Occlusion = 0.0f;
	float Radius = SampleRadius / PixelPosition.z;

	int IterationCount = 4;
	for (int I = 0; I < IterationCount; I++)
	{
		float2 SampleTexcoord0 = reflect(SampleKernel[I], RandomNormal) * Radius;
		float2 SampleTexcoord1 = float2(SampleTexcoord0.x - SampleTexcoord0.y, SampleTexcoord0.x + SampleTexcoord0.y) * 0.707f;

		Occlusion += AmbientOcclusion(Input.Texcoord + SampleTexcoord0 * 0.25f, PixelPosition, PixelNormal, Input.ViewVector);
		Occlusion += AmbientOcclusion(Input.Texcoord + SampleTexcoord0 * 0.5f, PixelPosition, PixelNormal, Input.ViewVector);
		Occlusion += AmbientOcclusion(Input.Texcoord + SampleTexcoord0 * 0.75f, PixelPosition, PixelNormal, Input.ViewVector);
		Occlusion += AmbientOcclusion(Input.Texcoord + SampleTexcoord0, PixelPosition, PixelNormal, Input.ViewVector);
	} 
	
	Occlusion /= (float)IterationCount * 4.0;
	Output.Color = (1.0f - Occlusion);
	//Output.Color = float4(abs(PixelPosition - GetViewPosition(DepthBuffer, Input.Texcoord, Input.ViewVector)) * 10, 0.0f);
	return Output;
}
