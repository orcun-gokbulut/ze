//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - MLAAProcessor.hlsl
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
sampler2D 	ColorBuffer		: register(s3);
sampler2D	EdgeBuffer		: register(s4);
sampler2D	WeightBuffer	: register(s5);
sampler2D	AreaBuffer		: register(s6);

// Vertex shader parameters
float2 		PixelSize		: register(vs, c0)
							: register(ps, c0);

// Pixel shader paramters
float2		MLAAParameters	: register(ps, c1);


#define		Treshold		MLAAParameters.x
#define		FarZ			MLAAParameters.y

#define		SEARCH_STEP		8
#define		NUM_DISTANCES	32.0f
#define		AREA_SIZE (NUM_DISTANCES * 5.0f)


/************************************************************************/
/*					        IO Structures			                    */
/************************************************************************/

// Vertex Shader
struct VS_INPUT
{
	float3 Position : POSITION0;
	
};

struct VS_OUTPUT 
{
	float4 Position		: POSITION0;
	float2 TextureCoord	: TEXCOORD0;

};

// Pixel Shader 
struct PS_INPUT
{
	float2 TextureCoord	: TEXCOORD0;
	
};

struct PS_OUTPUT
{
	float4 PixelColor : COLOR0;

};


/************************************************************************/
/*                Common Vertex Shader for All Passes	                */
/************************************************************************/

VS_OUTPUT vs_main_common(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0.0f;
   
	Output.Position	= float4(sign(Input.Position).xyz, 1.0f);
	
	Output.TextureCoord.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
	Output.TextureCoord.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);
	
	return Output;
}

/************************************************************************/
/*                   MLAA Shader Helper Functions	                    */
/************************************************************************/

float4 MultiplyAdd(float4 M, float4 A, float4 B)
{
	return M * A + B;
}

float2 Area(float2 Distance, float E1, float E2)
{
	float2 PixelCoord = NUM_DISTANCES * round(4.0f * float2(E1, E2)) + Distance;
	float2 TextureCoord = PixelCoord / (AREA_SIZE - 1.0f);

	return tex2Dlod(AreaBuffer, float4(TextureCoord.x, TextureCoord.y, 0.0f, 0.0f)).rg;
}

float SearchLeft(float2 TextureCoord)
{
	float E = 0.0f;
	TextureCoord -= float2(1.5f, 0.0f) * PixelSize;

	int I = 0;
	[unroll(SEARCH_STEP)]
    while (I < SEARCH_STEP)
	{
		E = tex2Dlod(EdgeBuffer, float4(TextureCoord.x, TextureCoord.y, 0.0f, 0.0f)).g;
		[flatten] if (E < 0.9f) break;
		TextureCoord -= float2(2.0f, 0.0f) * PixelSize;
		I++;
    }
    
    return max(-2.0f * I - 2.0f * E, -2.0f * SEARCH_STEP);
}

float SearchRight(float2 TextureCoord)
{
	float E = 0.0f;
	TextureCoord += float2(1.5f, 0.0f) * PixelSize;

	int I = 0;
	[unroll(SEARCH_STEP)]
    while (I < SEARCH_STEP)
	{
		E = tex2Dlod(EdgeBuffer, float4(TextureCoord.x, TextureCoord.y, 0.0f, 0.0f)).g;
		[flatten] if (E < 0.9f) break;
		TextureCoord += float2(2.0f, 0.0f) * PixelSize;
		I++;
    }
    
    return min(2.0f * I + 2.0f * E, 2.0f * SEARCH_STEP);
}

float SearchUp(float2 TextureCoord)
{
	TextureCoord -= float2(0.0f, 1.5f) * PixelSize;
	float E = 0.0f;

	int I = 0;
	[unroll(SEARCH_STEP)]
    while (I < SEARCH_STEP)
	{
		E = tex2Dlod(EdgeBuffer, float4(TextureCoord.x, TextureCoord.y, 0.0f, 0.0f)).r;
		[flatten] if (E < 0.9f) break;
		TextureCoord -= float2(0.0f, 2.0f) * PixelSize;
		I++;
    }
    
    return max(-2.0f * I - 2.0f * E, -2.0f * SEARCH_STEP);
}

