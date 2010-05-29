//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - FixedMaterial.hlsl
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

sample ColorInput	: register(s0);
sample DepthInput	: register(s1);

float2 Paramters0	: registers(c0);

#define SAOFactor	Parameters0[0]
#define	SSAORadius	Parameters0[1]
#define	SSAOValue	Parameters0[2]


float2 Kernel[]		: registers(c10);

// General Vertex Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct VS_Input
{
	float4		Position : POSITION0;
	float2		Texcoord : TEXCOORD0;
};

struct VS_Output
{
	float4		Position : POSITION0;
	float2		Texcoord : TEXCOORD0;
};

VS_Output VS_Main (VS_Input Input)
{
	VS_Output Output;
	Output.Position = Input.Position + float4(-PixelSize.x, PixelSize.y, 0.0f, 0.0f);
	Output.Texcoord = Input.Texcoord;
	return Output;
}

// SSOA Pass Pixel Shader
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 PS_SSOA(float2 Texcoord : TEXCOORD0) : COLOR0
{
	float4 Color = tex2D(ColorInput, Texcoord);
	float OrginalDepth = tex2D(DepthInput, Texcoord).r;
	float AmbientOcclusion = 0.0f;
	for (size_t I = 0; I < 16; I++)
	{
		float DepthSample = tex2D(DepthInput, Texcoord + Kernel[I]);
		if (abs(DepthSample - OrginalDepth) < SSAORadius)
			AmbientOcclusion += SSAOValue;
	}
	
	return AmbientOcclusion * SSOAF * Color;
}
