//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECanvasBrush.cpp
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

#include "ZECanvasBrush.h"
#include "ZECore\ZEError.h"
#include "ZEGame\ZEDrawParameters.h"
#include "ZEGraphics\ZEGraphicsModule.h"
#include "ZEGraphics\ZERenderer.h"
#include "ZEGraphics\ZERenderOrder.h"
#include "ZEGraphics\ZEVertexBuffer.h"

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
		if (VertexBuffer == NULL)
			VertexBuffer = zeGraphics->CreateStaticVertexBuffer();

		if (OldVertexCount != Canvas.Vertices.GetCount())
		{
			OldVertexCount = Canvas.Vertices.GetCount();
			VertexBuffer->Release();
			if (!VertexBuffer->Create(Canvas.GetBufferSize()))
				zeCriticalError("Canvas Brush", "Can not create Static Vertex Buffer.");
		}
		
		void* Buffer = VertexBuffer->Lock();
		memcpy(Buffer, Canvas.GetVertexBuffer(), Canvas.GetBufferSize());
		VertexBuffer->Unlock();
	
		ZEAABoundingBox BoundingBox;
		Canvas.CalculateBoundingBox(BoundingBox);
		SetLocalBoundingBox(BoundingBox);
		RenderOrder.VertexBuffer = Canvas.CreateStaticVertexBuffer();

		DirtyFlags |= ZE_EDF_WORLD_BOUNDING_BOX | ZE_EDF_WORLD_BOUNDING_SPHERE;
	}
}

void ZECanvasBrush::Draw(ZEDrawParameters* DrawParameters)
{
	if (RenderOrder.VertexBuffer != NULL)
	{
		RenderOrder.Lights.SetCount(DrawParameters->Lights.GetCount());
		for (size_t I = 0; I < DrawParameters->Lights.GetCount(); I++)
			RenderOrder.Lights[I] = DrawParameters->Lights[I];

		switch(PrimitiveType)
		{
			case ZE_ROPT_POINT:
				RenderOrder.PrimitiveCount = Canvas.Vertices.GetCount();
				break;			
			case ZE_ROPT_LINE:
				RenderOrder.PrimitiveCount = Canvas.Vertices.GetCount() / 2;
				break;			
			case ZE_ROPT_TRIANGLE:
				RenderOrder.PrimitiveCount = Canvas.Vertices.GetCount() / 3;
				break;			
			case ZE_ROPT_TRIANGLESTRIPT:
				RenderOrder.PrimitiveCount = Canvas.Vertices.GetCount() - 2;
				break;			
			default:
				RenderOrder.PrimitiveCount = 0;
				break;
		}

		RenderOrder.Material = Material;
		RenderOrder.PrimitiveType = PrimitiveType;
		RenderOrder.WorldMatrix = GetWorldTransform();
		DrawParameters->Renderer->AddToRenderList(&RenderOrder);
	}
}

void ZECanvasBrush::Deinitialize()
{
	if (VertexBuffer != NULL)
		VertexBuffer->Destroy();

	Canvas.Clean();
}

void ZECanvasBrush::Tick(float ElapsedTime)
{
	if (Material != NULL)
		Material->AdvanceAnimation(ElapsedTime);
}

ZECanvasBrush::ZECanvasBrush()
{
	VertexBuffer = NULL;
	OldVertexCount = 0;
	SetBoundingVolumeMechanism(ZE_BVM_USE_LOCAL_ONLY);
	RenderOrder.SetZero();
	Material = NULL;
	PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderOrder.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	RenderOrder.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	RenderOrder.Material = Material;
}

ZECanvasBrush::~ZECanvasBrush()
{
	if (Material != NULL)
		Material->Release();
	if (RenderOrder.VertexBuffer != NULL)
		((ZEStaticVertexBuffer*)RenderOrder.VertexBuffer)->Release();
}

#include "ZECanvasBrush.h.zpp"

ZEEntityRunAt ZECanvasBrushDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}