float SearchDown(float2 TextureCoord)
{
	TextureCoord += float2(0.0f, 1.5f) * PixelSize;
	float E = 0.0f;

	int I = 0;
	[unroll(SEARCH_STEP)]
    while (I < SEARCH_STEP)
	{
		E = tex2Dlod(EdgeBuffer, float4(TextureCoord.x, TextureCoord.y, 0.0f, 0.0f)).r;
		[flatten] if (E < 0.9f) break;
		TextureCoord += float2(2.0f, 0.0f) * PixelSize;
		I++;
    }
    
    return min(2.0f * I + 2.0f * E, 2.0f * SEARCH_STEP);
}

/************************************************************************/
/*              Pixel Shader for Edge Detection                         */
/************************************************************************/

PS_OUTPUT ps_main_edge_detection_depth(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	// Get Depths
	float PixelDepthCenter	= (float4)1.0f - ZEGBuffer_GetDepth(Input.TextureCoord) / FarZ;
	float PixelDepthUp		= ZEGBuffer_GetDepth(Input.TextureCoord - float2(0.0f, PixelSize.y));
	float PixelDepthDown	= ZEGBuffer_GetDepth(Input.TextureCoord + float2(0.0f, PixelSize.y));
	float PixelDepthLeft	= ZEGBuffer_GetDepth(Input.TextureCoord - float2(PixelSize.x, 0.0f));
	float PixelDepthRight	= ZEGBuffer_GetDepth(Input.TextureCoord + float2(PixelSize.x, 0.0f));
	
	float4 Depths = (float4)1.0f - float4(PixelDepthLeft, PixelDepthUp, PixelDepthRight, PixelDepthDown) / FarZ;
	float4 DepthGradient = abs(PixelDepthCenter.rrrr - Depths);

	// Get Normals
	float3 PixelNormCenter = ZEGBuffer_GetViewNormal(Input.TextureCoord);

	float3 PixelNormUp		= ZEGBuffer_GetViewNormal(Input.TextureCoord - float2(0.0f, PixelSize.y));
	float3 PixelNormDown	= ZEGBuffer_GetViewNormal(Input.TextureCoord + float2(0.0f, PixelSize.y));
	float3 PixelNormLeft	= ZEGBuffer_GetViewNormal(Input.TextureCoord - float2(PixelSize.x, 0.0f));
	float3 PixelNormRight	= ZEGBuffer_GetViewNormal(Input.TextureCoord + float2(PixelSize.x, 0.0f));

	float4 AngleGradient = (float4)0.0f;
	AngleGradient.x = dot(PixelNormCenter, PixelNormLeft);
	AngleGradient.y = dot(PixelNormCenter, PixelNormUp);
	AngleGradient.z = dot(PixelNormCenter, PixelNormRight);
	AngleGradient.w = dot(PixelNormCenter, PixelNormDown);
	
	// Distance based angle treshold
	float AngleTreshold = (1.0f - PixelDepthCenter) * 0.7f + 0.05f;
	
	// Reverse map 0-180 degrees[-1 to +1] to 0-1
	AngleGradient = 1.0f - (AngleGradient * 0.5f + 0.5f);
		
	float4 Angles = step(AngleTreshold.xxxx, AngleGradient);
	float4 Depth = step(Treshold.xxxx / 10.0f, DepthGradient);
	
	// Combine angle and depth edge detection results
	Output.PixelColor = (Angles < 1.0f) ? 0.0f : 1.0f; 
	Output.PixelColor = (Depth < 1.0f) ? Output.PixelColor : 1.0f;
	
	// Output.PixelColor = (Angles + Depth) * 0.5f;
	// Output.PixelColor = step(float4(0.4f, 0.4f, 0.4f, 0.4f), Output.PixelColor);


	if (dot(Output.PixelColor, 1.0f) == 0.0f) discard;

	Output.PixelColor.b = 0.0f;
	Output.PixelColor.a = 0.0f;
	return Output;
}

