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

#define ZERN_HTMO_LOGARITHMIC			0
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
	float			ZERNHDR_Saturation;

	float			ZERNHDR_LuminanceMin;
	float			ZERNHDR_LuminanceMax;
	float			ZERNHDR_Reserved1;
	float			ZERNHDR_Reserved2;
};

static const float3	ZERNHDR_LuminanceWeights = float3(0.212671f, 0.715160f, 0.072169f);

// TEXTURES
///////////////////////////////////////////////////////////////////////////////
Texture2D<float3>	ZERNHDR_InputTexture				: register(t5);
Texture2D<float3>	ZERNHDR_BlurTexture					: register(t6);
Texture2D<float>	ZERNHDR_CurrentLuminanceTexture		: register(t7);
Texture2D<float>	ZERNHDR_PreviousLuminanceTexture	: register(t8);

// SAMPLERS
///////////////////////////////////////////////////////////////////////////////

SamplerState 		ZERNHDR_SamplerLinearClamp			: register(s0);

// LUMINANCE CALCULATE
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Calculate_Luminance(float3 Color)
{
	return max(dot(Color, ZERNHDR_LuminanceWeights), 0.00000001f);
}

// AUTO KEY CALCULATE
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Calculate_Key(float AvgLuminance)
{
	return max(0.0f, 1.5f - (1.5f / (AvgLuminance * 0.1f + 1.0f))) + 0.1f;
}

// SCALED LUMINANCE CALCULATE
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Calculate_ScaledLuminance(float PixelLuminance)
{
	float AverageLuminance = ZERNHDR_CurrentLuminanceTexture.Load(int3(0, 0, 0));
	AverageLuminance = clamp(AverageLuminance, ZERNHDR_LuminanceMin, ZERNHDR_LuminanceMax);
	
	float Key = ZERNHDR_Key;
	if(ZERNHDR_AutoKey)
		Key = ZERNHDR_Calculate_Key(AverageLuminance);
	
	float ScaledLuminance = Key * PixelLuminance / AverageLuminance;
	
	return ScaledLuminance;
}

// TONE MAP OPERATORS
///////////////////////////////////////////////////////////////////////////////

float3 ZERNHDR_ToneMapOperator_Logarithmic(float3 PixelColor, float PixelLuminance, float ScaledLuminance)
{
	float ToneMappedLuminance = (log10(1.0f + ScaledLuminance) / log10(1.0f + ZERNHDR_WhiteLevel));
	return ToneMappedLuminance * pow(abs(PixelColor / PixelLuminance), ZERNHDR_Saturation);
}

float3 ZERNHDR_ToneMapOperator_Exponential(float3 PixelColor, float PixelLuminance, float ScaledLuminance)
{
	float ToneMappedLuminance = (1.0f - exp(-ScaledLuminance));
	return ToneMappedLuminance * pow(abs(PixelColor / PixelLuminance), ZERNHDR_Saturation);
}

float3 ZERNHDR_ToneMapOperator_Reinhard(float3 PixelColor, float PixelLuminance, float ScaledLuminance)
{
	float ToneMappedLuminance = (ScaledLuminance / (ScaledLuminance + 1.0f));
	return ToneMappedLuminance * pow(abs(PixelColor / PixelLuminance), ZERNHDR_Saturation);
}

float3 ZERNHDR_ToneMapOperator_ModifiedReinhard(float3 PixelColor, float PixelLuminance, float ScaledLuminance)
{
	float ToneMappedLuminance = (ScaledLuminance * (1.0f + ScaledLuminance / (ZERNHDR_WhiteLevel * ZERNHDR_WhiteLevel)) / (1.0f + ScaledLuminance));
	return ToneMappedLuminance * pow(abs(PixelColor / PixelLuminance), ZERNHDR_Saturation);
}

float3 ZERNHDR_ToneMapOperator_Uncharted(float3 PixelColor)
{
	float A = 0.15f;	//Shoulder Strength
	float B = 0.50f;	//Linear Strength
	float C = 0.10f;	//Linear Angle
	float D = 0.20f;	//Toe Strength
	float E = 0.02f;	//Toe Numerator
	float F = 0.30f;	//Toe Denumerator	E / F = Toe Angle
	
	return ((PixelColor * (A * PixelColor + C * B) + D * E) / (PixelColor * (A * PixelColor + B) + D * F)) - E / F;
}

