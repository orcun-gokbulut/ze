//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SSOAProcessor.hlsl
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

// Textures
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
sampler ColorInput				: register(s0);
sampler DepthInput				: register(s1);
sampler RandomTexture			: register(s2);

// Parameters
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float2 PixelSize			: register(c0);
float4 Parameters0			: register(c1);
#define SSAOFactor			(Parameters0[0])
#define Radius				(Parameters0[1])
#define Attenuation			(Parameters0[2])


// Kernels
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float2 DiscKernel[12]	: register(c10);


// General Vertex Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct VS_InputOutput
{
	float4		Position : POSITION0;
	float2		Texcoord : TEXCOORD0;
};

VS_InputOutput VS_Main (VS_InputOutput Input)
{
	VS_InputOutput Output;
	Output.Position = Input.Position + float4(-PixelSize.x, PixelSize.y, 0.0f, 0.0f);
	Output.Texcoord = Input.Texcoord;
	return Output;
}


float4 PS_SSAO(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float CurrentDepth = tex2D(DepthInput, Texcoord);
	
	float AmbientOcclusion = 1.0f;

	for (int I = 0; I < 12; I++)
	{
		float2 Rnd = tex2D(RandomTexture, Texcoord);

		float2 SamplePosition;
		SamplePosition.x = Rnd.x * DiscKernel[I].x - Rnd.y * DiscKernel[I].y;
		SamplePosition.x = Rnd.y * DiscKernel[I].x + Rnd.x * DiscKernel[I].y;

		float Sample = tex2D(DepthInput, SamplePosition).r;
		
		if (abs(Sample - CurrentDepth) < Radius)
			AmbientOcclusion -= 1.0f / 12.0f;
	}
	
	return AmbientOcclusion * tex2D(ColorInput, Texcoord);
}
