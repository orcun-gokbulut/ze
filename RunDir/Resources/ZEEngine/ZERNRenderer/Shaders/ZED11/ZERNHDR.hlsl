//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNHDR.hlsl
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

#ifndef __ZERN_HDR_HLSL__
#define __ZERN_HDR_HLSL__

#include "ZERNRenderer.hlsl"

// SHADER INPUT OUTPUT
///////////////////////////////////////////////////////////////////////////////

#define ZERNHDR_BloomFilterWidth		9
#define ZERNHDR_BloomFilterHalfWidth	4

#define ZERN_HTMO_LOGARITMIC			0
#define ZERN_HTMO_EXPONENTIAL			1
#define ZERN_HTMO_RAINHARD				2
#define ZERN_HTMO_MODIFIED_RAINHARD		3
#define ZERN_HTMO_FILMIC				4

#define ZERN_BRIGTNESS_THRESHOLD		0.5f
#define ZERN_MIDDLE_GREY				0.08f

// SHADER INPUT OUTPUT
///////////////////////////////////////////////////////////////////////////////

cbuffer ZERNHDR_Constants	:	register(b8)
{
	bool		ZERNHDR_AutoKey;
	bool		ZERNHDR_AutoExposure;
	bool		ZERNHDR_BloomEnabled;
	bool		ZERNHDR_Reserved0;

	float		ZERNHDR_Key;
	float		ZERNHDR_Exposure;
	float		ZERNHDR_WhiteLevel;
	float		ZERNHDR_Saturation;
	
	float		ZERNHDR_LuminanceUpper;
	float		ZERNHDR_LuminanceLower;
	float		ZERNHDR_AdaptationRate;
	float		ZERNHDR_BloomFactor;

	float		ZERNHDR_BloomThreshold;
	float		ZERNHDR_BloomDeviation;
	float		ZERNHDR_BloomWeightLarge;
	float		ZERNHDR_BloomWeightMedium;

	uint		ZERNHDR_ToneMapOperator;
	uint		ZERNHDR_BloomPassCount;
	uint		ZERNHDR_Reserved1[2];
	
	float3		ZERNHDR_BloomFilter[9];
	float		ZERNHDR_BloomWeightSmall;
};

static const float3 ZERNHDR_LuminanceWeights = float3(0.299f, 0.587f, 0.114f);
static const float  ZERNHDR_Luminance_SampleNormalization = 1.0f / 9.0f;
static const float2 ZERNHDR_Luminance_SampleOffsets3x3[9] = 
{
	{-1.0f, -1.0f},	{+0.0f, -1.0f},	{+1.0f, -1.0f},
	{-1.0f, +0.0f},	{+0.0f, +0.0f},	{+1.0f, +0.0f},
	{-1.0f, +1.0f},	{+0.0f, +1.0f},	{+1.0f, +1.0f}
};

static const float2 ZERNHDR_Luminance_SampleOffsets2x2[] =
{
	{-1.0f, -1.0f},	{+1.0f, -1.0f},
};  {-1.0f, +1.0f},	{+1.0f, +1.0f}

// TEXTURES
///////////////////////////////////////////////////////////////////////////////
Texture2D<float4>	InputTexture					: register(t0);
Texture2D<float4>	AvgLuminanceTexture				: register(t1);
Texture2D<float>	CurrentAdaptedLuminance			: register(t2);
Texture2D<float>	PreviousAdaptedLuminance		: register(t3);
Texture2D<float>	MeasuredLuminance				: register(t4);
Texture2D<float>	AdditionTexture					: register(t5);


SamplerState ZERNHDR_SamplerLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

// LUMINANCE CALCULATE
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Luminance_Calculate(float3 Color)
{
	return max(dot(Color, ZERNHDR_LuminanceWeights), 0.001f);
}


// TONE MAP OPERATORS
///////////////////////////////////////////////////////////////////////////////

