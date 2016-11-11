//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNResolving.hlsl
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

#ifndef __ZERN_RESOLVING_H__
#define __ZERN_RESOLVING_H__

#include "ZERNScreenCover.hlsl"
#include "ZERNGBuffer.hlsl"

#define ZERN_FILTER_NONE	0
#define	ZERN_FILTER_BOX		1

struct ZERNResolving_Output
{
	float3	GBuffer1								: SV_Target0;
	float	Depth									: SV_Depth;
};

struct ZERNResolving_GBuffers_Output
{
	float3	GBuffer1								: SV_Target0;
	float3	GBuffer2								: SV_Target1;
	float3	GBuffer3								: SV_Target2;
	float	Depth									: SV_Depth;
};

cbuffer ZERNResolving_Constants						: register(b8)
{
	uint	ZERNResolving_FilterType;
	float3	ZERNResolving_Reserved0;
};

Texture2DMS<float4, SAMPLE_COUNT>	InputTexture	: register(t5);

float4 ZERNResolving_Resolve_BoxFilter(Texture2DMS<float4, SAMPLE_COUNT> Texture, float2 PositionViewport)
{
	float4 ResultColor = 0.0f;
	
	[unroll]
	for (uint I = 0; I < SAMPLE_COUNT; I++)
		ResultColor += Texture.Load(PositionViewport, I);
	
	return ResultColor * rcp(SAMPLE_COUNT);
}

float4 ZERNResolving_Resolve_NoFilter(Texture2DMS<float4, SAMPLE_COUNT> Texture, float2 PositionViewport)
{
	return Texture.Load(PositionViewport, 0);
}

float4 ZERNResolving_ResolveCustom_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{
	float4 ResultColor = 0.0f;
	
	if (ZERNResolving_FilterType == ZERN_FILTER_NONE)
		ResultColor = ZERNResolving_Resolve_NoFilter(InputTexture, PositionViewport.xy);
	
	else if (ZERNResolving_FilterType == ZERN_FILTER_BOX)
		ResultColor = ZERNResolving_Resolve_BoxFilter(InputTexture, PositionViewport.xy);
	
	return ResultColor;
}

ZERNResolving_Output ZERNResolving_ResolveGBuffers_PixelShader_Main(float4 PositionViewport : SV_Position)
{
	ZERNResolving_Output Output = (ZERNResolving_Output)0;
	#ifdef MSAA_ENABLED
		Output.GBuffer1 = ZERNResolving_Resolve_BoxFilter(ZERNGBuffer_Buffer1, PositionViewport.xy).xyz;
		Output.Depth = ZERNResolving_Resolve_NoFilter(ZERNGBuffer_DepthBuffer, PositionViewport.xy).x;
	#endif
	return Output;
}

ZERNResolving_GBuffers_Output ZERNResolving_ResolveAllGBuffers_PixelShader_Main(float4 PositionViewport : SV_Position)
{
	ZERNResolving_GBuffers_Output Output = (ZERNResolving_GBuffers_Output)0;
	#ifdef MSAA_ENABLED
		Output.GBuffer1 = ZERNResolving_Resolve_BoxFilter(ZERNGBuffer_Buffer1, PositionViewport.xy).xyz;
		Output.GBuffer2 = ZERNResolving_Resolve_BoxFilter(ZERNGBuffer_Buffer2, PositionViewport.xy).xyz;
		Output.GBuffer3 = ZERNResolving_Resolve_NoFilter(ZERNGBuffer_Buffer3, PositionViewport.xy).xyz;
		Output.Depth = ZERNResolving_Resolve_NoFilter(ZERNGBuffer_DepthBuffer, PositionViewport.xy).x;
	#endif
	return Output;
}

#endif
