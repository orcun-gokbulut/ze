//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFiltering.hlsl
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

#ifndef __ZERN_FILTERING_H__
#define __ZERN_FILTERING_H__

#include "ZERNScreenCover.hlsl"

Texture2D Input : register(t0);

SamplerState ZERNFiltering_SamplerLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

cbuffer ZERNFiltering_Constants : register(b8)
{
	int		ZERNFiltering_KernelSize;
	int3	ZERNFiltering_Reserved;
	float4	ZERNFiltering_KernelValues[64];
};

float4 ZERNFiltering_PixelShader(float4 PositionViewport : SV_Position) : SV_Target0
{
	float4 ResultColor = {0.0f, 0.0f, 0.0f, 1.0f};
	float2 ScreenSize;
	Input.GetDimensions(ScreenSize.x, ScreenSize.y);
	float2 TexelOffset = 1.0f / ScreenSize;
	float2 TexCoord = PositionViewport.xy * TexelOffset;
	
	for(int I = 0; I < ZERNFiltering_KernelSize; ++I)
	{
		float4 SampleColor = Input.Sample(ZERNFiltering_SamplerLinear, TexCoord + ZERNFiltering_KernelValues[I].xy * TexelOffset);
		ResultColor += SampleColor * ZERNFiltering_KernelValues[I].w;
	}
	
	return ResultColor;
}

#endif
