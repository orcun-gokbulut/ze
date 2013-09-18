//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - LBuffer.hlsl
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
SamplerState	LBufferPointSampler : register(s3);

// Textures
Texture2D<float4>	LBuffer1 : register(t3);
//Texture2D<float4>	LBuffer2 : register(t4);

#define	ZE_LIGHT_ZERO_TRESOLD		0.01f
#define	ZE_LIGHT_ZERO_CHECK(Value)	((abs( (Value) ) - 0.01f) < (0.0f))

struct ZELBuffer
{
	float4 DiffuseSpecular	: SV_TARGET0;
	//float4 NotDecidedYet	: SV_TARGET1;
};

void ZELBuffer_SetDiffuse(inout ZELBuffer LBuffer, in float3 Diffuse)
{
	LBuffer.DiffuseSpecular.xyz = Diffuse;
}

float3 ZELBuffer_GetDiffuse(in float2 Texcoord)
{
	return LBuffer1.Sample(LBufferPointSampler, Texcoord).rgb;
}

void ZELBuffer_SetSpecular(inout ZELBuffer LBuffer, in float Specular)
{
	LBuffer.DiffuseSpecular.a = Specular;
}

float3 ZELBuffer_GetSpecular(in float2 Texcoord)
{
	float4 Sample = LBuffer1.Sample(LBufferPointSampler, Texcoord);
	return max(normalize(Sample.rgb) * Sample.a, 0.0f);
}

float ZELBuffer_GetLuminance(in float3 Color)
{
	return dot(Color, float3(0.299f, 0.587f, 0.114f));
}
