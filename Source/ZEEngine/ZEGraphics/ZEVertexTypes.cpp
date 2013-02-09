//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVertexTypes.cpp
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

#include "ZEVertexTypes.h"

ZEVertexLayout ZESimpleVertex::Layout;
const ZEVertexLayout&	ZESimpleVertex::GetVertexLayout()
{
	static bool Initialized = false;
	static const ZEVertexElement VertexElements[] = 
	{
		{"POSITION",	0, ZE_VET_FLOAT3, 0, 0,		ZE_VU_PER_VERTEX, 0},
		{"NORMAL",		0, ZE_VET_FLOAT3, 0, 12,	ZE_VU_PER_VERTEX, 0},
		{"TEXCOORD",	0, ZE_VET_FLOAT2, 0, 24,	ZE_VU_PER_VERTEX, 0},
		{"COLOR",		0, ZE_VET_FLOAT4, 0, 32,	ZE_VU_PER_VERTEX, 0}
	};

	if (!Initialized)
	{
		Layout.SetLayout(VertexElements, 4);
		Initialized = true;
	}

	return Layout;
}

ZEVertexLayout ZEInteriorVertex::Layout;
const ZEVertexLayout& ZEInteriorVertex::GetVertexLayout()
{
	static bool Initialized = false;
	static const ZEVertexElement VertexElements[] = 
	{
		{"POSITION",	0, ZE_VET_FLOAT3, 0, 0,		ZE_VU_PER_VERTEX, 0},
		{"NORMAL",		0, ZE_VET_FLOAT3, 0, 12,	ZE_VU_PER_VERTEX, 0},
		{"TANGENT",		0, ZE_VET_FLOAT3, 0, 24,	ZE_VU_PER_VERTEX, 0},
		{"BINORMAL",	0, ZE_VET_FLOAT3, 0, 36,	ZE_VU_PER_VERTEX, 0},
		{"TEXCOORD",	0, ZE_VET_FLOAT2, 0, 48,	ZE_VU_PER_VERTEX, 0},
	};

	if (!Initialized)
	{
		Layout.SetLayout(VertexElements, 5);
		Initialized = true;
	}

	return Layout;
}

ZEVertexLayout ZEModelVertex::Layout;
const ZEVertexLayout& ZEModelVertex::GetVertexLayout()
{
	static bool Initialized = false;
	static const ZEVertexElement VertexElements[] = 
	{
		{"POSITION",	0, ZE_VET_FLOAT3, 0, 0,		ZE_VU_PER_VERTEX, 0},
		{"NORMAL",		0, ZE_VET_FLOAT3, 0, 12,	ZE_VU_PER_VERTEX, 0},
		{"TANGENT",		0, ZE_VET_FLOAT3, 0, 24,	ZE_VU_PER_VERTEX, 0},
		{"BINORMAL",	0, ZE_VET_FLOAT3, 0, 36,	ZE_VU_PER_VERTEX, 0},
		{"TEXCOORD",	0, ZE_VET_FLOAT2, 0, 48,	ZE_VU_PER_VERTEX, 0},
	};

	if (!Initialized)
	{
		Layout.SetLayout(VertexElements, 5);
		Initialized = true;
	}

	return Layout;
}

ZEVertexLayout ZESkinnedModelVertex::Layout;
const ZEVertexLayout& ZESkinnedModelVertex::GetVertexLayout()
{
	static bool Initialized = false;
	static const ZEVertexElement VertexElements[] = 
	{
		{"POSITION",	0, ZE_VET_FLOAT3,	0, 0,	ZE_VU_PER_VERTEX, 0},
		{"NORMAL",		0, ZE_VET_FLOAT3,	0, 12,	ZE_VU_PER_VERTEX, 0},
		{"TANGENT",		0, ZE_VET_FLOAT3,	0, 24,	ZE_VU_PER_VERTEX, 0},
		{"BINORMAL",	0, ZE_VET_FLOAT3,	0, 36,	ZE_VU_PER_VERTEX, 0},
		{"TEXCOORD",	0, ZE_VET_FLOAT2,	0, 48,	ZE_VU_PER_VERTEX, 0},
		{"BLENDINDEX",	0, ZE_VET_UINT,		0, 56,	ZE_VU_PER_VERTEX, 0},
		{"BLENDWEIGHT",	0, ZE_VET_FLOAT4,	0, 60,	ZE_VU_PER_VERTEX, 0},

	};

	if (!Initialized)
	{
		Layout.SetLayout(VertexElements, 7);
		Initialized = true;
	}

	return Layout;
}

ZEVertexLayout ZEUIVertex::Layout;
const ZEVertexLayout& ZEUIVertex::GetVertexLayout()
{
	static bool Initialized = false;
	static const ZEVertexElement VertexElements[] = 
	{
		{"POSITION",	0, ZE_VET_FLOAT2,	0, 0,	ZE_VU_PER_VERTEX, 0},
		{"COLOR",		0, ZE_VET_FLOAT4,	0, 8,	ZE_VU_PER_VERTEX, 0},
		{"TEXCOORD",	0, ZE_VET_FLOAT2,	0, 24,	ZE_VU_PER_VERTEX, 0}
	};

	if (!Initialized)
	{
		Layout.SetLayout(VertexElements, 3);
		Initialized = true;
	}

	return Layout;
}

ZEVertexLayout ZEColoredVertex::Layout;
const ZEVertexLayout& ZEColoredVertex::GetVertexLayout()
{
	static bool Initialized = false;
	static const ZEVertexElement VertexElements[] = 
	{
		{"POSITION",	0, ZE_VET_FLOAT3,	0, 0,	ZE_VU_PER_VERTEX, 0},
		{"COLOR",		0, ZE_VET_FLOAT4,	0, 12,	ZE_VU_PER_VERTEX, 0},
	};

	if (!Initialized)
	{
		Layout.SetLayout(VertexElements, 2);
		Initialized = true;
	}

	return Layout;
}
