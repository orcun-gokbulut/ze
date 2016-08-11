//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourceLOD.cpp
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

#include "ZEMDResourceLOD.h"

#include "ZEMDResource.h"
#include "ZEMDResourceMesh.h"

#include "ZEML\ZEMLReader.h"
#include "ZEGraphics\ZEGRVertexBuffer.h"
#include "ZEGraphics\ZEGRIndexBuffer.h"
#include "ZERenderer\ZERNMaterial.h"
#include "ZEUI\ZEUTextEditControl.h"

ZEMDResourceLOD::ZEMDResourceLOD() : Link(this)
{
	ResourceMesh = NULL;
	Level = 0;
	StartDistance = 0;
	EndDistance = 1000000;
	MaterialID = -1;
	VertexType = ZEMD_VT_NORMAL;
	IndexType = ZEMD_VIT_NONE;
}

ZEMDResourceLOD::~ZEMDResourceLOD()
{
	if (GetResourceMesh() != NULL)
		GetResourceMesh()->RemoveLOD(this);
}

ZEMDResource* ZEMDResourceLOD::GetResource()
{
	if (ResourceMesh == NULL)
		return NULL;
	else
		return ResourceMesh->GetResource();
}

const ZEMDResource* ZEMDResourceLOD::GetResource() const
{
	if (ResourceMesh == NULL)
		return NULL;
	else
		return ResourceMesh->GetResource();
}

ZEMDResourceMesh* ZEMDResourceLOD::GetResourceMesh()
{
	return ResourceMesh;
}

const ZEMDResourceMesh* ZEMDResourceLOD::GetResourceMesh() const
{
	return ResourceMesh;
}

void ZEMDResourceLOD::SetLevel(ZEInt32 LODLevel)
{
	this->Level = LODLevel;
}

ZEInt32 ZEMDResourceLOD::GetLevel() const 
{
	return Level;
}

void ZEMDResourceLOD::SetStartDistance(float StartDistance)
{
	this->StartDistance = StartDistance;
}

float ZEMDResourceLOD::GetStartDistance() const 
{
	return StartDistance;
}

void ZEMDResourceLOD::SetEndDistance(float EndDistance)
{
	this->EndDistance = EndDistance;
}

float ZEMDResourceLOD::GetEndDistance() const 
{
	return EndDistance;
}

void ZEMDResourceLOD::SetVertexType(ZEMDVertexType VertexType)
{
	this->VertexType = VertexType;
}

ZEMDVertexType ZEMDResourceLOD::GetVertexType() const
{
	return VertexType;
}

void ZEMDResourceLOD::SetIndexType(ZEMDVertexIndexType IndexType)
{
	this->IndexType = IndexType;
}

ZEMDVertexIndexType ZEMDResourceLOD::GetIndexType() const
{
	return IndexType;
}

void ZEMDResourceLOD::SetVertices(const ZEArray<ZEMDVertex>& Vertices)
{
	this->Vertices = Vertices;
	this->VerticesSkin.Clear();
}

const ZEArray<ZEMDVertex>& ZEMDResourceLOD::GetVertices() const
{
	return Vertices;
}

void ZEMDResourceLOD::SetVerticesSkin(const ZEArray<ZEMDVertexSkin>& VerticesSkin)
{
	this->VerticesSkin = VerticesSkin;
	this->Vertices.Clear();
}

const ZEArray<ZEMDVertexSkin>& ZEMDResourceLOD::GetVerticesSkin() const
{
	return VerticesSkin;
}

void ZEMDResourceLOD::SetIndices(const ZEArray<ZEUInt16>& Indices)
{
	this->Indices = Indices;
	this->Indices32.Clear();
}

const ZEArray<ZEUInt16>& ZEMDResourceLOD::GetIndices() const
{
	return Indices;
}

void ZEMDResourceLOD::SetIndices32(const ZEArray<ZEUInt32>& Indices)
{
	this->Indices32 = Indices;
	this->Indices.Clear();
}

