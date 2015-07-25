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
#include "ZEError.h"
#include "ZEGame/ZERNDrawParameters.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCommand.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZERenderer/ZERNMaterial.h"

ZEDrawFlags ZECanvasBrush::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_CULL;
}

ZECanvas* ZECanvasBrush::GetCanvas()
{
	return &Canvas;
}

void ZECanvasBrush::UpdateCanvas()
{
	if (OldVertexCount != Canvas.Vertices.GetCount())
	{
		OldVertexCount = Canvas.Vertices.GetCount();
		VertexBuffer = VertexBuffer->Create(Canvas.GetBufferSize() / sizeof(ZECanvasVertex), sizeof(ZECanvasVertex));
		if (VertexBuffer != NULL)
			zeCriticalError("Can not create Static Vertex Buffer.");
	}
	
	void* Buffer = NULL; 
	if (!VertexBuffer->Lock(&Buffer))
	{
		zeError("Cannot lock vertex buffer.");
		return;
	}

	memcpy(Buffer, Canvas.GetVertexBuffer(), Canvas.GetBufferSize());
	VertexBuffer->Unlock();
	
	ZEAABBox BoundingBox;
	Canvas.CalculateBoundingBox(BoundingBox);
	SetBoundingBox(BoundingBox);
	RenderCommand.VertexBuffer = Canvas.CreateVertexBuffer();
}

void ZECanvasBrush::Draw(ZERNDrawParameters* DrawParameters)
{
	if (RenderCommand.VertexBuffer != NULL)
	{
		RenderCommand.Lights.SetCount(DrawParameters->Lights.GetCount());
		for (ZESize I = 0; I < DrawParameters->Lights.GetCount(); I++)
			RenderCommand.Lights[I] = DrawParameters->Lights[I];

		switch(PrimitiveType)
		{
			case ZE_ROPT_POINT:
				RenderCommand.PrimitiveCount = Canvas.Vertices.GetCount();
				break;			
			case ZE_ROPT_LINE:
				RenderCommand.PrimitiveCount = Canvas.Vertices.GetCount() / 2;
				break;			
			case ZE_ROPT_TRIANGLE:
				RenderCommand.PrimitiveCount = Canvas.Vertices.GetCount() / 3;
				break;			
			case ZE_ROPT_TRIANGLE_STRIPT:
				RenderCommand.PrimitiveCount = Canvas.Vertices.GetCount() - 2;
				break;			
			default:
				RenderCommand.PrimitiveCount = 0;
				break;
		}

		RenderCommand.Material = Material;
		RenderCommand.PrimitiveType = PrimitiveType;
		RenderCommand.WorldMatrix = GetWorldTransform();
		DrawParameters->Renderer->AddCommand(&RenderCommand);
	}
}

bool ZECanvasBrush::DeinitializeSelf()
{	
	Canvas.Clean();
	VertexBuffer.Release();
	return ZEEntity::DeinitializeSelf();
}

void ZECanvasBrush::Tick(float ElapsedTime)
{

}

ZECanvasBrush::ZECanvasBrush()
{
	VertexBuffer = NULL;
	RenderCommand.RenderState = RenderState;
}

ZECanvasBrush::~ZECanvasBrush()
{

}

ZECanvasBrush* ZECanvasBrush::CreateInstance()
{
	return new ZECanvasBrush();
}