/************************************************************************/
/*                 Pixel Shader for Weight Blending	                    */
/************************************************************************/

PS_OUTPUT ps_main_weight_blending(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	float2 E = tex2Dlod(EdgeBuffer, float4(Input.TextureCoord.x, Input.TextureCoord.y, 0.0f, 0.0f)).rg;

	[branch]
    if (E.g) // Edge at north
	{
        float2 D = float2(SearchLeft(Input.TextureCoord), SearchRight(Input.TextureCoord));
        
        float4 Coords = MultiplyAdd(float4(D.x, -0.25f, D.y + 1.0f, -0.25f), PixelSize.xyxy, Input.TextureCoord.xyxy);
        float E1 = tex2Dlod(EdgeBuffer, float4(Coords.x, Coords.y, 0.0f, 0.0f)).r;
        float E2 = tex2Dlod(EdgeBuffer, float4(Coords.z, Coords.w, 0.0f, 0.0f)).r;
        
		Output.PixelColor.rg = Area(abs(D), E1, E2);
    }

	[branch]
    if (E.r) // Edge at west
	{
        float2 D = float2(SearchUp(Input.TextureCoord), SearchDown(Input.TextureCoord));

        float4 Coords = MultiplyAdd(float4(-0.25, D.x, -0.25, D.y + 1.0), PixelSize.xyxy, Input.TextureCoord.xyxy);
		float E1 = tex2Dlod(EdgeBuffer, float4(Coords.x, Coords.y, 0.0f, 0.0f)).g;
        float E2 = tex2Dlod(EdgeBuffer, float4(Coords.z, Coords.w, 0.0f, 0.0f)).g;
        
		Output.PixelColor.ba = Area(abs(D), E1, E2);
    }

	return Output;
}

/************************************************************************/
/*                Pixel Shader for Color Blending                       */
/************************************************************************/
PS_OUTPUT ps_main_color_blending(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;
	
	float4 UpLeft	= tex2D(WeightBuffer, Input.TextureCoord);
	float  Right	= tex2D(WeightBuffer, Input.TextureCoord + float2(0.0f, PixelSize.y)).g;
	float  Down		= tex2D(WeightBuffer, Input.TextureCoord + float2(PixelSize.x, 0.0f)).a;
	
	float4 A		= float4(UpLeft.r, Right, UpLeft.b, Down);
	float Sum	= dot(A, float4(1.0f, 1.0f, 1.0f, 1.0f));

	float4 TexCoord = float4(Input.TextureCoord.x, Input.TextureCoord.y, 0.0f, 0.0f);

	[branch]
    if (Sum > 0.001f) 
	{
        float4 O = A * PixelSize.yyxx;
     
        Output.PixelColor = MultiplyAdd(tex2Dlod(ColorBuffer, TexCoord + float4( 0.0f, -O.r , 0.0f, 0.0f)), A.r, Output.PixelColor);
        Output.PixelColor = MultiplyAdd(tex2Dlod(ColorBuffer, TexCoord + float4( 0.0f,  O.g , 0.0f, 0.0f)), A.g, Output.PixelColor);
        Output.PixelColor = MultiplyAdd(tex2Dlod(ColorBuffer, TexCoord + float4(-O.b ,  0.0f, 0.0f, 0.0f)), A.b, Output.PixelColor);
        Output.PixelColor = MultiplyAdd(tex2Dlod(ColorBuffer, TexCoord + float4( O.a ,  0.0f, 0.0f, 0.0f)), A.a, Output.PixelColor);
        
		Output.PixelColor /= Sum;
    }
	else 
	{
        Output.PixelColor = tex2Dlod(ColorBuffer, TexCoord);
    }

	return Output;
}



