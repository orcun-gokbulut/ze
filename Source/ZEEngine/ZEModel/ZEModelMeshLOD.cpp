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

bool ZEModelMeshLOD::Load(const ZEMDResourceLOD* Resource)
{
	Unload();

	if (Resource == NULL)
		return true;

	SetStartDistance(Resource->GetStartDistance());
	SetEndDistance(Resource->GetEndDistance());
	SetVertexType(Resource->GetVertexType());
	SetVertexBuffer(Resource->GetVertexBuffer());
	SetIndexType(Resource->GetIndexType());
	SetIndexBuffer(Resource->GetIndexBuffer());
	
	ZESize DrawCount = Resource->GetDraws().GetCount();
	Draws.SetCount(DrawCount);

	for (ZESize I = 0; I < DrawCount; I++)
	{
		const ZEMDResourceDraw* ResourceDraw = &Resource->GetDraws()[I];
		Draws[I].SetMaterial(ResourceDraw->GetMaterial());
		Draws[I].SetOffset(ResourceDraw->GetOffset());
		Draws[I].SetCount(ResourceDraw->GetCount());
		Draws[I].LOD = this;
		Draws[I].Resource = ResourceDraw;
	}

	return true;
}

bool ZEModelMeshLOD::Unload()
{
	SetStartDistance(0);
	SetEndDistance(0);
	SetVertexBuffer(NULL);
	SetIndexBuffer(NULL);

	return true;
}


ZEModelMeshLOD::ZEModelMeshLOD() : MeshLink(this)
{
	Mesh = NULL;
	StartDistance = 0;
	EndDistance = 0;
	VertexType = ZEMD_VT_NORMAL;
	IndexType = ZEMD_VIT_NONE;
}

ZEModelMeshLOD::~ZEModelMeshLOD()
{
	Unload();

	if (GetMesh() != NULL)
		GetMesh()->RemoveLOD(this);
}

ZEModel* ZEModelMeshLOD::GetModel()
{
	if (Mesh != NULL)
		return Mesh->GetModel();
	else
		return NULL;
}

const ZEModel* ZEModelMeshLOD::GetModel() const
{
	if (Mesh != NULL)
		return Mesh->GetModel();
	else
		return NULL;
}


ZEModelMesh* ZEModelMeshLOD::GetMesh()
{
	return Mesh;
}

const ZEModelMesh* ZEModelMeshLOD::GetMesh() const
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

void ZEModelMeshLOD::SetVertexBuffer(ZEHolder<const ZEGRVertexBuffer> VertexBuffer)
{
	this->VertexBuffer = VertexBuffer;
}

ZEHolder<const ZEGRVertexBuffer> ZEModelMeshLOD::GetVertexBuffer() const
{
	return VertexBuffer;
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

ZEModelMeshLOD* ZEModelMeshLOD::CreateInstance()
{
	return new ZEModelMeshLOD();
}
