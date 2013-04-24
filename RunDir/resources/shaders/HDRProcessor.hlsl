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

#ifndef __ZE_HDR_PROCESSOR_HLSL__
#define __ZE_HDR_PROCESSOR_HLSL__

#if defined (ZE_SHADER_COMPONENT_0)
	#define	TONE_MAP_LOGARITHMIC
#elif defined(ZE_SHADER_COMPONENT_1)
	#define	TONE_MAP_EXPONENTIAL
#elif defined(ZE_SHADER_COMPONENT_2)
	#define	TONE_MAP_REINHARD
#elif defined(ZE_SHADER_COMPONENT_3)
	#define	TONE_MAP_REINHARD_MODIFIED
#elif defined(ZE_SHADER_COMPONENT_4)
	#define	TONE_MAP_FILMIC
#endif

#if defined(ZE_SHADER_COMPONENT_5)
	#define AUTO_KEY_CALCULATION
#endif

#if defined(ZE_SHADER_COMPONENT_6)
	#define AUTO_EXPOSURE_CALCULATION
#endif

sampler2D	ColorTexture					: register(s0);
sampler2D	BloomTexture					: register(s1);
sampler2D	CurrentAdaptedLuminance			: register(s2);
sampler2D	PreviousAdaptedLuminance		: register(s3);
sampler2D	MeasuredLuminance				: register(s4);
sampler2D	AdditionTexture					: register(s5);

float2		PixelSize						: register(c0);
float		Key								: register(c1);
float		Exposure						: register(c3);
float		AdaptationRate					: register(c4);
float		BloomFactor						: register(c5);
float		BloomTreshold					: register(c6);
float		WhiteLevel						: register(c7);
float		Saturation						: register(c8);
float		ElapsedTime						: register(c9);
float		BloomWeight 					: register(c10);

#define		BLUR_FILTER_WIDTH				9
float4		BlurFilter[BLUR_FILTER_WIDTH]	: register(c15);
#define		FILTER_OFFSET(Index)			BlurFilter[Index].xy
#define		FILTER_TAP(Index)				BlurFilter[Index].z

static const float3 LumWeights = float3(0.299f, 0.587f, 0.114f);

float CalculateLuminance(float3 Color)
{
	return max(dot(Color, LumWeights), 0.001f);
}

/************************************************************/
/*					Tone Mapping Operators					*/
/************************************************************/
#ifdef TONE_MAP_LOGARITHMIC
	float3 ToneMapOperator(float3 Color)
	{
		float PixelLuminance = CalculateLuminance(Color);
		float ToneMappedLuminance = log10(1.0f + PixelLuminance) / log10(1.0f + WhiteLevel);
		return ToneMappedLuminance * pow(Color / PixelLuminance, Saturation);
	}
#endif

#ifdef TONE_MAP_EXPONENTIAL
	float3 ToneMapOperator(float3 Color)
	{
		float PixelLuminance = CalculateLuminance(Color);
		float ToneMappedLuminance = 1.0f - exp(-PixelLuminance / WhiteLevel);
		return ToneMappedLuminance * pow(Color / PixelLuminance, Saturation);
	}
#endif

#ifdef TONE_MAP_REINHARD
	float3 ToneMapOperator(float3 Color)
	{
		float PixelLuminance = CalculateLuminance(Color);
		float ToneMappedLuminance = PixelLuminance / (PixelLuminance + 1.0f);
		return ToneMappedLuminance * pow(Color / PixelLuminance, Saturation);
	}
#endif

#ifdef TONE_MAP_REINHARD_MODIFIED
	float3 ToneMapOperator(float3 Color)
	{
		float PixelLuminance = CalculateLuminance(Color);
		float ToneMappedLuminance = PixelLuminance * (1.0f + PixelLuminance / (WhiteLevel * WhiteLevel)) / (1.0f + PixelLuminance);
		return ToneMappedLuminance * pow(Color / PixelLuminance, Saturation);
	}
#endif

#ifdef TONE_MAP_FILMIC
	float3 ToneMapOperator(float3 Color)
	{
		Color = max(0, Color - 0.004f);
		Color = (Color * (6.2f * Color + 0.5f)) / (Color * (6.2f * Color + 1.7f)+ 0.06f);

		// result has 1/2.2 gamma baked in so revert it
		return pow(Color, 2.2f);
	}
