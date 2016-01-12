//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDVertex.cpp
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

#include "ZEMDVertex.h"

#include "ZEGraphics\ZEGRVertexLayout.h"

const ZEGRVertexLayout& ZEMDVertex::GetVertexLayout()
{
	static ZEGRVertexLayout VertexLayout;
	if (VertexLayout.GetElementCount() == 0)
	{
		static ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION,	0, ZEGR_VET_FLOAT3, 0, 0,  ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_NORMAL,	0, ZEGR_VET_FLOAT3, 0, 12, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TANGENT,	0, ZEGR_VET_FLOAT3, 0, 24, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BINORMAL,	0, ZEGR_VET_FLOAT3, 0, 36, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD,	0, ZEGR_VET_FLOAT2, 0, 48, ZEGR_VU_PER_VERTEX, 0}
		};

		VertexLayout.SetElements(ElementArray, 5);
	}

	return VertexLayout;
}


const ZEGRVertexLayout& ZEMDVertexSkin::GetVertexLayout()
{
	static ZEGRVertexLayout VertexLayout;
	if (VertexLayout.GetElementCount() == 0)
	{
		static ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION,	0, ZEGR_VET_FLOAT3, 0, 0,  ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_NORMAL,	0, ZEGR_VET_FLOAT3, 0, 12, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TANGENT,	0, ZEGR_VET_FLOAT3, 0, 24, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BINORMAL,	0, ZEGR_VET_FLOAT3, 0, 36, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD,	0, ZEGR_VET_FLOAT2, 0, 48, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BLEND_INDEXES, 0, ZEGR_VET_UINT4,	 0, 56, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BLEND_WEIGHTS, 0, ZEGR_VET_FLOAT4, 0, 60, ZEGR_VU_PER_VERTEX, 0},
		};

		VertexLayout.SetElements(ElementArray, 4);
	}

	return VertexLayout;
}
