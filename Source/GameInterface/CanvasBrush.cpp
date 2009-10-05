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

ZEDWORD ZECanvasBrush::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_CULL;
}

void ZECanvasBrush::UpdateCanvas()
{
	if (RenderOrder.VertexBuffer != NULL)
		delete RenderOrder.VertexBuffer;
	
	if (!Canvas.IsEmpty())
	{
		ZEAABoundingBox BoundingBox;
		Canvas.CalculateBoundingBox(BoundingBox);
		SetLocalBoundingBox(BoundingBox);
		RenderOrder.VertexBuffer = Canvas.CreateStaticVertexBuffer();
		UpdateBoundingBox = true;
		UpdateBoundingSphere = true;
	}
}

void ZECanvasBrush::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
	if (RenderOrder.VertexBuffer != NULL)
	{
		RenderOrder.Lights.SetCount(Lights.GetCount());
		for (size_t I = 0; I < Lights.GetCount(); I++)
			RenderOrder.Lights[I] = Lights[I];
		switch(PrimitiveType)
		{
			case ZE_RLPT_POINT:
				RenderOrder.PrimitiveCount = Canvas.Vertices.GetCount();
				break;			
			case ZE_RLPT_LINE:
				RenderOrder.PrimitiveCount = Canvas.Vertices.GetCount() / 2;
				break;			
			case ZE_RLPT_TRIANGLE:
				RenderOrder.PrimitiveCount = Canvas.Vertices.GetCount() / 3;
				break;			
			case ZE_RLPT_TRIANGLESTRIPT:
				RenderOrder.PrimitiveCount = Canvas.Vertices.GetCount() - 2;
				break;			
			default:
				RenderOrder.PrimitiveCount = 0;
				break;
		}

		RenderOrder.Material = Material;
		RenderOrder.PrimitiveType = PrimitiveType;
		RenderOrder.WorldMatrix = GetWorldTransform();
		Renderer->AddToRenderOrder(&RenderOrder);
	}
}

void ZECanvasBrush::Deinitialize()
{
	Canvas.Clean();
}

void ZECanvasBrush::Tick(float ElapsedTime)
{
	if (Material != NULL)
		Material->AdvanceAnimation(ElapsedTime);
}

ZECanvasBrush::ZECanvasBrush()
{
	SetBoundingVolumeMechanism(ZE_BVM_USELOCALONLY);
	RenderOrder.SetZero();
	Material = NULL;
	PrimitiveType = ZE_RLPT_TRIANGLE;
	RenderOrder.VertexDeclaration = ZESimpleVertex::GetVertexDeclaration();
	RenderOrder.Flags = ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM | ZE_RLF_ENABLE_WORLD_TRANSFORM | ZE_RLF_ENABLE_ZCULLING;
	RenderOrder.Material = Material;
}

ZECanvasBrush::~ZECanvasBrush()
{
	if (Material != NULL)
		Material->Release();
	if (RenderOrder.VertexBuffer != NULL)
		delete RenderOrder.VertexBuffer;
}

#include "CanvasBrush.h.zpp"
