//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - CanvasBrush.cpp
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
#include "CanvasBrush.h"

ZE_ENTITY_DESCRIPTION_START(ZECanvasBrush, ZEEntity, ZE_ERA_BOTH, "", "Canvas brush")
	ZE_ENTITY_NOATTRIBUTE(ZECanvasBrush)
ZE_ENTITY_DESCRIPTION_END(ZECanvasBrush)

bool ZECanvasBrush::IsDrawable()
{
	return true;
}

void ZECanvasBrush::UpdateCanvas()
{
	if (RenderList.VertexBuffer != NULL)
	{
		((ZEStaticVertexBuffer*)RenderList.VertexBuffer)->Destroy();
		delete RenderList.VertexBuffer;
	}
	
	if (!Canvas.IsEmpty())
	{
		ZEAABoundingBox BoundingBox;
		Canvas.CalculateBoundingBox(BoundingBox);
		SetLocalBoundingBox(BoundingBox);
		RenderList.VertexBuffer = Canvas.CreateStaticVertexBuffer();
		UpdateBoundingBox = true;
		UpdateBoundingSphere = true;
	}
}

void ZECanvasBrush::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
	if (RenderList.VertexBuffer != NULL)
	{
		RenderList.Lights.SetCount(Lights.GetCount());
		for (size_t I = 0; I < Lights.GetCount(); I++)
			RenderList.Lights[I] = Lights[I];
		RenderList.PrimitiveCount = Canvas.Vertices.GetCount() / 3;
		RenderList.PrimitiveType = PrimitiveType;
		RenderList.WorldMatrix = GetWorldTransform();
		Renderer->AddToRenderList(&RenderList);
	}
}

void ZECanvasBrush::Deinitialize()
{
	Canvas.Clean();
}

ZECanvasBrush::ZECanvasBrush()
{
	SetBoundingVolumeMechanism(ZE_BVM_USELOCALONLY);
	RenderList.SetZero();
	Material.SetZero();
	PrimitiveType = ZE_RLPT_TRIANGLE;
	RenderList.VertexType = ZE_VT_SIMPLEVERTEX;
	RenderList.Flags = ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM | ZE_RLF_ENABLE_WORLD_TRANSFORM | ZE_RLF_ENABLE_ZCULLING;
	RenderList.Material = &Material;
}

ZECanvasBrush::~ZECanvasBrush()
{
	if (RenderList.VertexBuffer != NULL)
		delete RenderList.VertexBuffer;
}
