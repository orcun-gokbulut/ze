//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - VertexTypes.cpp
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

#include "VertexTypes.h"
#include "VertexDeclaration.h"

ZEVertexDeclaration* ZESimpleVertex::VertexDeclaration;
ZEVertexDeclaration* ZESimpleVertex::GetVertexDeclaration()
{
	if (VertexDeclaration != NULL)
		return VertexDeclaration;

	VertexDeclaration = ZEVertexDeclaration::CreateInstance();

	ZEVertexElement ElementArray[] = {
										{ZE_VES_POSITION, ZE_VET_FLOAT3, 0},
										{ZE_VES_NORMAL, ZE_VET_FLOAT3, 0},
										{ZE_VES_TEXTCOORD, ZE_VET_FLOAT2, 0},
										{ZE_VES_END, ZE_VET_END, 0}
									};


	if (!VertexDeclaration->Create(ElementArray))
	{
		VertexDeclaration->Destroy();
		VertexDeclaration = NULL;
	}

	return VertexDeclaration;
}

ZEVertexDeclaration* ZEMapVertex::VertexDeclaration = NULL;
ZEVertexDeclaration* ZEMapVertex::GetVertexDeclaration()
{
	if (VertexDeclaration != NULL)
		return VertexDeclaration;

	VertexDeclaration = ZEVertexDeclaration::CreateInstance();

	ZEVertexElement ElementArray[] = {
										{ZE_VES_POSITION, ZE_VET_FLOAT3, 0},
										{ZE_VES_NORMAL, ZE_VET_FLOAT3, 0},
										{ZE_VES_TANGENT, ZE_VET_FLOAT3, 0},
										{ZE_VES_BINORMAL, ZE_VET_FLOAT3, 0},
										{ZE_VES_TEXTCOORD, ZE_VET_FLOAT2, 0},
										{ZE_VES_END, ZE_VET_END, 0}
									};
	
	if (!VertexDeclaration->Create(ElementArray))
	{
		VertexDeclaration->Destroy();
		VertexDeclaration = NULL;
	}

	return VertexDeclaration;
}

ZEVertexDeclaration* ZEModelVertex::VertexDeclaration = NULL;
ZEVertexDeclaration* ZEModelVertex::GetVertexDeclaration()
{
	if (VertexDeclaration != NULL)
		return VertexDeclaration;

	VertexDeclaration = ZEVertexDeclaration::CreateInstance();

	ZEVertexElement ElementArray[] = {
										{ZE_VES_POSITION, ZE_VET_FLOAT3, 0},
										{ZE_VES_NORMAL, ZE_VET_FLOAT3, 0},
										{ZE_VES_TANGENT, ZE_VET_FLOAT3, 0},
										{ZE_VES_BINORMAL, ZE_VET_FLOAT3, 0},
										{ZE_VES_TEXTCOORD, ZE_VET_FLOAT2, 0},
										{ZE_VES_END, ZE_VET_END, 0}
									};

	if (!VertexDeclaration->Create(ElementArray))
	{
		VertexDeclaration->Destroy();
		VertexDeclaration = NULL;
	}

	return VertexDeclaration;
}

ZEVertexDeclaration* ZESkinnedModelVertex::VertexDeclaration = NULL;
ZEVertexDeclaration* ZESkinnedModelVertex::GetVertexDeclaration()
{
	if (VertexDeclaration != NULL)
		return VertexDeclaration;

	VertexDeclaration = ZEVertexDeclaration::CreateInstance();

	ZEVertexElement ElementArray[] = {
										{ZE_VES_POSITION, ZE_VET_FLOAT3, 0},
										{ZE_VES_NORMAL, ZE_VET_FLOAT3, 0},
										{ZE_VES_TANGENT, ZE_VET_FLOAT3, 0},
										{ZE_VES_BINORMAL, ZE_VET_FLOAT3, 0},
										{ZE_VES_TEXTCOORD, ZE_VET_FLOAT2, 0},
										{ZE_VES_BLENDINDEX, ZE_VET_BYTE4, 0},
										{ZE_VES_BLENDWEIGHT, ZE_VET_FLOAT4, 0},
										{ZE_VES_END, ZE_VET_END, 0}
									};

	if (!VertexDeclaration->Create(ElementArray))
	{
		VertexDeclaration->Destroy();
		VertexDeclaration = NULL;
	}

	return VertexDeclaration;
}

ZEVertexDeclaration* ZEGUIVertex::VertexDeclaration = NULL;
ZEVertexDeclaration* ZEGUIVertex::GetVertexDeclaration()
{
	if (VertexDeclaration != NULL)
		return VertexDeclaration;

	VertexDeclaration = ZEVertexDeclaration::CreateInstance();

	ZEVertexElement ElementArray[] = {
										{ZE_VES_POSITION, ZE_VET_FLOAT2, 0},
										{ZE_VES_TEXTCOORD, ZE_VET_FLOAT2, 0},
										{ZE_VES_END, ZE_VET_END, 0}
									};

	if (!VertexDeclaration->Create(ElementArray))
	{
		VertexDeclaration->Destroy();
		VertexDeclaration = NULL;
	}

	return VertexDeclaration;
}
