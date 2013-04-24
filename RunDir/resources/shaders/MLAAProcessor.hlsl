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

#include "GBuffer.hlsl"

// Samplers
sampler2D 	ColorBuffer		: register(s3);
sampler2D	EdgeBuffer		: register(s4);
sampler2D	WeightBuffer	: register(s5);
sampler2D	AreaBuffer		: register(s6);

// Vertex shader parameters
float2 		PixelSize		: register(c0);

// Pixel shader paramters
float4		MLAAParameters	: register(ps, c1);


#define		Treshold		MLAAParameters.x

#define		SEARCH_STEP		12.0f
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

	return tex2Dlod(AreaBuffer, float4(TextureCoord.xy, 0.0f, 0.0f)).rg;
}

float SearchLeft(float2 TextureCoord)
{
	float E = 0.0f;
	TextureCoord -= float2(1.5f, 0.0f) * PixelSize;

	int I = 0;
    while (I < SEARCH_STEP)
	{
		E = tex2Dlod(EdgeBuffer, float4(TextureCoord, 0.0f, 0.0f)).g;
		
		[flatten] 
		if (E < 0.9f) 
			break;
		
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
    while (I < SEARCH_STEP)
	{
		E = tex2Dlod(EdgeBuffer, float4(TextureCoord, 0.0f, 0.0f)).g;
		
		[flatten]
		if (E < 0.9f)
			break;
		
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
    while (I < SEARCH_STEP)
	{
		E = tex2Dlod(EdgeBuffer, float4(TextureCoord.x, TextureCoord.y, 0.0f, 0.0f)).r;
		
		[flatten]
		if (E < 0.9f)
			break;
		
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
    while (I < SEARCH_STEP)
	{
		E = tex2Dlod(EdgeBuffer, float4(TextureCoord.x, TextureCoord.y, 0.0f, 0.0f)).r;
		
		[flatten]
		if (E < 0.9f)
			break;
		
		TextureCoord += float2(2.0f, 0.0f) * PixelSize;
		I++;
    }
    
    return min(2.0f * I + 2.0f * E, 2.0f * SEARCH_STEP);
}

/************************************************************************/
/*              Pixel Shader for Edge Detection                         */
/************************************************************************/

static const float3 Weights = float3(0.2126f, 0.7152f, 0.0722f);

PS_OUTPUT ps_main_edge_detection_color(PS_INPUT Input)
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;

	float4 LeftOffset	= float4(Input.TextureCoord - float2(PixelSize.x, 0.0f), 0.0f, 0.0f);
	float4 TopOffset	= float4(Input.TextureCoord - float2(0.0f, PixelSize.y), 0.0f, 0.0f);
	float4 RightOffset	= float4(Input.TextureCoord + float2(PixelSize.x, 0.0f), 0.0f, 0.0f);
	float4 BottomOffset	= float4(Input.TextureCoord + float2(0.0f, PixelSize.y), 0.0f, 0.0f);
	float4 CenterOffset = float4(Input.TextureCoord, 0.0f, 0.0f);

	float LeftColor		= dot(tex2Dlod(ColorBuffer, LeftOffset).rgb, Weights);
	float TopColor		= dot(tex2Dlod(ColorBuffer, TopOffset).rgb, Weights);
	float RightColor	= dot(tex2Dlod(ColorBuffer, RightOffset).rgb, Weights);
	float BottomColor	= dot(tex2Dlod(ColorBuffer, BottomOffset).rgb, Weights);
	float CenterColor	= dot(tex2Dlod(ColorBuffer, CenterOffset).rgb, Weights);

	float4	Discontinuities = abs(CenterColor.xxxx - float4(LeftColor, TopColor, RightColor, BottomColor));
	float4	Detected = step(Treshold.xxxx, Discontinuities);

	if (dot(Detected, 1.0f) == 0.0f)
		discard;

	Output.PixelColor = Detected;

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

	float2 E = tex2Dlod(EdgeBuffer, float4(Input.TextureCoord, 0.0f, 0.0f)).rg;

	[branch]
    if (E.g) // Edge at north
	{
        float2 D = float2(SearchLeft(Input.TextureCoord), SearchRight(Input.TextureCoord));
        
        float4 Coords = MultiplyAdd(float4(D.x, -0.25f, D.y + 1.0f, -0.25f), PixelSize.xyxy, Input.TextureCoord.xyxy);
        float E1 = tex2Dlod(EdgeBuffer, float4(Coords.xy, 0.0f, 0.0f)).r;
        float E2 = tex2Dlod(EdgeBuffer, float4(Coords.zw, 0.0f, 0.0f)).r;
        
		Output.PixelColor.rg = Area(abs(D), E1, E2);
    }

	[branch]
    if (E.r) // Edge at west
	{
        float2 D = float2(SearchUp(Input.TextureCoord), SearchDown(Input.TextureCoord));

        float4 Coords = MultiplyAdd(float4(-0.25, D.x, -0.25, D.y + 1.0), PixelSize.xyxy, Input.TextureCoord.xyxy);
		float E1 = tex2Dlod(EdgeBuffer, float4(Coords.xy, 0.0f, 0.0f)).g;
        float E2 = tex2Dlod(EdgeBuffer, float4(Coords.zw, 0.0f, 0.0f)).g;
        
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