#endif

/************************************************************/
/*				Shader Input Output Structures				*/
/************************************************************/
struct VSInput
{
	float3 Position	: POSITION0;
	float2 TexCoord : TEXCOORD0;
};

struct VSOutput
{
	float4 Position	: POSITION0;
	float2 TexCoord	: TEXCOORD0;
};

struct PSInput
{
	float2 TexCoord	: TEXCOORD0;
};

struct PSOutput
{
	float4 Color	: COLOR0;
};

/************************************************************/
/*						Vertex Shader Common				*/
/************************************************************/
VSOutput VSMainCommon(VSInput Input)
{
	VSOutput Output = (VSOutput)0.0f;

	Output.Position = float4(sign(Input.Position).xy, 0.0f, 1.0f);
	Output.TexCoord = Input.TexCoord + 0.5f * PixelSize;

	return Output;
}

/************************************************************/
/*				Pixel Shader Blur Horizontal				*/
/************************************************************/
float4 PSMainBlurHorizontal(PSInput Input) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	[unroll]
	for (int I = 0; I < BLUR_FILTER_WIDTH; ++I)
	{
		float2 SampleOffset = Input.TexCoord + FILTER_OFFSET(I);
		float4 SampleColor = tex2D(ColorTexture, SampleOffset);
		Color += SampleColor * FILTER_TAP(I);
	}

	Color.a = 1.0f;
	return Color;
}

/************************************************************/
/*				Pixel Shader Blur Vertical					*/
/************************************************************/
float4 PSMainBlurVerticalUpSample2x(PSInput Input) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	[unroll]
	for (int I = 0; I < BLUR_FILTER_WIDTH; ++I)
	{
		float2 SampleOffset = Input.TexCoord + FILTER_OFFSET(I);
		float4 SampleColor = tex2D(ColorTexture, SampleOffset);
		Color += SampleColor * FILTER_TAP(I);
	}

	float4 AddColor = tex2D(AdditionTexture, Input.TexCoord);

	return float4(Color.rgb * BloomWeight + AddColor.rgb, 1.0f);
}

/************************************************************/
/*		3x3	Sampling offsets for down sampling 3x			*/
/************************************************************/
static const float NormFactor3x3 = 1.0f / 9.0f;
static const float2 SampleOffsets3x3[9] = 
{
	{-1.0f, -1.0f},	{+0.0f, -1.0f},	{+1.0f, -1.0f},
	{-1.0f, +0.0f},	{+0.0f, +0.0f},	{+1.0f, +0.0f},
	{-1.0f, +1.0f},	{+0.0f, +1.0f},	{+1.0f, +1.0f}
};

/************************************************************/
/*				Pixel Shader Convert to Luminance			*/
/************************************************************/
float4 PSConvertToLuminance(PSInput Input) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	[unroll]
	for (int I  = 0; I < 9; ++I)
	{
		float2 SampleCoord = Input.TexCoord + SampleOffsets3x3[I] * PixelSize;
		float4 SampleColor = tex2D(ColorTexture, SampleCoord);
		Color += SampleColor;
	}
	Color *= NormFactor3x3;

	float Luminance = CalculateLuminance(Color.rgb);

	return float4(Luminance.r, 1.0f, 1.0f, 1.0f);
}

/************************************************************/
/*			Pixel Shader Luminanace Scale 3x				*/
/************************************************************/
float4 PSMainLuminanaceScale3x(PSInput Input) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	[unroll]
	for (int I = 0; I < 9; ++I)
	{
		float2 SampleCoord = Input.TexCoord + SampleOffsets3x3[I] * PixelSize;
		float4 SampleColor = tex2D(ColorTexture, SampleCoord);
		Color += SampleColor;
	}
	Color *= NormFactor3x3;

	return float4(Color.r, 1.0f, 1.0f, 1.0f);
}

/************************************************************/
/*					Pixel Shader Debug Print				*/
/************************************************************/
float4 PSMainDebugPrint(PSInput Input) : COLOR0
{
	float4 Color = tex2D(ColorTexture, Input.TexCoord);
	
	return float4(Color.rgb, 1.0f);
}

