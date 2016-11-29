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
	static ZELock Lock;

	Lock.Lock();
	if (VertexLayout.GetElementCount() == 0)
	{
		static ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION,	0, ZEGR_VET_FLOAT_3, 0, 0,  ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_NORMAL,	0, ZEGR_VET_INT16_2, 0, 16, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TANGENT,	0, ZEGR_VET_INT16_2, 0, 20, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD,	0, ZEGR_VET_FLOAT_2, 0, 24, ZEGR_VU_PER_VERTEX, 0}
		};

		VertexLayout.SetElements(ElementArray, 4);
	}
	Lock.Unlock();

	return VertexLayout;
}

const ZEGRVertexLayout& ZEMDVertexInstance::GetVertexLayout()
{
	static ZEGRVertexLayout VertexLayout;
	static ZELock Lock;

	Lock.Lock();
	if (VertexLayout.GetElementCount() == 0)
	{
		static ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION,	0, ZEGR_VET_FLOAT_3, 0, 0,  ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_NORMAL,	0, ZEGR_VET_INT16_2, 0, 16, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TANGENT,	0, ZEGR_VET_INT16_2, 0, 20, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD,	0, ZEGR_VET_FLOAT_2, 0, 24, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD,	1, ZEGR_VET_FLOAT_4, 1, 0, ZEGR_VU_PER_INSTANCE, 1},
			{ZEGR_VES_TEXCOORD,	2, ZEGR_VET_FLOAT_4, 1, 16, ZEGR_VU_PER_INSTANCE, 1},
			{ZEGR_VES_TEXCOORD,	3, ZEGR_VET_FLOAT_4, 1, 32, ZEGR_VU_PER_INSTANCE, 1},
			{ZEGR_VES_TEXCOORD,	4, ZEGR_VET_FLOAT_4, 1, 48, ZEGR_VU_PER_INSTANCE, 1},
			{ZEGR_VES_TEXCOORD,	5, ZEGR_VET_FLOAT_4, 1, 64, ZEGR_VU_PER_INSTANCE, 1},
			{ZEGR_VES_TEXCOORD,	6, ZEGR_VET_FLOAT_4, 1, 80, ZEGR_VU_PER_INSTANCE, 1},
			{ZEGR_VES_TEXCOORD,	7, ZEGR_VET_FLOAT_4, 1, 96, ZEGR_VU_PER_INSTANCE, 1},
			{ZEGR_VES_TEXCOORD,	8, ZEGR_VET_FLOAT_4, 1, 112, ZEGR_VU_PER_INSTANCE, 1},
			{ZEGR_VES_TEXCOORD,	9, ZEGR_VET_FLOAT_4, 1, 128, ZEGR_VU_PER_INSTANCE, 1},
			{ZEGR_VES_TEXCOORD,	10, ZEGR_VET_UINT32_4, 1, 144, ZEGR_VU_PER_INSTANCE, 1}
		};

		VertexLayout.SetElements(ElementArray, 14);
	}
	Lock.Unlock();

	return VertexLayout;
}

const ZEGRVertexLayout& ZEMDVertexSkin::GetVertexLayout()
{
	static ZEGRVertexLayout VertexLayout;
	static ZELock Lock;

	Lock.Lock();
	if (VertexLayout.GetElementCount() == 0)
	{
		static ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION,		 0, ZEGR_VET_FLOAT_3,	0, 0,  ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BLEND_INDICES, 0, ZEGR_VET_UINT8_4,	0, 12, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_NORMAL,		 0, ZEGR_VET_INT16_2,	0, 16, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TANGENT,		 0, ZEGR_VET_INT16_2,	0, 20, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD,		 0, ZEGR_VET_FLOAT_2,	0, 24, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_BLEND_WEIGHTS, 0, ZEGR_VET_FLOAT_4,	0, 32, ZEGR_VU_PER_VERTEX, 0}
		};

		VertexLayout.SetElements(ElementArray, 6);
	}
	Lock.Unlock();

	return VertexLayout;
}
