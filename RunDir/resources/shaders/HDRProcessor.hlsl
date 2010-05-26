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

float2 PixelSize		: register(c0);
float TimeElapsed		: register(c1);

sampler Input			: register(s0);
sampler OldLimunance	: register(s1);

// Kernels
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float2 Kernel2x2[4] = 
{
	{-0.5f, -0.5f},
	{-0.5f,  0.5f},
	{ 0.5f, -0.5f},
	{ 0.5f,  0.5f}
};

float2 Kernel3x3[9] = 
{
	{-1.0f, -1.0f},
	{-1.0f,  0.0f},
	{-1.0f,  1.0f},
	{ 0.0f, -1.0f},
	{ 0.0f,  0.0f},
	{ 0.0f,  1.0f},
	{ 1.0f, -1.0f},
	{ 1.0f,  0.0f},
	{ 1.0f,  1.0f}
};

float2 Kernel4x4[16] = 
{
	{-1.5f, -1.5f},
	{-1.5f, -0.5f},
	{-1.5f,  0.5f},
	{-1.5f,  1.5f},

	{-0.5f, -1.5f},
	{-0.5f, -0.5f},
	{-0.5f,  0.5f},
	{-0.5f,  1.5f},

	{ 0.5f, -1.5f},
	{ 0.5f, -0.5f},
	{ 0.5f,  0.5f},
	{ 0.5f,  1.5f},

	{ 1.5f, -1.5f},
	{ 1.5f, -0.5f},
	{ 1.5f,  0.5f},
	{ 1.5f,  1.5f}
};

// General Vertex Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct VS_InputOutput
{
	float4		Position : POSITION0;
	float2		Texcoord : TEXCOORD0;
};

VS_InputOutput VS_Main (VS_InputOutput Input)
{
	VS_InputOutput Output;
	Output.Position = Input.Position + float4(PixelSize, 0.0f, 0.0f);
	Output.Texcoord = Input.Texcoord;
	return Output;
}

// Limunance Measurement (Start) Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PS_LumMeasureStart(float2 Texcoord : TEXCOORD0) : COLOR0
{
	const float3 ColorWeights = {0.299f, 0.587f, 0.114f};
	
	float Luminance = 0.0f;
	for (int I = 0; I < 4; I++)
		Luminance = log(0.00001 + dot(tex2D(Input, Texcoord + PixelSize * Kernel2x2[I]).xyz, ColorWeights));

	return float4(exp(Luminance / 4.0f), 0.0f, 0.0f, 0.0f);
}

// Limunance Measurement (Down Sample) Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PS_LumDownSample3x(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float AvarageLuminance = 0.0f;

	for (int I = 0; I < 9; I++)
		AvarageLuminance += tex2D(Input, Texcoord + PixelSize * Kernel3x3[I]).r;

	return float4(AvarageLuminance / 9.0f, 0.0f, 0.0f, 0.0f);
}

// Limunance Measurement (End) Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PS_LumMeasureEnd(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float AvarageLuminance = 0.0f;

	for (int I = 0; I < 9; I++)
		AvarageLuminance += tex2D(Input, Texcoord + PixelSize * Kernel3x3[I]).r;

	AvarageLuminance /= 9.0f;

	float Luminance = lerp(AvarageLuminance, tex2D(OldLimunance, float2(0.5f, 0.5f)), TimeElapsed);

	return float4(Luminance, 0.0f, 0.0f, 0.0f);
}

