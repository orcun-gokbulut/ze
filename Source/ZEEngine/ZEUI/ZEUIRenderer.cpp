//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEUIRenderer.cpp
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

#include "ZEUIRenderer.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEVertexDeclaration.h"
#include "ZEGraphics/ZEFixedMaterial.h"
#include "ZEGraphics/ZEVertexTypes.h"
#include "ZEGraphics/ZERenderCommand.h"
#include "ZEGraphics/ZEVertexBuffer.h"

ZEInt CompareCommandOrder(const ZERenderCommand* Command1, const ZERenderCommand* Command2)
{
	if(Command1->Order > Command2->Order)
		return 1;
	else if(Command1->Order < Command2->Order)
		return -1;
	else
		return 0;
}

ZEUIRenderer::ZEUIRenderer()
{
	VertexDeclaration = NULL;
	DefaultMaterial = NULL;
}

ZEUIRenderer::~ZEUIRenderer()
{
	Deinitialize();
}

void ZEUIRenderer::Initialize()
{
	if (VertexDeclaration == NULL)
		VertexDeclaration = ZEUIVertex::GetVertexDeclaration();

	if (DefaultMaterial == NULL)
	{
		DefaultMaterial = ZEFixedMaterial::CreateInstance();
		((ZEFixedMaterial*)DefaultMaterial)->SetLightningEnabled(false);
		((ZEFixedMaterial*)DefaultMaterial)->SetAmbientEnabled(true);
		((ZEFixedMaterial*)DefaultMaterial)->SetAmbientColor(ZEVector3(1.0f, 1.0f, 0.0f));
		((ZEFixedMaterial*)DefaultMaterial)->UpdateMaterial();
	}

	ZEMatrix4x4::CreateViewPortTransform(ScreenTransform, 0.0f, (float)zeGraphics->GetScreenWidth(), 0.0f, (float)zeGraphics->GetScreenHeight(), 0.0f, 1.0f);
	
}

void ZEUIRenderer::Deinitialize()
{
	for (ZESize I = 0; I < RenderCommands.GetCount(); I++)
		if (RenderCommands[I].VertexBuffer != NULL)
			delete RenderCommands[I].VertexBuffer;

	if (VertexDeclaration != NULL)
	{
		VertexDeclaration->Release();
		VertexDeclaration = NULL;
	}

	if (DefaultMaterial != NULL)
	{
		DefaultMaterial->Release();
		DefaultMaterial = NULL;
	}

}

void ZEUIRenderer::Destroy()
{
	delete this;
}

void ZEUIRenderer::AddRectangle(const ZEUIRectangle& Rectangle)
{
	for (ZESize I = 0; I < RenderCommands.GetCount(); I++)
		if (RenderCommands[I].Material == Rectangle.Material || (Rectangle.Material == NULL && RenderCommands[I].Material == DefaultMaterial))
		{
			RenderCommands[I].PrimitiveCount += 2;
			ZEUIVertex* Buffer = ((ZEArrayVertexBuffer<ZEUIVertex>*)RenderCommands[I].VertexBuffer)->MassAdd(6);
			Rectangle.ConvertToVertices(Buffer);
			return;
		}

	ZERenderCommand* NewRenderCommand = RenderCommands.Add();
	NewRenderCommand->WorldMatrix = ScreenTransform;
	NewRenderCommand->Material = Rectangle.Material;
	NewRenderCommand->Pipeline = ZE_RORP_2D;
	NewRenderCommand->VertexBuffer = new ZEArrayVertexBuffer<ZEUIVertex>();
	NewRenderCommand->Flags = ZE_ROF_ENABLE_Z_CULLING | ZE_ROF_ENABLE_WORLD_TRANSFORM;
	NewRenderCommand->PrimitiveType = ZE_ROPT_TRIANGLE;
	NewRenderCommand->VertexDeclaration = VertexDeclaration;
	NewRenderCommand->VertexBufferOffset = 0;
	NewRenderCommand->IndexBuffer = NULL;
	NewRenderCommand->PrimitiveCount = 2;
	NewRenderCommand->Order = (float)Rectangle.ZOrder;
	ZEUIVertex* Buffer = ((ZEArrayVertexBuffer<ZEUIVertex>*)NewRenderCommand->VertexBuffer)->MassAdd(6);
	Rectangle.ConvertToVertices(Buffer);
}

void ZEUIRenderer::Render(ZERenderer* Renderer)
{
	RenderCommands.Sort(&CompareCommandOrder);
	
	for (ZESize I = 0; I < RenderCommands.GetCount(); I++)
	{
		if (RenderCommands[I].Material == NULL)
			RenderCommands[I].Material = DefaultMaterial;
		Renderer->AddToRenderList(&RenderCommands[I]);
	}
}

void ZEUIRenderer::Clean()
{
	for (ZESize I = 0; I < RenderCommands.GetCount(); I++)
		if (RenderCommands[I].VertexBuffer != NULL)
		{
			((ZEArrayVertexBuffer<ZEUIVertex>*)RenderCommands[I].VertexBuffer)->Clear();
			RenderCommands[I].PrimitiveCount = 0;
		}
}

ZEUIRenderer* ZEUIRenderer::CreateInstance()
{
	return new ZEUIRenderer();
}