const ZEArray<ZEUInt32>& ZEMDResourceLOD::GetIndices32() const
{
	return Indices32;
}

void ZEMDResourceLOD::SetAffectingBoneIds(const ZEArray<ZEUInt16>& BoneIds)
{
	AffectingBoneIds = BoneIds;
}

const ZEArray<ZEUInt16>& ZEMDResourceLOD::GetAffectingBoneIds() const
{
	return AffectingBoneIds;
}

void ZEMDResourceLOD::SetVertexBufferBase(ZEHolder<const ZEGRVertexBuffer> VertexBuffer)
{
	this->VertexBufferBase = VertexBuffer;
}

ZEHolder<const ZEGRVertexBuffer> ZEMDResourceLOD::GetVertexBufferBase() const
{
	return VertexBufferBase;
}

void ZEMDResourceLOD::SetVertexBufferNormals(ZEHolder<const ZEGRVertexBuffer> VertexBuffer)
{
	this->VertexBufferNormals = VertexBuffer;
}

ZEHolder<const ZEGRVertexBuffer> ZEMDResourceLOD::GetVertexBufferNormals() const
{
	return VertexBufferNormals;
}

void ZEMDResourceLOD::SetVertexBufferSkin(ZEHolder<const ZEGRVertexBuffer> VertexBuffer)
{
	this->VertexBufferSkin = VertexBuffer;
}

ZEHolder<const ZEGRVertexBuffer> ZEMDResourceLOD::GetVertexBufferSkin() const
{
	return VertexBufferSkin;
}

void ZEMDResourceLOD::SetVertexBufferExtra(ZEHolder<const ZEGRVertexBuffer> VertexBuffer)
{
	this->VertexBufferExtra = VertexBuffer;
}

ZEHolder<const ZEGRVertexBuffer> ZEMDResourceLOD::GetVertexBufferExtra() const
{
	return VertexBufferExtra;
}

void ZEMDResourceLOD::SetIndexBuffer(ZEHolder<const ZEGRIndexBuffer> IndexBuffer)
{
	this->IndexBuffer = IndexBuffer;
}

ZEHolder<const ZEGRIndexBuffer> ZEMDResourceLOD::GetIndexBuffer() const
{
	return IndexBuffer;
}

const ZEArray<ZEMDResourceDraw>& ZEMDResourceLOD::GetDraws() const
{
	return Draws;
}

void ZEMDResourceLOD::AddDraw(const ZEMDResourceDraw& Draw)
{
	Draws.Add(Draw);
	Draws.GetLastItem().LOD = this;
}

void ZEMDResourceLOD::RemoveDraw(ZESize Index)
{
	Draws[Index].LOD = NULL;
	Draws.Remove(Index);
}

void ZEMDResourceLOD::GenerateBuffers()
{
	if (GetVertexType() == ZEMD_VT_NORMAL)
		VertexBufferBase = ZEGRVertexBuffer::CreateResource(Vertices.GetCount(), sizeof(ZEMDVertex), ZEGR_RU_GPU_READ_ONLY, Vertices.GetCArray()).GetPointer();
	else
		VertexBufferBase = ZEGRVertexBuffer::CreateResource(VerticesSkin.GetCount(), sizeof(ZEMDVertexSkin), ZEGR_RU_GPU_READ_ONLY, VerticesSkin.GetCArray()).GetPointer();
}

