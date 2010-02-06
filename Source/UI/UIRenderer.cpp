//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - UIRenderer.cpp
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

#include "UIRenderer.h"
#include "Graphics/GraphicsModule.h"
#include "Graphics/VertexDeclaration.h"
#include "Graphics/FixedMaterial.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/RenderOrder.h"
#include "Graphics/VertexBuffer.h"

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
		((ZEFixedMaterial*)DefaultMaterial)->SetZero();
		((ZEFixedMaterial*)DefaultMaterial)->SetLightningEnabled(false);
		((ZEFixedMaterial*)DefaultMaterial)->SetAmbientEnabled(true);
		((ZEFixedMaterial*)DefaultMaterial)->SetAmbientColor(ZEVector3(1.0f, 1.0f, 0.0f));
		((ZEFixedMaterial*)DefaultMaterial)->UpdateMaterial();
	}

	ZEMatrix4x4::CreateViewPortTransform(ScreenTransform, 0.0f, zeGraphics->GetScreenWidth(), 0.0f, zeGraphics->GetScreenHeight(), 0.0f, 1.0f);
	
}

void ZEUIRenderer::Deinitialize()
{
	for (size_t I = 0; I < RenderOrders.GetCount(); I++)
		if (RenderOrders[I].VertexBuffer != NULL)
			delete RenderOrders[I].VertexBuffer;

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
	for (size_t I = 0; I < RenderOrders.GetCount(); I++)
		if (RenderOrders[I].Material == Rectangle.Material || (Rectangle.Material == NULL && RenderOrders[I].Material == DefaultMaterial))
		{
			RenderOrders[I].PrimitiveCount += 2;
			ZEUIVertex* Buffer = ((ZEArrayVertexBuffer<ZEUIVertex>*)RenderOrders[I].VertexBuffer)->Vertices.MassAdd(6);
			Rectangle.ConvertToVertices(Buffer);
			return;
		}

	ZERenderOrder* NewRenderOrder = RenderOrders.Add();
	NewRenderOrder->WorldMatrix = ScreenTransform;
	NewRenderOrder->Material = Rectangle.Material;
	NewRenderOrder->VertexBuffer = new ZEArrayVertexBuffer<ZEUIVertex>();
	NewRenderOrder->Flags = ZE_ROF_ENABLE_ZCULLING | ZE_ROF_ENABLE_WORLD_TRANSFORM;
	NewRenderOrder->PrimitiveType = ZE_ROPT_TRIANGLE;
	NewRenderOrder->VertexDeclaration = VertexDeclaration;
	NewRenderOrder->VertexBufferOffset = 0;
	NewRenderOrder->IndexBuffer = NULL;
	NewRenderOrder->PrimitiveCount = 2;
	ZEUIVertex* Buffer = ((ZEArrayVertexBuffer<ZEUIVertex>*)NewRenderOrder->VertexBuffer)->Vertices.MassAdd(6);
	Rectangle.ConvertToVertices(Buffer);
}

void ZEUIRenderer::Render(ZERenderer* Renderer)
{
	
	for (size_t I = 0; I < RenderOrders.GetCount(); I++)
	{
		if (RenderOrders[I].Material == NULL)
			RenderOrders[I].Material = DefaultMaterial;
		Renderer->AddToRenderList(&RenderOrders[I]);
	}
}

void ZEUIRenderer::Clean()
{
	for (size_t I = 0; I < RenderOrders.GetCount(); I++)
		if (RenderOrders[I].VertexBuffer != NULL)
		{
			((ZEArrayVertexBuffer<ZEUIVertex>*)RenderOrders[I].VertexBuffer)->Vertices.Clear();
			RenderOrders[I].PrimitiveCount = 0;
		}
}

ZEUIRenderer* ZEUIRenderer::CreateInstance()
{
	return new ZEUIRenderer();
}
