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
#include "ZEModelDraw.h"
#include "ZEMDResourceLOD.h"

#include "ZEResource/ZERSHolder.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRIndexBuffer.h"

bool ZEModelMeshLOD::Load()
{
	Unload();

	if (Resource == NULL)
		return true;

	SetStartDistance(Resource->GetStartDistance());
	SetEndDistance(Resource->GetEndDistance());
	SetVertexBufferBase(Resource->GetVertexBufferBase());
	SetVertexBufferNormals(Resource->GetVertexBufferNormals());
	SetVertexBufferSkin(Resource->GetVertexBufferSkin());
	SetIndexBuffer(Resource->GetIndexBuffer());

	ZESize DrawCount = Resource->GetDraws().GetCount();
	Draws.SetCount(DrawCount);

	for (ZESize I = 0; I < DrawCount; I++)
	{
		const ZEMDResourceDraw* ResourceDraw = &Resource->GetDraws()[I];
		Draws[I].SetMaterial(Resource->GetMaterial());
		Draws[I].SetOffset(ResourceDraw->GetOffset());
		Draws[I].SetCount(ResourceDraw->GetCount());
		Draws[I].LOD = this;
	}

	return true;
}

bool ZEModelMeshLOD::Unload()
{
	SetStartDistance(0);
	SetEndDistance(0);
	SetVertexBufferBase(NULL);
	SetVertexBufferNormals(NULL);
	SetVertexBufferSkin(NULL);
	SetIndexBuffer(NULL);

	return true;
}

ZEModel* ZEModelMeshLOD::GetModel()
{
	return Model;
}

ZEModelMesh* ZEModelMeshLOD::GetMesh()
{
	return Mesh;
}

const ZEArray<ZEModelDraw>& ZEModelMeshLOD::GetDraws()
{
	return Draws;
}

void ZEModelMeshLOD::AddDraw(const ZEModelDraw& Draw)
{
	Draws.Add(Draw);
	Draws.GetLastItem().LOD = this;
}

void ZEModelMeshLOD::RemoveDraw(ZESize Index)
{
	Draws.Remove(Index);
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

void ZEModelMeshLOD::SetResource(ZERSHolder<const ZEMDResourceLOD> Resource)
{
	this->Resource = Resource;
}

ZERSHolder<const ZEMDResourceLOD> ZEModelMeshLOD::GetResource()
{
	return Resource;
}

ZEModelMeshLOD::ZEModelMeshLOD() : MeshLink(this)
{
	Model = NULL;
	Mesh = NULL;

	StartDistance = 0;
	EndDistance = 0;
	VertexType = ZEMD_VT_NORMAL;
	IndexType = ZEMD_VIT_NONE;
}

ZEModelMeshLOD::~ZEModelMeshLOD()
{
	Unload();
}
