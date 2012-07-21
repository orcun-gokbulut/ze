//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ColorTransformProcessor.hlsl
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
sampler2D 	TextureInput	 		: register(s6);

// Vertex Shader Parameters
float4 		VSParameters			: register(vs, c0);

#define		PixelSize				VSParameters.xy

// Pixel Shader parameters
float4		PSParameters			: register(ps, c0);
float4x4	ColorTransformMatrix 	: register(ps, c1);

#define		HueBoost				PSParameters.x
#define		IntensityBoost			PSParameters.y
#define		SaturationBoost			PSParameters.z
#define		BlendFactor				PSParameters.w


#define MAX(Vector3)	(max(max( (Vector3.x), (Vector3.y) ), (Vector3.z) ))
#define MIN(Vector3)	(min(min( (Vector3.x), (Vector3.y) ), (Vector3.z) ))


// Vertex Shader Input Struct
struct VS_INPUT
{
	float4 Position  : POSITION0;
	float2 Texcoord  : TEXCOORD0;

};

// Vertex Shader Output Struct
struct VS_OUTPUT 
{
	float4 	Position   : POSITION0;
	float2 	Texcoord   : TEXCOORD0;
};

// Pixel Shader Input Struct
struct PS_INPUT
{
	float2 	TexCoord  : TEXCOORD0;   
};

// Pixel Shader Output Struct
struct PS_OUTPUT
{
	float4 PixelColor : COLOR0;
};

// Vertex Shader Main
VS_OUTPUT vs_main( VS_INPUT Input )
{
	VS_OUTPUT Output;
	
	Output.Position = float4(sign(Input.Position).xy, 0.0f, 1.0f);
	Output.Texcoord.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
	Output.Texcoord.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);

	return Output;
}


float3 RGBtoHSL(float3 RGBColor)
{
	float Hue = 0.0f;
	float Saturation = 0.0f;
	float Lightness = 0.0f;
   
	float Max = MAX(RGBColor);
	float Min = MIN(RGBColor);

	float MaxMinSum = Max + Min;
	float MaxMinDiff = Max - Min;

	// Lightness
	Lightness = (MaxMinSum) / 2.0f;
   
	// Saturation and Hue
	[branch]
	if (Max == Min)
	{
		Hue = 0.0f;
		Saturation = 0.0f;
	}
	else // Chromatic
	{
		[branch]
		if (Lightness <= 0.5f)
			Saturation = (MaxMinDiff) / (MaxMinSum);
		else
			Saturation = (MaxMinDiff) / (2.0f - MaxMinDiff);

		// Hue
		float3 Chrom; //Chrom = CrCgCb
		Chrom = (Max - RGBColor) / MaxMinDiff;
   
		[branch]
		if (RGBColor.r == Max)
			Hue = Chrom.b - Chrom.g;
		else if (RGBColor.g == Max)
			Hue = 2.0f + Chrom.r - Chrom.b;
		else //(RGBColor.b == Max)
			Hue = 4.0f + Chrom.g - Chrom.r;
   
		Hue *= 60.0f;

		if (Hue < 0.0f)
			Hue += 360.0f;
	}

	return float3(Hue, Saturation, Lightness);
}

float3 HSLtoRGB(float3 HSLColor)
{
	float Hue = HSLColor.x;
	float Saturation = HSLColor.y;
	float Lightness = HSLColor.z;

	float M1, M2;
	float3 Color = 0.0f;

	[branch]
	if (Lightness < 0.5f)
		M2 = Lightness * (1.0f + Saturation);
	else
		M2 = (Lightness + Saturation) - Lightness * Saturation;
   
	M1 = 2.0f * Lightness - M2;
   
	float M2M1Diff = M2 - M1;

	[branch]
	if (Saturation == 0.0f)
	{
		Color.r = Lightness;
		Color.g = Lightness;
		Color.b = Lightness;
	}
	else
	{
		float h = Hue + 120.0f;
		if (h > 360.0f)
			h = h - 360.0f;
      
		// Red
		[branch]
		if (h  <  60.0f)
			Color.r = M1 + ( M2M1Diff ) * h / 60.0f;
		else if (h < 180.0f)
			Color.r = M2;
		else if (h < 240.0f)
			Color.r = M1 + ( M2M1Diff ) * ( 240.0f - h ) / 60.0f;
		else
			Color.r = M1;
         
		h = Hue;
         
		// Green
		[branch]
		if (h < 60.0f)
			Color.g = M1 + ( M2M1Diff ) * h / 60.0f;
		else if (h < 180.0f)
			Color.g = M2;
		else if (h < 240.0f)
			Color.g = M1 + ( M2M1Diff ) * ( 240.0f - h ) / 60.0f;
		else
			Color.g  = M1;
      
		h = Hue - 120;
		if (h < 0.0f)
			h += 360.0f;
         
		// Blue
		[branch]
		if (h  <  60.0f)
			Color.b = M1 + ( M2M1Diff ) * h / 60.0f;
		else if (h < 180.0f)
			Color.b = M2;
		else if (h < 240.0f)
			Color.b = M1 + ( M2M1Diff ) * ( 240.0f - h ) / 60.0f;
		else
			Color.b = M1;
	}
  
	return Color; 
}

PS_OUTPUT ps_main( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;
	
	// Transform Color
	float4 SampleColor = saturate(tex2D(TextureInput, Input.TexCoord));
	float4 TransformedColor = mul(ColorTransformMatrix, float4(SampleColor.xyz, 1.0f));

	// Convert to HSL
	float3 HSL = RGBtoHSL(TransformedColor.rgb);

	// Add values
	// Do not modify lightness at this point
	// Do not let gray pixels gain saturation as mush as color pixels
	HSL += float3(HueBoost, HSL.y * SaturationBoost, 0.0f);

	// Limit values
	float4 ClampedHSL;
	HSL.x = fmod(HSL.x, 360.0f);
	HSL.yz = clamp(HSL.yz, 0.0f, 1.0f);
	
	// Convert back to rgb
	float3 RGB = HSLtoRGB(HSL);
	
	// Adjust intensity
	RGB += IntensityBoost;
	
	Output.PixelColor.a = SampleColor.a;
	Output.PixelColor.rgb = lerp(SampleColor.rgb, RGB, BlendFactor);

	return Output;
}
