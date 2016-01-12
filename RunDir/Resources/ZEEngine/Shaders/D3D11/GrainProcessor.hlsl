//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - GrainProcessor.hlsl
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

sampler2D 	ColorBuffer		: register (s0);
sampler2D 	GrainBuffer		: register (s1);

float2 		VSParameters	: register(vs, c0);

#define		PixelSize		VSParameters.xy

float4 		PSParameters	: register(ps, c0);

#define		PI				3.1415f
#define		Time			PSParameters.x
#define		GrainStrength	PSParameters.y


// Vertex Shader
struct VS_INPUT
{
	float4 Position	: POSITION0;
	
};

struct VS_OUTPUT 
{
	float4 Position	: POSITION0;
	float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
	float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 PixelColor : COLOR0;
};

VS_OUTPUT vs_main_common(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0.0f;
   
	Output.Position	= sign(Input.Position);

	Output.TexCoord.x = 0.5f * (1.0f + Output.Position.x + PixelSize.x);
	Output.TexCoord.y = 0.5f * (1.0f - Output.Position.y + PixelSize.y);
	
	return Output;
}


float Random(float2 Vector)
{
	const float4 Temp = float4( 97.409091f, 148.413159f, 56.205410f, 44.687805f );
	float4 Result = float4(Vector,Vector);
  
	for(int i = 0; i < 3; i++)
	{
		Result.x = frac( dot(Result, Temp) );
		Result.y = frac( dot(Result, Temp) );
		Result.z = frac( dot(Result, Temp) );
		Result.w = frac( dot(Result, Temp) );
	}
  
	return (float)Result.xy;
}

PS_OUTPUT ps_main_grain( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)1.0f;

	float Rand = Random(Input.TexCoord + Time) * GrainStrength; 
	float3 Grain = float3(Rand, Rand, Rand);

	Output.PixelColor.rgb *= 1.0f - Grain;
	Output.PixelColor.rgb *= (1.0f + GrainStrength / 2.0f);
	
	return Output;
}


PS_OUTPUT ps_main_blend( PS_INPUT Input )
{
	PS_OUTPUT Output = (PS_OUTPUT)0.0f;
	
	float4 Grain = tex2D(GrainBuffer, Input.TexCoord);
	Output.PixelColor = tex2D(ColorBuffer, Input.TexCoord);
	
	[branch]
	if (Grain.x <= 0.999f)
		Output.PixelColor *= Grain;
	
	return Output;
}
