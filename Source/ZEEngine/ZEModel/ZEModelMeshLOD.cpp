//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelMeshLOD.cpp
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

#include "ZEModelMeshLOD.h"

#include "ZEModel.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZEMath/ZEAngle.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZERenderer/ZERNShaderSlots.h"

ZEModel* ZEModelMeshLOD::GetModel()
{
	return Model;
}

ZEModelMesh* ZEModelMeshLOD::GetMesh()
{
	return Mesh;
}

void ZEModelMeshLOD::SetVertexType(ZEMDVertexType Type)
{
	VertexType = Type;
}

ZEMDVertexType ZEModelMeshLOD::GetVertexType() const
{
	return VertexType;
}

void ZEModelMeshLOD::SetVertexBufferBase(ZEHolder<const ZEGRVertexBuffer> VertexBuffer)
{
	VertexBufferBase = VertexBuffer;
}

ZEHolder<const ZEGRVertexBuffer> ZEModelMeshLOD::GetVertexBufferBase() const
{
	return VertexBufferBase;
}

void ZEModelMeshLOD::SetVertexBufferNormals(ZEHolder<const ZEGRVertexBuffer> VertexBuffer)
{
	VertexBufferNormals = VertexBufferNormals;
}

ZEHolder<const ZEGRVertexBuffer> ZEModelMeshLOD::GetVertexBufferNormals() const
{
	return VertexBufferNormals;
}

void ZEModelMeshLOD::SetVertexBufferSkin(ZEHolder<const ZEGRVertexBuffer> VertexBuffer)
{
	VertexBufferSkin = VertexBufferSkin;
}

ZEHolder<const ZEGRVertexBuffer> ZEModelMeshLOD::GetVertexBufferSkin() const
{
	return VertexBufferSkin;
}

void ZEModelMeshLOD::SetVertexBufferExtra(ZEHolder<const ZEGRVertexBuffer> VertexBuffer)
{

}

ZEHolder<const ZEGRVertexBuffer> ZEModelMeshLOD::GetVertexBufferExtra() const
{
	return NULL;
}

void ZEModelMeshLOD::SetIndexType(ZEMDVertexIndexType Type)
{
	IndexType = Type;
}

ZEMDVertexIndexType ZEModelMeshLOD::GetIndexType() const
{
	return IndexType;
}

void ZEModelMeshLOD::SetIndexBuffer(ZEHolder<const ZEGRIndexBuffer> IndexBuffer)
{
	this->IndexBuffer = IndexBuffer;
}

ZEHolder<const ZEGRIndexBuffer> ZEModelMeshLOD::GetIndexBuffer() const
{
	return IndexBuffer;
}

void ZEModelMeshLOD::SetStartDistance(float Distance)
{
	StartDistance = Distance;
}

float ZEModelMeshLOD::GetStartDistance() const
{
	return StartDistance;
}

void ZEModelMeshLOD::SetEndDistance(float Distance)
{
	EndDistance = Distance;
}

float ZEModelMeshLOD::GetEndDistance() const 
{
	return EndDistance;
}

const ZEArray<ZEModelDraw>& ZEModelMeshLOD::GetDraws()
{
	return Draws;
}

void ZEModelMeshLOD::AddDraw(const ZEModelDraw& Draw)
{
	Draws.Add(Draw);
}

void ZEModelMeshLOD::RemoveDraw(ZESize Index)
{
	Draws.Remove(Index);
}

void ZEModelMeshLOD::Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command)
{
	if (VertexBufferBase.IsNull())
		return;

	ZEGRContext* Context = RenderParameters->Context;

	Context->SetIndexBuffer(IndexBuffer);
	if (RenderParameters->Type == ZERN_DT_SHADOW)
	{
		Context->SetVertexBuffers(0, 1, &VertexBufferBase);
		if (GetVertexType() == ZEMD_VT_SKINNED)
			Context->SetVertexBuffers(2, 1, &VertexBufferSkin);
	}
	else
	{
		Context->SetVertexBuffers(0, 1, &VertexBufferBase);
		Context->SetVertexBuffers(1, 1, &VertexBufferNormals);
		if (GetVertexType() == ZEMD_VT_SKINNED)
			Context->SetVertexBuffers(2, 1, &VertexBufferSkin);
	}

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBuffer);
	ze_for_each(Draw, Draws)
	{
		Draw->GetMaterial()->SetupMaterial(RenderParameters->Context, RenderParameters->Stage);
		Context->Draw(Draw->GetCount() * 3, Draw->GetOffset() * 3);
		Draw->GetMaterial()->CleanupMaterial(RenderParameters->Context, RenderParameters->Stage);
	}
}

void ZEModelMeshLOD::LoadResource(ZEHolder<const ZEModelResource> ModelResource, const ZEModelResourceMeshLOD* Resource)
{
	this->ModelResource = ModelResource;
	SetStartDistance(Resource->GetStartDistance());
	SetEndDistance(Resource->GetEndDistance());
	SetVertexBufferBase(Resource->GetVertexBufferBase());
	SetVertexBufferNormals(Resource->GetVertexBufferNormals());
	SetVertexBufferSkin(Resource->GetVertexBufferSkin());
	SetIndexBuffer(Resource->GetIndexBuffer());
	
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEMatrix4x4));
	ConstantBuffer->SetData((void*)&Model->GetTransform());
}

ZEModelMeshLOD::ZEModelMeshLOD() : MeshLink(this)
{
	Model = NULL;
	Mesh = NULL;
	LODResource = NULL;

	StartDistance = 0;
	EndDistance = 0;
	VertexType = ZEMD_VT_NORMAL;
	IndexType = ZEMD_VIT_NONE;
}

ZEModelMeshLOD::~ZEModelMeshLOD()
{

}
