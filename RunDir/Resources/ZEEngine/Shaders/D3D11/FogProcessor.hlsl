//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - FogProcessor.hlsl
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

// Includes
#include "GBuffer.hlsl"

// Samplers
sampler2D 	ColorBuffer		: register (s4);

// Vertex shader parameters
float2 		PixelSize		: register(vs, c0);

// Pixel shader paramters
float4		FogParameters0	: register(ps, c0);
float4		FogParameters1	: register(ps, c1);
float3		FogParameters2	: register(ps, c2);
float3		FogParameters3	: register(ps, c3);

// Parameter definitions
#define		FogColor				FogParameters0.xyz
#define		FarZ					FogParameters0.w

#define		FogDistanceFar			FogParameters1.x
#define		FogDistanceNear			FogParameters1.y
#define		FogHeight				FogParameters1.z
#define		FogVisibility			FogParameters1.w

#define		OutScatterFactor		FogParameters2.xyz
#define		InScatterFactor			FogParameters3.xyz



// Vertex Shader
struct VS_INPUT
{
	float3 Position  : POSITION0;
	
};

struct VS_OUTPUT 
{
	float4 Position		: POSITION0;
	float2 TexCoord		: TEXCOORD1;

};

VS_OUTPUT vs_main_common(VS_INPUT Input)
{
	VS_OUTPUT Output	= (VS_OUTPUT)0.0f;
   
	Output.Position		= float4(sign(Input.Position).xyz, 1.0f);
	Output.TexCoord.x	= 0.5f * (1.0f + Output.Position.x + PixelSize.x);
	Output.TexCoord.y	= 0.5f * (1.0f - Output.Position.y + PixelSize.y);
	
	return Output;
}

// Pixel Shader 
struct PS_INPUT
{
	float2 TexCoord		: TEXCOORD1;
	
};

struct PS_OUTPUT
{
	float4 PixelColor	: COLOR0;

};

PS_OUTPUT ps_main_linear(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;
	
	// Get samples
	float PixelDepth = ZEGBuffer_GetDepth(Input.TexCoord);
	float4 PixelColor = tex2D(ColorBuffer, Input.TexCoord);
	
	// Sky dome depth is 0, make it equal to FarZ to apply fog
	if (0.0f <= PixelDepth && PixelDepth <= 0.0001f)
		PixelDepth = FarZ;
	
	// Do not fog FogDistanceNear
	if (FogDistanceNear > PixelDepth)
		PixelDepth = 0.0000f;
	else
		PixelDepth -= FogDistanceNear;
	
	// Linear fog calculation
	float FogAmount = (PixelDepth - FogDistanceNear) / (FogDistanceFar - FogDistanceNear);
	FogAmount = clamp(FogAmount, 0.0f, 1.0f);

    float3 FinalColor = lerp(PixelColor.rgb, FogColor, FogAmount);
	
	// Give output based on Visibility
	Output.PixelColor.xyz = lerp(PixelColor, FinalColor, FogVisibility);
    
    return Output;
}

PS_OUTPUT ps_main_exponential(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;
	
	// Get samples
	float PixelDepth = ZEGBuffer_GetDepth(Input.TexCoord);
	float4 PixelColor = tex2D(ColorBuffer, Input.TexCoord);
	
	// Sky dome depth is 0, make it equal to FarZ to apply fog
	if (0.0f <= PixelDepth && PixelDepth <= 0.0001f)
		PixelDepth = FarZ;
	
	// Do not fog FogDistanceNear
	if (FogDistanceNear > PixelDepth)
		PixelDepth = 0.0000f;
	else
		PixelDepth -= FogDistanceNear;
	
	// Calculate how much the fog and pixel color will be scattered due to distance
	float3 OutScatterRatio = exp(-1.0f * PixelDepth * OutScatterFactor);
	float3 InScatterRatio = exp(-1.0f * PixelDepth * InScatterFactor);
	
	// Calculate final color of fog and pixel
	float3 PixColAftOutScat = PixelColor.xyz * OutScatterRatio;
	float3 FogColAftInScat = FogColor * (1.0f - InScatterRatio);
	
	// Calculate final color
	float3 FinalColor = PixColAftOutScat + FogColAftInScat;
	
	// Give output based on Visibility
	Output.PixelColor.xyz = lerp(PixelColor, FinalColor, FogVisibility);
	
	return Output;
}

PS_OUTPUT ps_main_exponential_squared(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;
	
	// Get samples
	float PixelDepth = ZEGBuffer_GetDepth(Input.TexCoord);
	float4 PixelColor = tex2D(ColorBuffer, Input.TexCoord);
	
	// Sky dome depth is 0, make it equal to FarZ to apply fog
	if (0.0f <= PixelDepth && PixelDepth <= 0.0001f)
		PixelDepth = FarZ;
	
	// Do not fog FogDistanceNear
	if (FogDistanceNear > PixelDepth)
		PixelDepth = 0.0000f;
	else
		PixelDepth -= FogDistanceNear;
	
	// Calculate how much the fog and pixel color will be scattered due to distance
	float3 OutScatterRatio = exp(-1.0f * pow(PixelDepth * OutScatterFactor, 2.0f));
	float3 InScatterRatio = exp(-1.0f * pow(PixelDepth * InScatterFactor, 2.0f));
	
	// Calculate final color of fog and pixel
	float3 PixColAftOutScat = PixelColor.xyz * OutScatterRatio;
	float3 FogColAftInScat = FogColor * (1.0f - InScatterRatio);
	
	// Calculate final color
	float3 FinalColor = PixColAftOutScat + FogColAftInScat;
	
	// Give output based on Visibility
	Output.PixelColor.xyz = lerp(PixelColor, FinalColor, FogVisibility);
	
	return Output;
}