float3 ZERNHDR_ToneMapOperator_Logaritmic(float3 Color)
{
	float PixelLuminance = ZERNHDR_Luminance_Calculate(Color);
	float ToneMappedLuminance = log10(1.0f + PixelLuminance) / log10(1.0f + ZERNHDR_WhiteLevel);
	return ToneMappedLuminance * pow(Color / PixelLuminance, ZERNHDR_Saturation);
}

float3 ZERNHDR_ToneMapOperator_Exponential(float3 Color)
{
	float PixelLuminance = ZERNHDR_Luminance_Calculate(Color);
	float ToneMappedLuminance = 1.0f - exp(-PixelLuminance / ZERNHDR_WhiteLevel);
	return ToneMappedLuminance * pow(Color / PixelLuminance, ZERNHDR_Saturation);
}

float3 ZERNHDR_ToneMapOperator_Reinhard(float3 Color)
{
	float PixelLuminance = ZERNHDR_Luminance_Calculate(Color);
	float ToneMappedLuminance = PixelLuminance / (PixelLuminance + 1.0f);
	return ToneMappedLuminance * pow(Color / PixelLuminance, ZERNHDR_Saturation);
}

float3 ZERNHDR_ToneMapOperator_ModifiedReinhard(float3 Color)
{
	float PixelLuminance = ZERNHDR_Luminance_Calculate(Color);
	float ToneMappedLuminance = PixelLuminance * (1.0f + PixelLuminance / (ZERNHDR_WhiteLevel * ZERNHDR_WhiteLevel)) / (1.0f + PixelLuminance);
	return ToneMappedLuminance * pow(Color / PixelLuminance, ZERNHDR_Saturation);
}

float3 ZERNHDR_ToneMapOperator_Filmic(float3 Color)
{
	Color = max(0, Color - 0.004f);
	Color = (Color * (6.2f * Color + 0.5f)) / (Color * (6.2f * Color + 1.7f)+ 0.06f);

	// result has 1/2.2 gamma baked in so revert it
	return pow(Color, 2.2f);
}

// BLUR HORIZONTAL
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_Bloom_BlurHorizontal_PixelShader(float4 ScreenCoordinate : SV_Position) : COLOR0
{
	float3 Color = 0.0f;

	[unroll]
	for (int I = 0; I < ZERNHDR_BloomFilerWidth; ++I)
	{
		float2 SampleOffset = ScreenCoordinate.xy + ZERNHDR_BloomFilter[I].xy;
		float3 SampleColor = ColorTexture.Load(int3(SampleOffset.x, SampleOffset.y, 0)).rgb;
		Color += SampleColor * ZERNHDR_BloomFilter[I].z;
	}

	return float4(Color, 1.0f);
}


// BLUR VERTICAL
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_Bloom_BlurVertical(int2 ScreenCoordinate, float BloomWeight)
{
	float3 Color = 0.0f;

	[unroll]
	for (int I = 0; I < ZERNHDR_BloomFilerWidth; ++I)
	{
		ScreenCoordinate.y += I - ZERNHDR_BloomFilterHalfWidth;
		float3 SampleColor = ColorTexture.Load(int3(SampleOffset, 0)).rgb;
		Color += SampleColor * ZERNHDR_BloomFilter[I];
	}

	Color *= ZERNHDR_BloomWeight;

	float3 AddColor = AdditionTexture.Load(int3(ScreenCoordinate.xy, 0));
	Color += AddColor;

	return float4(Color, 1.0f);
}

float4 ZERNHDR_Bloom_BlurVerticalSmall_PixelShader(float4 ScreenCoordinate : SV_Position) : COLOR0
{
	return ZERNHDR_Bloom_BlurVertical(ScreenCoordinate.xy, ZERNHDR_BloomWeightSmall);
}

float4 ZERNHDR_Bloom_BlurVerticalMedium_PixelShader(float4 ScreenCoordinate : SV_Position) : COLOR0
{
	return ZERNHDR_Bloom_BlurVertical(ScreenCoordinate.xy, ZERNHDR_BloomWeightSmall);
}