float3 ZERNHDR_ToneMapOperator_Filmic(float3 PixelColor, float ScaledLuminance)
{
	float3 ScaledColor = ScaledLuminance * PixelColor;
	
	ScaledColor = max(0, ScaledColor - 0.004f);
	ScaledColor = (ScaledColor * (6.2f * ScaledColor + 0.5f)) / 
					(ScaledColor * (6.2f * ScaledColor + 1.7f) + 0.06f);

	// result has 1/2.2 gamma baked in so revert it
	return pow(ScaledColor, 2.2f);
}

// LUMINANCE CALCULATE PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Luminance_Calculate_PixelShader(float4 ScreenCoordinate : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{	
	float3 Color = ZERNHDR_InputTexture.Sample(ZERNHDR_SamplerLinearClamp, TexCoord);

	return log(ZERNHDR_Calculate_Luminance(Color));
}

// LUMINANCE DOWNSCALE PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Luminance_DownScale_PixelShader(float4 ScreenCoordinate : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	return ZERNHDR_InputTexture.Sample(ZERNHDR_SamplerLinearClamp, TexCoord).r;
}

// LUMINANCE CALCULATE ADAPTED PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float ZERNHDR_Luminance_CalculateAdapted_PixelShader() : SV_Target0
{
	float CurrentLuminance = exp(ZERNHDR_CurrentLuminanceTexture.Load(int3(0, 0, 0)));
	float PreviousLuminance = ZERNHDR_PreviousLuminanceTexture.Load(int3(0, 0, 0));
	
	float RodSensitivity = 0.04f / (0.04f + CurrentLuminance);
	float AdaptionConstant = RodSensitivity * 0.4f + (1.0f - RodSensitivity) * 0.1f;
	
	return PreviousLuminance + (CurrentLuminance - PreviousLuminance) * (1.0f - exp(-ZERNRenderer_ElapsedTime * AdaptionConstant));
}

// BRIGHTNESS CALCULATE PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float3 ZERNHDR_Bright_Calculate_PixelShader(float4 ScreenCoordinate : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	float3 Color = ZERNHDR_InputTexture.Sample(ZERNHDR_SamplerLinearClamp, TexCoord);
	float ScaledLuminance = ZERNHDR_Calculate_ScaledLuminance(ZERNHDR_Calculate_Luminance(Color));
	
	float Bright = max(ScaledLuminance - ZERNHDR_BloomThreshold, 0.0f);
	
	return (Bright / (1.0f + Bright)) * Color;
}

// TONE MAPPING PIXEL SHADER
///////////////////////////////////////////////////////////////////////////////

float4 ZERNHDR_ToneMapping_PixelShader(float4 ScreenCoordinate : SV_Position, float2 TexCoord : TEXCOORD0) : SV_Target0
{
	float3 Color = ZERNHDR_InputTexture.Load(int3(ScreenCoordinate.xy, 0));
	float Luminance = ZERNHDR_Calculate_Luminance(Color);
	float ScaledLuminance = ZERNHDR_Calculate_ScaledLuminance(Luminance);
	
	float3 ResultColor = (float3)0.0f;
	switch(ZERNHDR_ToneMapOperator)
	{
		default:
		case ZERN_HTMO_LOGARITHMIC:
			ResultColor = ZERNHDR_ToneMapOperator_Logarithmic(Color, Luminance, ScaledLuminance);
			break;
			
		case ZERN_HTMO_EXPONENTIAL:
			ResultColor = ZERNHDR_ToneMapOperator_Exponential(Color, Luminance, ScaledLuminance);
			break;
			
		case ZERN_HTMO_REINHARD:
			ResultColor = ZERNHDR_ToneMapOperator_Reinhard(Color, Luminance, ScaledLuminance);
			break;
			
		case ZERN_HTMO_MODIFIED_REINHARD:
			ResultColor = ZERNHDR_ToneMapOperator_ModifiedReinhard(Color, Luminance, ScaledLuminance);
			break;
			
		case ZERN_HTMO_FILMIC:
			ResultColor = ZERNHDR_ToneMapOperator_Filmic(Color, ScaledLuminance);
			break;
			
		case ZERN_HTMO_UNCHARTED:
		{
			float3 ScaledColor = ScaledLuminance * Color;
			ResultColor = ZERNHDR_ToneMapOperator_Uncharted(ScaledColor) * ( 1.0f / ZERNHDR_ToneMapOperator_Uncharted(ZERNHDR_WhiteLevel));
			break;
		}
	}
	
	if(ZERNHDR_BloomEnabled)
		ResultColor += ZERNHDR_BloomFactor * ZERNHDR_BlurTexture.Sample(ZERNHDR_SamplerLinearClamp, TexCoord).rgb;
	
	return float4(ResultColor, 1.0f);
}

#endif
