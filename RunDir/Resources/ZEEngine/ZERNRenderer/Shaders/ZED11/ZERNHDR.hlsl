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

#ifndef __ZERN_HDR_H__
#define __ZERN_HDR_H__

#include "ZERNRenderer.hlsl"
#include "ZERNScreenCover.hlsl"

// SHADER INPUT OUTPUT
///////////////////////////////////////////////////////////////////////////////

#define ZERN_HTMO_LOGARITMIC			0
#define ZERN_HTMO_EXPONENTIAL			1
#define ZERN_HTMO_REINHARD				2
#define ZERN_HTMO_MODIFIED_REINHARD		3
#define ZERN_HTMO_FILMIC				4
#define ZERN_HTMO_UNCHARTED				5

cbuffer ZERNHDR_Constants								: register(b8)
{	
	float			ZERNHDR_Key;
	float			ZERNHDR_WhiteLevel;
	float			ZERNHDR_BloomFactor;
	float			ZERNHDR_BloomThreshold;
		
	bool			ZERNHDR_AutoKey;
	uint			ZERNHDR_ToneMapOperator;
	bool			ZERNHDR_BloomEnabled;
	uint			ZERNHDR_Reserved;
};

static const float3	ZERNHDR_LuminanceWeights = float3(0.299f, 0.587f, 0.114f);

// TEXTURES
///////////////////////////////////////////////////////////////////////////////
Texture2D<float4>	ZERNHDR_InputTexture				: register(t0);
Texture2D<float4>	ZERNHDR_BlurTexture					: register(t1);
Texture2D<float>	ZERNHDR_CurrentLuminanceTexture		: register(t2);
Texture2D<float>	ZERNHDR_PreviousLuminanceTexture	: register(t3);

// SAMPLERS
///////////////////////////////////////////////////////////////////////////////

SamplerState 		ZERNHDR_SamplerLinearClanp			: register(s0);

// LUMINANCE CALCULATE
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Calculate_Luminance(float3 Color)
{
	return max(dot(Color, ZERNHDR_LuminanceWeights), 0.001f);
}

// KEY CALCULATE
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Calculate_Key(float AvgLuminance)
{
	return max(0.0f, 1.5f - (1.5f / (AvgLuminance * 0.1f + 1.0f))) + 0.1f;
}

// SCALED COLOR CALCULATE(aka Linear Mapping)
///////////////////////////////////////////////////////////////////////////////

float3 ZERNHDR_Calculate_ScaledColor(float3 Color)
{
	float AdaptedAverageLuminance = ZERNHDR_CurrentLuminanceTexture.Load(int3(0, 0, 0)).r;
	float Key;
	if(ZERNHDR_AutoKey)
		Key = ZERNHDR_Calculate_Key(AdaptedAverageLuminance);
	else
		Key = ZERNHDR_Key;
	
	float ScaledLuminance = Key * ZERNHDR_Calculate_Luminance(Color.rgb) / AdaptedAverageLuminance;
	
	return ScaledLuminance * Color;
}

// TONE MAP OPERATORS
///////////////////////////////////////////////////////////////////////////////

float3 ZERNHDR_ToneMapOperator_Logaritmic(float3 Color)
{
	float PixelLuminance = ZERNHDR_Calculate_Luminance(Color);
	return log10(1.0f + PixelLuminance) / log10(1.0f + ZERNHDR_WhiteLevel);
}

float3 ZERNHDR_ToneMapOperator_Exponential(float3 Color)
{
	float PixelLuminance = ZERNHDR_Calculate_Luminance(Color);
	return 1.0f - exp(-PixelLuminance / ZERNHDR_WhiteLevel);
}

float3 ZERNHDR_ToneMapOperator_Reinhard(float3 Color)
{
	float3 ScaledColor = ZERNHDR_Calculate_ScaledColor(Color);
	return  ScaledColor / (ScaledColor + 1.0f);
}

float3 ZERNHDR_ToneMapOperator_ModifiedReinhard(float3 Color)
{
	float3 ScaledColor = ZERNHDR_Calculate_ScaledColor(Color);
	return ScaledColor * (1.0f + ScaledColor / (ZERNHDR_WhiteLevel * ZERNHDR_WhiteLevel)) / (1.0f + ScaledColor);
}

