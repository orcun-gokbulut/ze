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
#define		HueFactor				PSParameters.x
#define		LightnessFactor			PSParameters.y
#define		SaturationFactor		PSParameters.z
#define		BlendFactor				PSParameters.w

// Definitions
#define ZERO_TRESHOLD   0.001f

#define MAX(Vector3)	(max(max( (Vector3.x), (Vector3.y) ), (Vector3.z) ))
#define MIN(Vector3)	(min(min( (Vector3.x), (Vector3.y) ), (Vector3.z) ))

#define EQUAL(Scalar1, Scalar2)   (abs((Scalar1) - (Scalar2)) < (ZERO_TRESHOLD))


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


float3 RGBtoHSL(float3 RGB)
{
   float Hue;
   float Lightness;
   float Saturation;
   
   float Max = MAX(RGB);
   float Min = MIN(RGB);

   // Lightness
   Lightness = (Max + Min) / 2.0f;
   
   // Saturation
   if (EQUAL(Max, Min))
   {
      // Achromatic
      Hue = 0.0f;
      Saturation = 0.0f;

	  // Done
	  return float3(Hue, Saturation, Lightness);
   }
   else
   {
      // Chromatic
      if (Lightness <= 0.5f)
         Saturation = (Max - Min) / (Max + Min);
      else
         Saturation = (Max - Min) / (2.0f - Max - Min);
   }
   
   // Hue
   float Cr = (Max - RGB.r) / (Max - Min);	// division can be optimizable
   float Cg = (Max - RGB.g) / (Max - Min);
   float Cb = (Max - RGB.b) / (Max - Min);
   
   if (EQUAL(RGB.r, Max))
      Hue = Cb - Cg;
   if (EQUAL(RGB.g, Max))
      Hue = 2.0f + Cr - Cb;
   if (EQUAL(RGB.b, Max))
      Hue = 4.0f + Cg - Cr;
   
   Hue *= 60.0f;

   if (Hue < 0.0f)
      Hue += 360;
   
   return float3(Hue, Saturation, Lightness);
}

float3 HSLtoRGB(float3 HSL)
{
   float Hue = HSL.x;
   float Saturation = HSL.y;
   float Lightness = HSL.z;
   
   float Red, Green, Blue;

   float M1, M2;
  
   if (Lightness < 0.5f)
      M2 = Lightness * (1.0f + Saturation);
   else
      M2 = Lightness + Saturation - Lightness * Saturation;
   
   M1 = 2.0f * Lightness - M2;
   
   float M2M1Diff = M2 - M1;

   
   if (EQUAL(Saturation, 0.0f))
   {
      Red = Lightness;
      Green = Lightness;
      Blue = Lightness;

	  // Done
	  return float3(Red, Green, Blue); 
   }
   else
   {
      float h = Hue + 120.0f;
      if (h > 360.0f)
         h = h - 360.0f;
      
      // Red
      if (h  <  60.0f)
         Red = M1 + ( M2M1Diff ) * h / 60.0f;
      else if (h < 180.0f)
         Red = M2;
      else if (h < 240.0f)
         Red = M1 + ( M2M1Diff ) * ( 240.0f - h ) / 60.0f;
      else
         Red = M1;
         
      h = Hue;
         
      // Green
      if (h < 60.0f)
         Green = M1 + ( M2M1Diff ) * h / 60.0f;
      else if (h < 180.0f)
         Green = M2;
      else if (h < 240.0f)
         Green = M1 + ( M2M1Diff ) * ( 240.0f - h ) / 60.0f;
      else
         Green  = M1;
      
      h = Hue - 120;
      if (h < 0.0f)
         h += 360.0f;
         
      // Blue
      if (h  <  60.0f)
         Blue = M1 + ( M2M1Diff ) * h / 60.0f;
      else if (h < 180.0f)
         Blue = M2;
      else if (h < 240.0f)
         Blue = M1 + ( M2M1Diff ) * ( 240.0f - h ) / 60.0f;
      else
         Blue = M1;
   }
   
   return float3(Red, Green, Blue); 
}

PS_OUTPUT ps_main( PS_INPUT Input )
{
	PS_OUTPUT Output;
	Output.PixelColor = (float4)0.0f;
	
	// Transform Color
	float4 SampleColor = tex2D(TextureInput, Input.TexCoord);
	float4 TransformedColor = mul(ColorTransformMatrix, float4(SampleColor.xyz, 1.0f));
	
	// Adjust H, S, L
	float3 HSL = RGBtoHSL(TransformedColor.rgb);
	
	// Let hue loop between 0 - 360
	HSL.x += HueFactor;	
	HSL.x = fmod(HSL.x, 360.0f);

	// Clamp Saturation and lightness 0 - 1 range
	HSL.yz *= float2(SaturationFactor, LightnessFactor);
	HSL.yz = saturate(HSL.yz);

	float3 RGB = HSLtoRGB(HSL);

	Output.PixelColor.a = SampleColor.a;
	Output.PixelColor.rgb = lerp(SampleColor.rgb, RGB, BlendFactor);
	
	return Output;
}
