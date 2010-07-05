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
sampler Bloom						: register(s2);
sampler OldAverageLuminanceTexture	: register(s3);


// Parameters
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float2 PixelSize			: register(c0);
float4 Parameters0			: register(c1);
float4 Parameters1			: register(c2);
#define Exposure			(Parameters0[0])
#define BrightPassTreshold	(Parameters0[1])
#define BrightPassOffset	(Parameters0[2])
#define BloomFactor			(Parameters0[3])
#define ElapsedTime			(Parameters1[2])


// Kernels
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float2 Kernel4x4[16]	: register(c10);
const float2 Kernel[16]		: register(c15);


// General Vertex Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct VSInputOutput
{
	float4		Position : POSITION0;
	float2		Texcoord : TEXCOORD0;
};

VSInputOutput VSMain(VSInputOutput Input)
{
	VSInputOutput Output;
	Output.Position = Input.Position;
	Output.Texcoord = Input.Texcoord + 0.5f * PixelSize;
	return Output;
}


// Limunance Measurement (Start) Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PSLumMeasureStart(float2 Texcoord : TEXCOORD0) : COLOR0
{
	const float3 ColorWeights = {0.299f, 0.587f, 0.114f};
	
	float Luminance = 0.0f;
	for (int I = 0; I < 4; I++)
		Luminance += log(0.00001f + dot(tex2D(Input, Texcoord + PixelSize * Kernel4x4[I]).xyz, ColorWeights));

	return float4(Luminance / 4.0f, 0.0f, 0.0f, 0.0f);
}


// Limunance Measurement (Down Sample) Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PSLumDownSample3x(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float AverageLuminance = 0.0f;

	for (int I = 0; I < 9; I++)
		AverageLuminance += tex2D(Input, Texcoord + PixelSize * Kernel[I]).r;

	return float4(AverageLuminance / 9.0f, 0.0f, 0.0f, 0.0f);
}


// Limunance Measurement (End) Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PSLumMeasureEnd(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float AverageLuminance = 0.0f;

	for (int I = 0; I < 9; I++)
		AverageLuminance += tex2D(Input, Texcoord + PixelSize * Kernel[I]).r;
	AverageLuminance = exp(AverageLuminance / 9.0f);
	float OldAverageLuminance = tex2D(OldAverageLuminanceTexture, float2(0.5f, 0.5f));
	float AdaptiveAverageLuminance = lerp(AverageLuminance, OldAverageLuminance, pow(0.000001f, ElapsedTime / 30.0f));
	
	return float4(AdaptiveAverageLuminance, 0.0f, 0.0f, 0.0f);
}


// Bright Pass Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PSBrightPass(float2 Texcoord : TEXCOORD0) : COLOR0
{
	const float3 ColorWeights = {0.299f, 0.587f, 0.114f};

	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int I = 0; I < 16; I++)
		Color += tex2D(Input, Texcoord + PixelSize * Kernel4x4[I]);
	Color /= 16.0f;
	
	float Luminance = dot(Color, ColorWeights);
	float AverageLuminance = tex2D(AverageLuminanceTexture, float2(0.5f, 0.5f));
	float ScaledLuminance = (Exposure / (AverageLuminance + 0.0001f)) * Luminance;
	ScaledLuminance = ScaledLuminance / (1.0f + ScaledLuminance);
	
	Color = max(Color - BrightPassTreshold, 0.0f);
	Color *= ScaledLuminance;
		
	return Color;
}


// Down Sample 4x Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PSColorDownSample4x(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int I = 0; I < 16; I++)
		Color += tex2D(Input, Texcoord + PixelSize * Kernel[I]);
	
	Color /= 16.0f;
	
	return Color;
}


// Vertical Bloom Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PSVerticalBloom(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int I = 0; I < 15; I++)
		Color += Kernel[I].y * tex2D(Input, Texcoord + float2(0.0f, PixelSize.y * Kernel[I].x));
		
	return Color;
}


// Horizontal Bloom Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PSHorizontalBloom(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int I = 0; I < 15; I++)
		Color += Kernel[I].y * tex2D(Input, Texcoord + float2(PixelSize.x * Kernel[I].x, 0.0f));
	
	return Color;
}


// Tone Mapping Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PSToneMap(float2 Texcoord : TEXCOORD0) : COLOR0
{
	const float3 ColorWeights = {0.299f, 0.587f, 0.114f};
	
	float4 Color = tex2D(Input, Texcoord);
	float AverageLuminance = tex2D(AverageLuminanceTexture, float2(0.5f, 0.5f));
	
	float Luminance = dot(Color, ColorWeights);
	float ScaledLuminance = (Exposure / (AverageLuminance + 0.0001f)) * Luminance;
	ScaledLuminance = ScaledLuminance / (1.0f + ScaledLuminance);
	
	Color *= ScaledLuminance;
	
	Color += BloomFactor * tex2D(Bloom, Texcoord);
	
	return Color;
}
