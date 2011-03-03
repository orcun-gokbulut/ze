//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SkyBoxMaterial.hlsl
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

samplerCUBE SkyTexture : register(s5);

float4x4 WorldViewProjMatrix: register(c0);
float4x4 WorldViewMatrix : register(c4);

float3 SkyColor : register(c10);

struct VSInput 
{
	float4 Position : POSITION0;
};

struct VSOutput 
{
	float4 Position : POSITION0;
	float3 CubeTexcoord : TEXCOORD0;
};

VSOutput VSMain(VSInput Input)
{
	VSOutput Output;

	Output.Position = mul(Input.Position, WorldViewProjMatrix).xyzz;
	Output.CubeTexcoord = Input.Position.xyz;
	return Output;
}

struct PSInput
{
	float3 CubeTexcoord : TEXCOORD0;
};

float4 PSMain(PSInput Input) : COLOR0
{
	return float4(SkyColor * texCUBE(SkyTexture, Input.CubeTexcoord), 1.0f);
}
