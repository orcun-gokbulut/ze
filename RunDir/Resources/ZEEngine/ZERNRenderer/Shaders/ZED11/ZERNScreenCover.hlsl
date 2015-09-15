//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNScreenCover.hlsl
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


#ifndef __ZERN_SCREEN_COVER_H__
#define __ZERN_SCREEN_COVER_H__

float4 ZERNScreenCover_GetPosition(uint VertexIndex)
{
    float x = -1.0 + float((VertexIndex & 1) << 2);
    float y = -1.0 + float((VertexIndex & 2) << 1);
    return float4(x, y, 0.0f, 1.0f);
}

float2 ZERNScreenCover_GetTexcoords(float4 Position)
{
	float2 Texcoords;
	Texcoords.x = (Position.x + 1.0f) * 0.5f;
    Texcoords.y = (Position.y + 1.0f) * 0.5f;

	return Texcoords;
}

float2 ZERNScreenCover_GetTexcoords(uint VertexIndex)
{
	return ZERNScreenCover_GetTexcoords(ZERNScreenCover_GetPosition(VertexIndex));
}

void ZERNScreenCover_VertexShader_Position(in uint VertexIndex : SV_VertexId, out float4 Position : SV_Position)
{
	Position = ZERNScreenCover_GetPosition(VertexIndex);
}

void ZERNScreenCover_VertexShader_PositionTexcoords(uint VertexIndex : SV_VertexId, out float4 Position : SV_Position, out float2 Texcoords : TEXCOORD0)
{
	Position = ZERNScreenCover_GetPosition(VertexIndex);
	Texcoord = ZERNScreenCover_GetTexcoords(Position);
}

#endif