float4 ZERNHDR_Bloom_BlurVerticalLarge_PixelShader(float4 ScreenCoordinate : SV_Position) : COLOR0
{
	return ZERNHDR_Bloom_BlurVertical(ScreenCoordinate.xy, ZERNHDR_BloomWeightSmall);
}

// DOWNSAMPLE 2X PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_DownSample2x_PixelShader(float4 ScreenCoordinate : SV_Position) : COLOR0
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	[unroll]
	for (int I = 0; I < 9; ++I)
	{
		float2 SampleCoord = ScreenCoordinate + SampleOffsets3x3[I] * PixelSize;
		float4 SampleColor = ColorTexture.Load(SampleCoord);
		Color += SampleColor;
	}
	Color *= NormFactor3x3;

	return float4(Color.rgb, 1.0f);
}


// LUMINANCE ADAPTATION PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float4 ZEHDR_Luminance_Adaptation_PixelShader(float4 ScreenCoordinate : SV_Position) : COLOR0
{
	float Luminance = 0.0f;

	[unroll]
	for (int I = 0; I < 9; ++I)
	{
		float2 SampleCoord = ScreenCoordinate + SampleOffsets3x3[I];
		float4 SampleColor = MeasuredLuminance.Load(SampleCoord);
		Luminance += SampleColor.r;
	}
	Luminance *= NormFactor3x3;

	// Measured luminance is linear at this point but previous luminanace is logarithmic
	float PreviousLuminance = exp(PreviousAdaptedLuminance.tex2D(ScreenCoordinate).r);
	
	// Pattanaik's technique
    float NewLuminance = PreviousLuminance + (Luminance - PreviousLuminance) * (1.0f - exp(-ElapsedTime * AdaptationRate));

	// New luminance is logarithmic
	return float4(log(NewLuminance), 1.0f, 1.0f, 1.0f);
}


// TONE MAPPING
///////////////////////////////////////////////////////////////////////////////

float ZEHDR_GetKey(float AverageLuminance)
{
	#if defined(AUTO_KEY_CALCULATION)
		return 1.03f - (2.0f / (2.0f + log10(AverageLuminance + 1.0f)));
	#else
		return Key;
	#endif
}

float ZEHDR_GetExposure(float AverageLuminance, float KeyValue, float Threshold)
{
	#if defined(AUTO_EXPOSURE_CALCULATION)
		float LinearExposure = (KeyValue / AverageLuminance);
        return log2(max(LinearExposure, 0.01f)) - Threshold;
	#else
		return Exposure - Threshold;
	#endif
}

float3 ZEHDR_ApplyExposure(float3 Color, float AverageLuminance, float Threshold)
{
	float FinalKey =  GetKey(AverageLuminance);
	float FinalExposure = GetExposure(AverageLuminance, FinalKey, Threshold);

	return exp2(FinalExposure) * Color;
}

float3 ZEHDR_ToneMap(float3 Color, float AverageLuminance, float Threshold)
{
	float3 ExposedColor = ApplyExposure(Color, AverageLuminance, Threshold);
	float3 ToneMappedColor = ToneMapOperator(ExposedColor);

	return ToneMappedColor;
}

// BRIGHT PASS
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_BrightPass_PixelShader(float4 ScreenCoordinate : SV_Position) : COLOR0
{
	float3 Color = tex2D(ColorTexture, Input.TexCoord).rgb;
	float CurrentLuminance = exp(CurrentAdaptedLuminance.Load(ScreenCoordinate).r);

	float3 MappedColor;
	switch(ZERNHDR_ToneMapOperator)
	{
		default:
		case ZERNHDR_TMO_LOGARITMIC:
			MapColor = ZERNHDR_ToneMapOperator_Logaritmic(Color, CurrentLuminance, BloomThreshold);
			break;

		case ZERNHDR_TMO_EXPONENTIAL:
			MapColor = ZERNHDR_ToneMapOperator_Exponential(Color, CurrentLuminance, BloomThreshold);
			break;

		case ZERNHDR_TMO_RAINHARD:
			MapColor = ZERNHDR_ToneMapOperator_Reinhard(Color, CurrentLuminance, BloomThreshold);
			break;

		case ZERNHDR_TMO_MODIFIED_RAINHARD:
			MapColor = ZERNHDR_ToneMapOperator_ModifiedRainHard(Color, CurrentLuminance, BloomThreshold);
			break;

		case ZERNHDR_TMO_FILMIC:
			MapColor = ZERNHDR_ToneMapOperator_Filmic(Color, CurrentLuminance, BloomThreshold);
			break;

	}

	return float4(MappedColor, 1.0f);
}