float3 ZERNHDR_ToneMapOperator_Uncharted(float3 Color)
{
	float A = 0.15f;
	float B = 0.50f;
	float C = 0.10f;
	float D = 0.20f;
	float E = 0.02f;
	float F = 0.30f;
	
	float3 ScaledColor = ZERNHDR_Calculate_ScaledColor(Color) * 2.0f;
	
	return ((ScaledColor * (A * ScaledColor + C * B) + D * E) / (ScaledColor * (A * ScaledColor + B) + D * F)) - E / F;
}

float3 ZERNHDR_ToneMapOperator_Filmic(float3 Color)
{
	Color = max(0, Color - 0.004f);
	Color = (Color * (6.2f * Color + 0.5f)) / (Color * (6.2f * Color + 1.7f)+ 0.06f);

	// result has 1/2.2 gamma baked in so revert it
	return pow(Color, 2.2f);
}

// LUMINANCE CALCULATE ADAPTED PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Luminance_CalculateAdapted_PixelShader() : SV_Target0
{
	float CurrentLuminance = exp(ZERNHDR_CurrentLuminanceTexture.Load(int3(0, 0, 0)).r);
	float PreviousLuminance = ZERNHDR_PreviousLuminanceTexture.Load(int3(0, 0, 0)).r;
	
	float RodSensitivity = 0.04f / (0.04f + CurrentLuminance);
	float AdaptionConstant = RodSensitivity * 0.4f + (1.0f - RodSensitivity) * 0.1f;
	
	return PreviousLuminance + (CurrentLuminance - PreviousLuminance) * (1.0f - exp(-ZERNRenderer_ElapsedTime * AdaptionConstant));;
}

// LUMINANCE CALCULATE PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Luminance_Calculate_PixelShader(float4 ScreenCoordinate : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{	
	float4 Color = ZERNHDR_InputTexture.Sample(ZERNHDR_SamplerLinearClamp, TexCoord);

	return log(ZERNHDR_Calculate_Luminance(Color.rgb));
}

// LUMINANCE DOWNSCALE PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Luminance_DownScale_PixelShader(float4 ScreenCoordinate : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	return ZERNHDR_InputTexture.Sample(ZERNHDR_SamplerLinearClamp, TexCoord).r;
}

// BRIGHTNESS CALCULATE PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float3 ZERNHDR_Bright_Calculate_PixelShader(float4 ScreenCoordinate : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	float3 Color = ZERNHDR_InputTexture.Sample(ZERNHDR_SamplerLinearClamp, TexCoord).rgb;
	float3 ScaledColor = ZERNHDR_Calculate_ScaledColor(Color);
	
	float3 BrightColor = max(ScaledColor - ZERNHDR_BloomThreshold, (float3)0.0f);
	
	return BrightColor / (1.0f + BrightColor);
}

// TONE MAPPING PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_ToneMapping_PixelShader(float4 ScreenCoordinate : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	float3 Color = ZERNHDR_InputTexture.Load(int3(ScreenCoordinate.xy, 0)).rgb;
	
	if(ZERNHDR_BloomEnabled)
		Color += ZERNHDR_BloomFactor * ZERNHDR_BlurTexture.Sample(ZERNHDR_SamplerLinearClamp, TexCoord).rgb;
	
	float3 ResultColor = (float3)0.0f;
	switch(ZERNHDR_ToneMapOperator)
	{
		case ZERN_HTMO_LOGARITMIC:
			ResultColor = ZERNHDR_ToneMapOperator_Logaritmic(Color);
			break;
		case ZERN_HTMO_EXPONENTIAL:
			ResultColor = ZERNHDR_ToneMapOperator_Exponential(Color);
			break;		
		case ZERN_HTMO_REINHARD:
			ResultColor = ZERNHDR_ToneMapOperator_Reinhard(Color);
			break;
		case ZERN_HTMO_MODIFIED_REINHARD:
			ResultColor = ZERNHDR_ToneMapOperator_ModifiedReinhard(Color);
			break;		
		case ZERN_HTMO_FILMIC:
			ResultColor = ZERNHDR_ToneMapOperator_Filmic(Color);
			break;
		case ZERN_HTMO_UNCHARTED:
			ResultColor = ZERNHDR_ToneMapOperator_Uncharted(Color) * ( 1.0f / ZERNHDR_ToneMapOperator_Uncharted(ZERNHDR_WhiteLevel));
			break;
	}
	
	return float4(ResultColor, 1.0f);
}

#endif
