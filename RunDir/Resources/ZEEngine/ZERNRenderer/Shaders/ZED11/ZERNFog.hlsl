//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNFog.hlsl
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

#ifndef __ZERN_FOG_H__
#define __ZERN_FOG_H__

#include "ZERNGBuffer.hlsl"
#include "ZERNScreenCover.hlsl"
#include "ZERNTransformations.hlsl"

cbuffer ZERNFog_Constants	: register(b8)
{
	float	ZERNFog_Density;
	float	ZERNFog_Range;
	float2	ZERNFog_Reserved0;
	
	float3	ZERNFog_Color;
	float	ZERNFog_Reserved1;
};

float3 ZERNFog_PixelShader_Main(float4 PositionViewport : SV_Position) : SV_Target0
{	
	float2 GBufferDimensions = ZERNGBuffer_GetDimensions();
	float3 PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, GBufferDimensions, ZERNGBuffer_GetDepth(PositionViewport.xy));
	float3 PositionWorld = ZERNTransformations_ViewToWorld(float4(PositionView, 1.0f));
	
	float Distance = distance(ZERNView_Position, PositionWorld);
	Distance = max(0.0f,Distance - ZERNFog_Range);
	float FogFactor = exp(-pow(ZERNFog_Density * Distance, 2.0f));
	
	float3 PixelColor = ZERNGBuffer_GetAccumulationColor(PositionViewport.xy);
	
	return lerp(ZERNFog_Color, PixelColor, FogFactor);
}

#endif