/************************************************************/
/*			Pixel Shader Color DownSample 2x				*/
/************************************************************/
float4 PSMainColorDownSample2x(PSInput Input) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	[unroll]
	for (int I = 0; I < 9; ++I)
	{
		float2 SampleCoord = Input.TexCoord + SampleOffsets3x3[I] * PixelSize;
		float4 SampleColor = tex2D(ColorTexture, SampleCoord);
		Color += SampleColor;
	}
	Color *= NormFactor3x3;

	return float4(Color.rgb, 1.0f);
}

/************************************************************/
/*				Pixel Shader Luminance Adaptation			*/
/************************************************************/
float4 PSMainLuminanceAdaptation(PSInput Input) : COLOR0
{
	float Luminance = 0.0f;

	[unroll]
	for (int I = 0; I < 9; ++I)
	{
		float2 SampleCoord = Input.TexCoord + SampleOffsets3x3[I] * PixelSize;
		float4 SampleColor = tex2D(MeasuredLuminance, SampleCoord);
		Luminance += SampleColor.r;
	}
	Luminance *= NormFactor3x3;

	// Measured luminance is linear at this point but previous luminanace is logarithmic
	float PreviousLuminance = exp(tex2D(PreviousAdaptedLuminance, Input.TexCoord).r);
	
	// Pattanaik's technique
    float NewLuminance = PreviousLuminance + (Luminance - PreviousLuminance) * (1.0f - exp(-ElapsedTime.x * AdaptationRate.x));

	// New luminance is logarithmic
	return float4(log(NewLuminance), 1.0f, 1.0f, 1.0f);
}

/************************************************************/
/*					Tone Mapping Methods					*/
/************************************************************/
float GetKey(float AverageLuminance)
{
	#if defined(AUTO_KEY_CALCULATION)
		return 1.03f - (2.0f / (2.0f + log10(AverageLuminance + 1.0f)));
	#else
		return Key;
	#endif
}

float GetExposure(float AverageLuminance, float KeyValue, float Treshold)
{
	#if defined(AUTO_EXPOSURE_CALCULATION)
		float LinearExposure = (KeyValue / AverageLuminance);
        return log2(max(LinearExposure, 0.01f)) - Treshold;
	#else
		return Exposure - Treshold;
	#endif
}

float3 ApplyExposure(float3 Color, float AverageLuminance, float Treshold)
{
	float FinalKey =  GetKey(AverageLuminance);
	float FinalExposure = GetExposure(AverageLuminance, FinalKey, Treshold);

	return exp2(FinalExposure) * Color;
}

float3 ToneMap(float3 Color, float AverageLuminance, float Treshold)
{
	float3 ExposedColor = ApplyExposure(Color, AverageLuminance, Treshold);
	float3 ToneMappedColor = ToneMapOperator(ExposedColor);

	return ToneMappedColor;
}

/************************************************************/
/*				Pixel Shader Bright Pass					*/
/************************************************************/
float4 PSMainBrightPass(PSInput Input) : COLOR0
{
	float3 Color = tex2D(ColorTexture, Input.TexCoord).rgb;
	float CurrentLuminance = exp(tex2D(CurrentAdaptedLuminance, Input.TexCoord).r);

	float3 MappedColor = ToneMap(Color, CurrentLuminance, BloomTreshold);

    return float4(MappedColor, 1.0f);
}

/************************************************************/
/*					Pixel Shader Combine					*/
/************************************************************/
float4 PSMainCombine(PSInput Input) : COLOR0
{	
	// Sample Colors
    float3 Color = tex2D(ColorTexture, Input.TexCoord).rgb;
	float3 Bloom = tex2D(BloomTexture, Input.TexCoord).rgb * BloomFactor;

	float CurrentLuminance = exp(tex2D(CurrentAdaptedLuminance, Input.TexCoord).r);
	CurrentLuminance = max(CurrentLuminance, 0.001f);

	// Tone Map with 0 treshold
	float3 MappedColor = ToneMap(Color, CurrentLuminance, 0);

	// Add in the bloom
	return float4(MappedColor + Bloom, 1.0f);
}

#endif
