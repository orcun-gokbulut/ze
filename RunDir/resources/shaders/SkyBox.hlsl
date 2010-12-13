//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SkyBox.hlsl
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

// Transformation matrices 5 matrices
float4x4 WorldViewProjMatrix : register(vs, c0);
float4x4 WorldMatrix : register(vs, c4);
float4x4 WorldInvTrpsMatrix : register(vs, c8);
float4x4 WorldInvMatrix : register(vs, c12);

// Other general constants 4 vectors
float4 ViewPosition : register(vs, c16);
float MaterialRefractionIndex : register(vs, c17);

float4 SkyColor : register(ps, c0);
sampler SkyTexture : register(ps, s0);

struct VSInput 
{
	float4 Position				: POSITION0;
};

struct VSOutput 
{
	float4 Position				: POSITION0;
	float3 CubeTexcoord			: TEXCOORD0;
};

VSOutput vs_main(VSInput Input)
{
	VSOutput Output;

	Output.Position = Input.Position;
	Output.CubeTexcoord = mul(Input.Position, WorldViewProjMatrix);
	
	return Output;
}

struct PSInput
{
	float3 CubeTexcoord       : TEXCOORD0;
};

half4 ps_main(PSInput Input) : COLOR0
{
	return SkyColor * texCUBE(SkyTexture, normalize(Input.CubeTexcoord));
}
