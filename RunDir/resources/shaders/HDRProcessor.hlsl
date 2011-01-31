//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - HDRProcessor.hlsl
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

// Textures
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
sampler Input						: register(s0);
sampler AverageLuminanceTexture		: register(s1);
sampler OldAverageLuminanceTexture	: register(s2);
sampler Bloom2x						: register(s3);
sampler Bloom4x						: register(s4);
sampler Bloom8x						: register(s5);
sampler Bloom16x					: register(s6);


// Parameters
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float2 PixelSize			: register(c0);
float4 Parameters0			: register(c1);
float4 Parameters1			: register(c2);
#define Key					(Parameters0[0])
#define BrightPassTreshold	(Parameters0[1])
#define BloomFactor			(Parameters0[2])

#define ElapsedTime	(Parameters1[0])

int4 IntParameters0			: register(i0);

#define BloomSampleCount	(IntParameters0[0])

bool BloomPass1				: register(b0);
bool BloomPass2				: register(b1);
bool BloomPass3				: register(b2);
bool BloomPass4				: register(b3);

// Kernels
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float2 Kernel[16]	: register(c10);


// General Vertex Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct VSInputOutput
{
	float4		Position : POSITION0;
	float2		Texcoord : TEXCOORD0;
};

VSInputOutput ZED3D9HDRProcessor_VertexShader(VSInputOutput Input)
{
	VSInputOutput Output;
	Output.Position = Input.Position;
	Output.Texcoord = Input.Texcoord + 0.5f * PixelSize;
	return Output;
}

float ZED3D9HDRProcessor_LuminanceTransform(float3 Color)
{
	const float3 ColorWeights = {0.299f, 0.587f, 0.114f};
	return dot(Color, ColorWeights);
}

float ZED3D9HDRProcessor_LuminanceAdaptationOperator(float OldLuminance, float NewLuminance)
{
	//return NewLuminance;
	return lerp(NewLuminance, OldLuminance, pow(0.5f, ElapsedTime));
/*
	if (OldLuminance > NewLuminance)
		return max(OldLuminance - MaxLuminanceChange, NewLuminance);
	else
		return min(OldLuminance + MaxLuminanceChange, NewLuminance);
		*/
}

float4 ZED3D9HDRProcessor_ToneMapOperator(float4 Color, float Luminance)
{

}


// Limunance Measurement (Start) Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 ZED3D9HDRProcessor_MeasureLuminanceStart(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float Luminance = 0.0f;
	for (int I = 0; I < 4; I++)
		Luminance += log(0.00001f + ZED3D9HDRProcessor_LuminanceTransform(tex2D(Input, Texcoord + PixelSize * Kernel[I]).xyz));

	return float4(Luminance / 4.0f, 0.0f, 0.0f, 0.0f);
}


// Limunance Measurement (Down Sample) Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 ZED3D9HDRProcessor_MeasureLuminanceDownSample3x(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float AverageLuminance = 0.0f;

	for (int I = 0; I < 9; I++)
		AverageLuminance += tex2D(Input, Texcoord + PixelSize * Kernel[I]).r;

	return float4(AverageLuminance / 9.0f, 0.0f, 0.0f, 0.0f);
}


// Limunance Measurement (End) Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 ZED3D9HDRProcessor_MeasureLuminanceEnd(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float AverageLuminance = 0.0f;

	for (int I = 0; I < 9; I++)
		AverageLuminance += tex2D(Input, Texcoord + PixelSize * Kernel[I]).r;
		
	AverageLuminance = exp(AverageLuminance / 9.0f);
	float OldAverageLuminance = tex2D(OldAverageLuminanceTexture, float2(0.5f, 0.5f));
	float AdaptiveAverageLuminance = ZED3D9HDRProcessor_LuminanceAdaptationOperator(OldAverageLuminance, AverageLuminance);
	
	return float4(AdaptiveAverageLuminance, 0.0f, 0.0f, 0.0f);
}


// Bright Pass Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 ZED3D9HDRProcessor_BrightPass(float2 Texcoord : TEXCOORD0) : COLOR0
{
	const float3 ColorWeights = {0.299f, 0.587f, 0.114f};

	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int I = 0; I < 4; I++)
		Color += tex2D(Input, Texcoord + PixelSize * Kernel[I]);
	Color /= 4.0f;
	
	float Luminance = ZED3D9HDRProcessor_LuminanceTransform(Color);
	float AverageLuminance = tex2D(AverageLuminanceTexture, float2(0.5f, 0.5f));
	float ScaledLuminance = (Key / (AverageLuminance + 0.0001f)) * Luminance;
	ScaledLuminance = ScaledLuminance / (1.0f + ScaledLuminance);
	Color *= ScaledLuminance;
	
	if (ScaledLuminance < BrightPassTreshold)
		Color = 0.0f;

	return Color;
}


// Down Sample 2x Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 ZED3D9HDRProcessor_DownSample2x(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int I = 0; I < 4; I++)
		Color += tex2D(Input, Texcoord + PixelSize * Kernel[I]);
	
	Color /= 4.0f;
	
	return Color;
}


// Vertical Bloom Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 ZED3D9HDRProcessor_VerticalBloom(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int I = 0; I < 7; I++)
		Color += Kernel[I].y * tex2D(Input, Texcoord + float2(0.0f, PixelSize.y * Kernel[I].x));
		
	return Color;
}


// Horizontal Bloom Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 ZED3D9HDRProcessor_HorizontalBloom(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int I = 0; I < 7; I++)
		Color += Kernel[I].y * tex2D(Input, Texcoord + float2(PixelSize.x * Kernel[I].x, 0.0f));
	
	return Color;
}


// Tone Mapping Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 ZED3D9HDRProcessor_ToneMap(float2 Texcoord : TEXCOORD0) : COLOR0
{
	const float3 ColorWeights = {0.299f, 0.587f, 0.114f};
	
	float4 Color = tex2D(Input, Texcoord);

	float Luminance = ZED3D9HDRProcessor_LuminanceTransform(Color);
	float AverageLuminance = tex2D(AverageLuminanceTexture, float2(0.5f, 0.5f));
	float ScaledLuminance = (Key / (AverageLuminance + 0.0001f)) * Luminance;
	ScaledLuminance = ScaledLuminance / (1.0f + ScaledLuminance);

	Color *= ScaledLuminance;
	
	float3 Bloom = float3(0.0f, 0.0f, 0.0f);
	if (BloomPass1)
		Bloom = tex2D(Bloom2x, Texcoord).rgb;

	if (BloomPass2)
		Bloom = tex2D(Bloom4x, Texcoord).rgb;

	if (BloomPass3)
		Bloom = tex2D(Bloom8x, Texcoord).rgb;

	if (BloomPass4)
		Bloom = tex2D(Bloom16x, Texcoord).rgb;
		
	Color.rgb += BloomFactor * Bloom;
	
	return Color;
}
