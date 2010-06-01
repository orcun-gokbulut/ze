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

// Parameters
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 Parameters0			: register(c0);
#define PixelSize			(Parameters0.xy)
#define FarZ				(Parameters0.z)
#define FOV					(Parameters0.w)

float4 Parameters1			: register(c1);
#define SSAOFactor			(Parameters1[0])
#define Radius				(Parameters1[1])
#define Attenuation			(Parameters1[2])


// Textures
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
sampler ColorInput            : register(s0);
sampler DepthInput            : register(s1);
sampler RandomTexture         : register(s2);


// Kernels
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float2 DiscKernel[12]	: register(c10);
/*{
	{ 0.6487,      -0.111764669,   0.0f, 0.0f},
	{ 0.0923,       0.586844921,   0.0f, 0.0f},
	{ 0.4026,      -0.185264587,   0.0f, 0.0f},
	{-0.5019,       0.563529253,   0.0f, 0.0f},
	{-0.6044,      -0.310119152,   0.0f, 0.0f},
	{-0.1893,       0.311908484,   0.0f, 0.0f},
	{ 0.5923,      -0.726073265,   0.0f, 0.0f},
	{ 0.035603881,   -0.793983221,   0.0f, 0.0f},
	{-0.296554923,   -0.939840794,   0.0f, 0.0f},
	{-0.181980252,    0.935429335,   0.0f, 0.0f},
	{-0.214773059,   -0.49524498,   0.0f, 0.0f},
	{ 0.895886064,    0.058031559,   0.0f, 0.0f},
};*/

// SSAO Vertex Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct VS_INPUT
{
	float4 Position			: POSITION0;
	float2 Texcoord			: TEXCOORD0;
};

struct VS_OUTPUT
{
   float4 Position			: POSITION0;
   float2 DepthTexcoord		: TEXCOORD0;
   float2 RandomTexcoord	: TEXCOORD1;
   float3 ViewVector		: TEXCOORD2;
};

VS_OUTPUT vs_main(VS_INPUT Input)
{
	VS_OUTPUT Output;

	Output.Position = Input.Position + float4(-PixelSize.x, PixelSize.y, 0.0f, 0.0f);
	Output.DepthTexcoord = Input.Texcoord;
	
	float2 ScreenSize = 1.0f / PixelSize;
	
	Output.RandomTexcoord = Output.DepthTexcoord * (ScreenSize / 64.0f);

	Output.ViewVector.x = ((Output.DepthTexcoord.x * 2.0f) - 1.0f) * tan(FOV / 2.0f) * (ScreenSize.x / ScreenSize.y);	
	Output.ViewVector.y = ((Output.DepthTexcoord.y * 2.0f) - 1.0f) * tan(FOV / 2.0f);
	Output.ViewVector.z = 1.0f;
	Output.ViewVector = normalize(Output.ViewVector);

	return Output;
}


// SSAO Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct PS_INPUT
{
   float2 DepthTexcoord		: TEXCOORD0;
   float2 RandomTexcoord	: TEXCOORD1;
   float3 ViewVector		: TEXCOORD2;
};

float4 ps_main(PS_INPUT Input) : COLOR0
{
	float CurrentDepth = tex2D(DepthInput, Input.DepthTexcoord).r;
	float3 CurrentPosition = (CurrentDepth * Input.ViewVector);

	float AmbientOcclusion = 1.0f;
	for (int I = 0; I < 9; I++)
	{	/*
		float2 Rnd = (tex2D(RandomTexture, Input.RandomTexcoord) * 2.0f) - 1.0f;
		float2 SamplePosition;
		SamplePosition.x = Rnd.x * DiscKernel[I].x - Rnd.y * DiscKernel[I].y;
		SamplePosition.y = Rnd.y * DiscKernel[I].x + Rnd.x * DiscKernel[I].y;*/

		float2 SampleCoord = (DiscKernel[I] * PixelSize) * Radius;
		float SampleDepth = tex2D(DepthInput, SampleCoord).r;
		float3 SamplePosition = (SampleDepth * Input.ViewVector);

		if (distance(CurrentPosition, SamplePosition) < Radius)
			AmbientOcclusion -= 1.0f / 9.0f;
	}

	float4	CurrentColor = tex2D(ColorInput, Input.DepthTexcoord);
	
	return AmbientOcclusion * CurrentColor;
}