bool ZEMDResourceLOD::Unserialize(const ZEMLReaderNode& LODNode)
{
	zeCheckError(!LODNode.IsValid(), false, "Invalid LOD node.");
	zeCheckError(LODNode.GetName() != "LOD", false, "Invalid LOD node name.");

	Vertices.Clear();
	VerticesSkin.Clear();
	Indices.Clear();
	AffectingBoneIds.Clear();

	IndexBuffer = NULL;
	VertexBufferBase = NULL;
	VertexBufferNormals = NULL;
	VertexBufferSkin = NULL;
	VertexBufferExtra = NULL;

	SetLevel(LODNode.ReadInt32("Level", LODNode.ReadInt32("LODLevel", 0)));
	SetStartDistance(LODNode.ReadFloat("StartDistance", (float)LODNode.ReadInt32("LODStartDistance", GetLevel() * 30)));
	SetEndDistance(LODNode.ReadFloat("EndDistance", (float)LODNode.ReadInt32("LODEndDistance", 100000)));
	SetVertexType((ZEMDVertexType)LODNode.ReadInt32("VertexType", ZEMD_VT_NORMAL));
	SetIndexType((ZEMDVertexIndexType)LODNode.ReadInt32("IndexType", ZEMD_VIT_NONE));
	MaterialID = LODNode.ReadInt32("MaterialId", 0);

	ZEString MaterialFileName = LODNode.ReadString("MaterialFilePath");

	if (GetVertexType() == ZEMD_VT_NORMAL)
	{
		Vertices.SetCount(LODNode.ReadDataSize("Vertices") / sizeof(ZEMDVertex));
		if (!LODNode.ReadDataItems("Vertices", Vertices.GetCArray(), sizeof(ZEMDVertex), Vertices.GetCount()))
			return false;

	}
	else if (GetVertexType() == ZEMD_VT_SKINNED)
	{
		VerticesSkin.SetCount(LODNode.ReadDataSize("Vertices") / sizeof(ZEMDVertexSkin));
		if (!LODNode.ReadDataItems("Vertices", VerticesSkin.GetCArray(), sizeof(ZEMDVertexSkin), VerticesSkin.GetCount()))
			return false;

		AffectingBoneIds.SetCount(LODNode.ReadDataSize("AffectingBoneIds") / sizeof(ZEUInt32));
		if (!LODNode.ReadDataItems("AffectingBoneIds", AffectingBoneIds.GetCArray(), sizeof(ZEUInt32), AffectingBoneIds.GetCount()))
			return false;

	}

	if (GetIndexType() == ZEMD_VIT_16BIT)
	{
		Indices.SetCount(LODNode.ReadDataSize("Indices") / sizeof(ZEUInt16));
		if (!LODNode.ReadDataItems("Indices", Indices.GetCArray(), sizeof(ZEUInt16), Indices.GetCount()))
			return false;
	}
	else if (GetIndexType() == ZEMD_VIT_32BIT)
	{
		Indices32.SetCount(LODNode.ReadDataSize("Indices32") / sizeof(ZEUInt32));
		if (!LODNode.ReadDataItems("Indices32", Indices32.GetCArray(), sizeof(ZEUInt32), Indices32.GetCount()))
			return false;
	}

	ZEMLReaderNode DrawsNode = LODNode.GetNode("Draws");
	if (DrawsNode.IsValid())
	{
		ZESize DrawsNodeCount = LODNode.GetNodeCount("Draws");
		for (ZESize I = 0; I < DrawsNodeCount; I++)
		{
			ZEMLReaderNode DrawNode = DrawsNode.GetNode("Draw", I);
			
			ZEMDResourceDraw Draw;
			if (!Draw.Unserialize(DrawNode))
				return false;
			
			Draws.Add(Draw);
		}
	}

	if (Draws.GetCount() == 0)
	{
		ZEMDResourceDraw Draw;
		if (GetVertexType() == ZEMD_VT_NORMAL)
			Draw.SetCount(Vertices.GetCount());
		else
			Draw.SetCount(VerticesSkin.GetCount());

		Draw.SetMaterialFileName(MaterialFileName);
		Draws.Add(Draw);
	}

	GenerateBuffers();

	return true;
}

bool ZEMDResourceLOD::Serialize(ZEMLWriterNode& LODNode) const
{
	return false; //Implementation required.
}

ZEMDResourceLOD* ZEMDResourceLOD::CreateInstance()
{
	return new ZEMDResourceLOD();
}