// COMBINE
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_Combine_PixelShader(float4 ScreenCoordinate : SV_Position) : COLOR0
{	
	// Sample Colors
    float3 Color = ColorTexture.Load(ScreenCoordinate).rgb;
	float3 Bloom = BloomTexture.Load(ScreenCoordinate).rgb * BloomFactor;

	float CurrentLuminance = exp(tex2D(CurrentAdaptedLuminance, Input.TexCoord).r);
	CurrentLuminance = max(CurrentLuminance, 0.001f);

	// Tone Map with 0 Threshold
	float3 MappedColor = ToneMap(Color, CurrentLuminance, 0);

	// Add in the bloom
	return float4(MappedColor + Bloom, 1.0f);
}

// LUMINANCE CALCULATE PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_Luminance_Calculate_PixelShader(float4 ScreenCoordinate : SV_Position) : SV_Target0
{
	float2 ScreenSize;
	InputTexture.GetDimensions(ScreenSize.x, ScreenSize.y);
	float2 TexCoord = ScreenCoordinate.xy / ScreenSize;
	
	float4 Color = InputTexture.Sample(ZERNHDR_SamplerLinear, TexCoord));
	float Luminance = log(ZERNHDR_Luminance_Calculate(Color.rgb));

	return float4(Luminance.r, 1.0f, 1.0f, 1.0f);
}

// LUMINANCE DOWNSCALE PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_Luminance_DownScale_PixelShader(float4 ScreenCoordinate : SV_Position) : SV_Target0
{
	float2 ScreenSize;
	InputTexture.GetDimensions(ScreenSize.x, ScreenSize.y);
	float2 TexCoord = ScreenCoordinate.xy / ScreenSize;
	
	float4 Color = InputTexture.Sample(ZERNHDR_SamplerLinear, TexCoord);

	return float4(Color.r, 1.0f, 1.0f, 1.0f);
}

// BRIGHTNESS CALCULATE PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_Bright_Calculate_PixelShader(float4 ScreenCoordinate : SV_Position) : SV_Target0
{
	float2 ScreenSize;
	InputTexture.GetDimensions(ScreenSize.x, ScreenSize.y);
	float2 TexCoord = ScreenCoordinate.xy / ScreenSize;
	
	float4 Color = InputTexture.Sample(ZERNHDR_SamplerLinear, TexCoord);
	
	float Brightness = max(Color.r - ZERN_BRIGTNESS_THRESHOLD, 0.0f);
	
	return float4(Brightness, 0.0f, 0.0f, 1.0f);
}

// TONE MAPPING PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_ToneMapping_PixelShader(float4 ScreenCoordinate : SV_Position) : SV_Target0
{
	float2 ScreenSize;
	InputTexture.GetDimensions(ScreenSize.x, ScreenSize.y);
	float2 TexCoord = ScreenCoordinate.xy / ScreenSize;
	
	float4 Color = InputTexture.Sample(ZERNHDR_SamplerLinear, TexCoord);
	float AvgLuminance = ZERN_MIDDLE_GREY / exp(AvgLuminanceTexture.Load(int3(0.0, 0.0, 0)).r);
	float3 ResultColor = AvgLuminance * Color;
	ResultColor = ResultColor / ( 1 + ResultColor);
	
	return float4(ResultColor, 1.0f);
}

#endif
